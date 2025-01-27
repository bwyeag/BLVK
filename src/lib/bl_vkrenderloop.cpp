#include <bl_vkrenderloop.hpp>

namespace BL {
const char* RenderLoopErrorCategory::name() const noexcept {
    return "RenderLoop error";
}
std::string RenderLoopErrorCategory::message(int ev) const {
    using Enum = ContextErrorEnum;
    switch (static_cast<Enum>(ev)) {
        case Enum::Success:
            return "no error";
        default:
            return "unknown error";
    }
}
static ContextErrorCategory category;
[[nodiscard]]
std::error_code make_error_code(ContextErrorEnum e) {
    return {static_cast<int>(e), category};
}
std::error_code RenderLoop::create(const RenderLoopInit* pInit) {
    VkResult result;
    const char* message = nullptr;
    auto& ctx = cur_context();
    windowContext = pInit->windowContext;
    currentRenderPass = curFrame = 0;
    maxImageCount = std::min(
        static_cast<uint32_t>(windowContext->swapchainImageViews.size()),
        MAX_FLIGHT_COUNT);
    maxRenderPassCount = pInit->renderPassCount;
    if (ctx.queueFamilyIndex_graphics != VK_QUEUE_FAMILY_IGNORED) {
        if (result = cmdPool_graphics.create(
                ctx.queueFamilyIndex_graphics,
                VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT)) {
            message = "cmdPool_graphics";
            goto CREATE_FAILED;
        }
        cmdBuffers.resize(maxImageCount * maxRenderPassCount);
        if (result = cmdPool_graphics.allocate_buffers(cmdBuffers.data(),
                                                       cmdBuffers.size())) {
            message = "cmdBuffers";
            goto CREATE_FAILED;
        }
    }
    if (ctx.queueFamilyIndex_compute != VK_QUEUE_FAMILY_IGNORED) {
        if (result = cmdPool_compute.create(
                ctx.queueFamilyIndex_presentation,
                VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT)) {
            message = "cmdPool_compute";
            goto CREATE_FAILED;
        }
    }
    // 呈现的队列族与用于图形的队列族不一致
    if ((ctx.queueFamilyIndex_presentation != VK_QUEUE_FAMILY_IGNORED &&
         ctx.queueFamilyIndex_presentation != ctx.queueFamilyIndex_graphics &&
         windowContext->swapchainCreateInfo.imageSharingMode ==
             VK_SHARING_MODE_EXCLUSIVE) ||
        pInit->force_ownership_transfer) {
        if (result = cmdPool_presentation.create(
                ctx.queueFamilyIndex_presentation,
                VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT)) {
            message = "cmdPool_presentation";
            goto CREATE_FAILED;
        }
        if (result = cmdPool_presentation.allocate_buffers(
                cmdBuffer_presentation.data(), maxImageCount)) {
            message = "cmdBuffer_presentation";
            goto CREATE_FAILED;
        }
        for (auto& it : semsOwnershipIsTransfered)
            it.create();
        // 决定是否需要进行所有权转移
        ownership_transfer = true;
    }
    for (size_t i = 0; i < maxImageCount; ++i)
        fences[i].create(VK_FENCE_CREATE_SIGNALED_BIT);
    semaphores.resize((maxRenderPassCount + 1) * maxImageCount);
    for (auto& it : semaphores)
        it.create();
    return make_error_code(RenderLoopErrorEnum::Success);
CREATE_FAILED:
    print_error("RenderContext", "Initialize ", message,
                " failed! Code:", result);
    return make_error_code(result);
}
void RenderLoop::destroy() noexcept {
    for (size_t i = 0; i < MAX_FLIGHT_COUNT; i++) {
        std::destroy_at(&fences[i]);
        std::destroy_at(&semsOwnershipIsTransfered[i]);
        std::destroy_at(&cmdPool_graphics);
        std::destroy_at(&cmdPool_compute);
        std::destroy_at(&cmdPool_presentation);
    }
    semaphores.clear();
    windowContext = nullptr;
}
VkResult RenderLoop::acquire_next_image(uint32_t* index,
                                        VkSemaphore semsImageAvaliable,
                                        VkFence fence) {
    auto& ctx = cur_context();
    auto& swapchain = windowContext->swapchain;
    auto& swapchainInfo = windowContext->swapchainCreateInfo;
    // 检查是否存在旧交换链，如果存在则销毁
    if (swapchainInfo.oldSwapchain && swapchainInfo.oldSwapchain != swapchain) {
        vkDestroySwapchainKHR(ctx.device, swapchainInfo.oldSwapchain, nullptr);
        swapchainInfo.oldSwapchain = VK_NULL_HANDLE;
    }
    while (VkResult result =
               vkAcquireNextImageKHR(ctx.device, windowContext->swapchain,
                                     UINT64_MAX, semsImageAvaliable, fence,
                                     index)) {  // 如果获取失败则重建交换链
        VkExtent2D& t = windowContext->swapchainCreateInfo.imageExtent;
        switch (result) {
            case VK_SUBOPTIMAL_KHR:
            case VK_ERROR_OUT_OF_DATE_KHR:
                windowContext->recreateSwapchain();
                print_log("RenderLoop", "New swapchain size:", t.width,
                          t.height);
                break;
            default:
                print_error(
                    "RenderLoop",
                    "wait for image in swapchain failed! Code:", result);
                return result;
        }
    }
    return VK_SUCCESS;
}
VkCommandBuffer reset_and_begin_cmdbuffer(
    CommandBuffer& cmdBuf,
    VkCommandBufferResetFlags resetflags,
    VkCommandBufferUsageFlags cmdbufusage) {
    if (VkResult result = cmdBuf.reset()) {
        print_error("RenderLoop", "current cmd-buffer ", "reset",
                    " failed! Code:", result);
        return VK_NULL_HANDLE;
    }
    if (VkResult result =
            cmdBuf.begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT)) {
        print_error("RenderLoop", "current cmd-buffer ", "begin",
                    " failed! Code:", result);
        return VK_NULL_HANDLE;
    }
    return cmdBuf;
}
VkCommandBuffer RenderLoop::begin_render() {
    auto& ctx = cur_context();
    auto& swapchain = windowContext->swapchain;
    auto& swapchainInfo = windowContext->swapchainCreateInfo;
    // 等待当前帧的栅栏，确保在这一帧的命令已完成执行
    if (VkResult result = fences[curFrame].wait_and_reset()) {
        print_error("RenderLoop", "Wait for fence failed! Code:", result);
        curFrame = (curFrame + 1) % maxImageCount;  // 跳过当前帧
        return VK_NULL_HANDLE;
    }
    // 请求下一张图像，确保可用
    acquire_next_image(&image_index,
                       semaphores[curFrame * (maxRenderPassCount + 1) + 0]);
    auto& curBuf = cmdBuffers[curFrame * maxRenderPassCount + 0];
    currentRenderPass = 0;
    return reset_and_begin_cmdbuffer(
        curBuf, 0, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
}
VkCommandBuffer RenderLoop::next_render_pass() {
#ifdef _BL_DEBUG
    if (currentRenderPass + 1 >= maxRenderPassCount) {
        print_error("RenderLoop", "Too many renderpass call!");
        exit(-1);
    }
#endif  //_BL_DEBUG
    uint32_t pos = curFrame * maxImageCount + currentRenderPass;
    auto& curBuf = cmdBuffers[pos];
    if (VkResult result = curBuf.end()) {
        print_error("RenderLoop",
                    "current cmd-buffer end failed! Code:", result);
    }
    // 发送渲染命令
    VkPipelineStageFlags flag = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    VkSubmitInfo submit_info = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores =
            semaphores[curFrame * (maxRenderPassCount + 1) + currentRenderPass]
                .getPointer(),
        .pWaitDstStageMask = &flag,
        .commandBufferCount = 1,
        .pCommandBuffers = curBuf.getPointer(),
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = semaphores[curFrame * (maxRenderPassCount + 1) +
                                        currentRenderPass + 1]
                                 .getPointer()};
    if (VkResult result = vkQueueSubmit(cur_context().queue_graphics, 1,
                                        &submit_info, VK_NULL_HANDLE)) {
        print_error("RenderLoop", "vkQueueSubmit() failed! Code:", result);
    }
    currentRenderPass++;
    return reset_and_begin_cmdbuffer(
        cmdBuffers[pos + 1], 0, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
}
VkResult RenderLoop::present_image(VkPresentInfoKHR& presentInfo) {
    switch (VkResult result = vkQueuePresentKHR(
                cur_context().queue_presentation, &presentInfo)) {
        case VK_SUCCESS:
            return VK_SUCCESS;
        case VK_SUBOPTIMAL_KHR:
        case VK_ERROR_OUT_OF_DATE_KHR:
            return windowContext->recreateSwapchain();
        default:
            print_error(
                "RenderLoop",
                "Failed to queue the image for presentation! Error code:",
                int32_t(result));
            return result;
    }
}
VkResult RenderLoop::present_image_semaphore(
    VkSemaphore semaphore_renderingIsOver) {
    VkPresentInfoKHR presentInfo = {.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
                                    .swapchainCount = 1,
                                    .pSwapchains = &windowContext->swapchain,
                                    .pImageIndices = &image_index};
    if (semaphore_renderingIsOver) {
        presentInfo.waitSemaphoreCount = 1,
        presentInfo.pWaitSemaphores = &semaphore_renderingIsOver;
    }
    return present_image(presentInfo);
}
void RenderLoop::cmd_transfer_image_ownership(VkCommandBuffer commandBuffer) {
    VkImageMemoryBarrier imageMemoryBarrier_g2p = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        .dstAccessMask = 0,
        .oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        .srcQueueFamilyIndex = cur_context().queueFamilyIndex_graphics,
        .dstQueueFamilyIndex = cur_context().queueFamilyIndex_presentation,
        .image = windowContext->swapchainImages[image_index],
        .subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}};
    vkCmdPipelineBarrier(commandBuffer,
                         VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                         VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0,
                         nullptr, 1, &imageMemoryBarrier_g2p);
}
VkResult RenderLoop::submit_cmdbuffer_presentation(
    VkCommandBuffer commandBuffer,
    VkSemaphore semaphore_renderingIsOver,
    VkSemaphore semaphore_ownershipIsTransfered,
    VkFence fence) {
    static constexpr VkPipelineStageFlags waitDstStage =
        VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
    VkSubmitInfo submitInfo = {.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                               .commandBufferCount = 1,
                               .pCommandBuffers = &commandBuffer};
    if (semaphore_renderingIsOver)
        submitInfo.waitSemaphoreCount = 1,
        submitInfo.pWaitSemaphores = &semaphore_renderingIsOver,
        submitInfo.pWaitDstStageMask = &waitDstStage;
    if (semaphore_ownershipIsTransfered)
        submitInfo.signalSemaphoreCount = 1,
        submitInfo.pSignalSemaphores = &semaphore_ownershipIsTransfered;
    VkResult result =
        vkQueueSubmit(cur_context().queue_presentation, 1, &submitInfo, fence);
    if (result)
        print_error("RenderLoop",
                    "Failed to submit the presentation command "
                    "buffer! Code:",
                    int32_t(result));
    return result;
}
void RenderLoop::end_render() {
    auto& curBuf = cmdBuffers[curFrame * maxImageCount + currentRenderPass];
    auto& waitsem =
        semaphores[curFrame * (maxRenderPassCount + 1) + currentRenderPass];
    auto& signalsem =
        semaphores[curFrame * (maxRenderPassCount + 1) + currentRenderPass + 1];
    if (ownership_transfer)
        cmd_transfer_image_ownership(curBuf);
    if (VkResult result = curBuf.end()) {
        print_error("RenderLoop",
                    "current cmd-buffer end failed! Code:", result);
    }
    // 发送渲染命令
    VkPipelineStageFlags flag = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    VkSubmitInfo submit_info = {.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                                .waitSemaphoreCount = 1,
                                .pWaitSemaphores = waitsem.getPointer(),
                                .pWaitDstStageMask = &flag,
                                .commandBufferCount = 1,
                                .pCommandBuffers = curBuf.getPointer()};
    if (!ownership_transfer) {
        submit_info.signalSemaphoreCount = 1;
        submit_info.pSignalSemaphores = signalsem.getPointer();
    }
    if (VkResult result = vkQueueSubmit(
            cur_context().queue_graphics, 1, &submit_info,
            ownership_transfer ? VK_NULL_HANDLE : VkFence(fences[curFrame]))) {
        print_error("RenderLoop", "vkQueueSubmit() failed! Code:", result);
        return;
    }
    if (ownership_transfer) {
        if (VkResult result = cmdBuffer_presentation[curFrame].begin(
                VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT)) {
            print_error(
                "RenderLoop",
                "current present cmd-buffer begin failed! Code:", result);
            return;
        }
        cmd_transfer_image_ownership(cmdBuffer_presentation[curFrame]);
        if (VkResult result = cmdBuffer_presentation[curFrame].end()) {
            print_error("RenderLoop",
                        "current present cmd-buffer end failed! Code:", result);
            return;
        }
        submit_cmdbuffer_presentation(
            cmdBuffer_presentation[curFrame], VK_NULL_HANDLE,
            semsOwnershipIsTransfered[curFrame], fences[curFrame]);
        present_image_semaphore(semsOwnershipIsTransfered[curFrame]);
    } else {
        present_image_semaphore(signalsem);
    }
    return;
}
}  // namespace BL