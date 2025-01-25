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
    auto& ctx = cur_context();
    VkResult result;
    windowContext = pInit->windowContext;
    currentRenderPass = curFrame = 0;
    maxImageCount = std::min(
        static_cast<uint32_t>(windowContext->swapchainImageViews.size()),
        MAX_FLIGHT_COUNT);
    maxRenderPassCount = pInit->renderPassCount;
    if (ctx.queueFamilyIndex_graphics != VK_QUEUE_FAMILY_IGNORED) {
        if (result = cmdPool_graphics.create(
                ctx.queueFamilyIndex_graphics,
                VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT))
            goto CREATE_FAILED;
        cmdBuffers.resize(maxImageCount * maxRenderPassCount);
        if (result = cmdPool_graphics.allocate_buffers(cmdBuffers.data(),
                                                       cmdBuffers.size()))
            goto CREATE_FAILED;
    }
    if (ctx.queueFamilyIndex_compute != VK_QUEUE_FAMILY_IGNORED) {
        if (result = cmdPool_compute.create(
                ctx.queueFamilyIndex_presentation,
                VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT))
            goto CREATE_FAILED;
    }
    // 呈现的队列族与用于图形的队列族不一致
    if ((ctx.queueFamilyIndex_presentation != VK_QUEUE_FAMILY_IGNORED &&
         ctx.queueFamilyIndex_presentation != ctx.queueFamilyIndex_graphics &&
         windowContext->swapchainCreateInfo.imageSharingMode ==
             VK_SHARING_MODE_EXCLUSIVE) ||
        pInit->force_ownership_transfer) {
        if (result = cmdPool_presentation.create(
                ctx.queueFamilyIndex_presentation,
                VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT))
            goto CREATE_FAILED;
        if (result = cmdPool_presentation.allocate_buffers(
                cmdBuffer_presentation.data(), maxImageCount))
            goto CREATE_FAILED;
        for (auto& it : semsOwnershipIsTransfered)
            it.create();
        // 决定是否需要进行所有权转移
        ownership_transfer = true;
    }
    for (auto& it : fences)
        it.create(VK_FENCE_CREATE_SIGNALED_BIT);
    semaphores.resize((maxRenderPassCount + 1) * maxImageCount);
    for (auto& it : semaphores)
        it.create();
    return make_error_code(RenderLoopErrorEnum::Success);
CREATE_FAILED:
    print_error("RenderContext", "Initialize failed! Code:", result);
    return make_error_code(result);
}
void RenderLoop::destroy() {
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
VkResult RenderLoop::_acquire_next_image(uint32_t* index,
                                         VkSemaphore semsImageAvaliable,
                                         VkFence fence) {
    auto& ctx = cur_context();
    VkExtent2D& t = windowContext->swapchainCreateInfo.imageExtent;
    while (VkResult result =
               vkAcquireNextImageKHR(ctx.device, windowContext->swapchain,
                                     UINT64_MAX, semsImageAvaliable, fence,
                                     index)) {  // 如果获取失败则重建交换链
        switch (result) {
            case VK_SUBOPTIMAL_KHR:
            case VK_ERROR_OUT_OF_DATE_KHR:
                windowContext->recreateSwapchain();
                print_log("RenderSize", "New size:", t.width, t.height);
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
VkCommandBuffer RenderLoop::begin_render() {
    auto& ctx = cur_context();
    auto& swapchain = windowContext->swapchain;
    auto& swapchainInfo = windowContext->swapchainCreateInfo;
    // 检查是否存在旧交换链，如果存在则销毁
    if (swapchainInfo.oldSwapchain && swapchainInfo.oldSwapchain != swapchain) {
        vkDestroySwapchainKHR(ctx.device, swapchainInfo.oldSwapchain, nullptr);
        swapchainInfo.oldSwapchain = VK_NULL_HANDLE;
    }
    // 等待当前帧的栅栏，确保在这一帧的命令已完成执行
    VkResult result = fences[curFrame].wait_and_reset();
    if (result != VK_SUCCESS) {
        print_error("RenderLoop", "Wait for fence failed! Code:", result);
        curFrame = (curFrame + 1) % maxImageCount;
        return;
    }
    // 请求下一张图像，确保可用
    _acquire_next_image(&image_index,
                        semaphores[curFrame * (maxRenderPassCount + 1) + 0]);
    auto& curBuf = cmdBuffers[curFrame * maxRenderPassCount + 0];
    currentRenderPass = 0;
    result = vkResetCommandBuffer(VkCommandBuffer(curBuf), 0);
    if (result != VK_SUCCESS) {
        print_error("RenderLoop",
                    "vkResetCommandBuffer() failed! Code:", result);
    }
    curBuf.begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    return curBuf;
}
VkCommandBuffer RenderLoop::next_render_pass() {
    if (currentRenderPass + 1 >= maxRenderPassCount) {
        print_error("RenderLoop", "Too many renderpass call!");
        exit(-1);
    }
    {
        auto& curBuf = cmdBuffers[curFrame * maxImageCount + currentRenderPass];
        curBuf.end();
        // 发送渲染命令
        VkPipelineStageFlags flag =
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        VkSubmitInfo submit_info = {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = semaphores[curFrame * (maxRenderPassCount + 1) +
                                          currentRenderPass]
                                   .getPointer(),
            .pWaitDstStageMask = &flag,
            .commandBufferCount = 1,
            .pCommandBuffers = curBuf.getPointer(),
            .signalSemaphoreCount = 1};
        submit_info.pSignalSemaphores =
            semaphores[curFrame * (maxRenderPassCount + 1) + currentRenderPass +
                       1]
                .getPointer();
        VkResult result = vkQueueSubmit(cur_context().queue_graphics, 1,
                                        &submit_info, VK_NULL_HANDLE);
        if (result != VK_SUCCESS) {
            print_error("RenderLoop", "vkQueueSubmit() failed! Code:", result);
        }
    }
    currentRenderPass++;
    auto& curBuf =
        cmdBuffers[curFrame * maxRenderPassCount + currentRenderPass];
    currentRenderPass = 0;
    VkResult result =
        vkResetCommandBuffer(VkCommandBuffer(curBuf), currentRenderPass);
    if (result != VK_SUCCESS) {
        print_error("RenderLoop",
                    "vkResetCommandBuffer() failed! Code:", result);
    }
    curBuf.begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    return curBuf;
}
VkResult RenderLoop::_present_image(VkPresentInfoKHR& presentInfo) {
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
VkResult RenderLoop::_present_image_semaphore(
    VkSemaphore semaphore_renderingIsOver = VK_NULL_HANDLE) {
    VkPresentInfoKHR presentInfo = {.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
                                    .swapchainCount = 1,
                                    .pSwapchains = &windowContext->swapchain,
                                    .pImageIndices = &image_index};
    if (semaphore_renderingIsOver)
        presentInfo.waitSemaphoreCount = 1,
        presentInfo.pWaitSemaphores = &semaphore_renderingIsOver;
    return _present_image(presentInfo);
}
void RenderLoop::end_render() {
    auto& curBuf = cmdBuffers[curFrame * maxImageCount + currentRenderPass];
    curBuf.end();
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
        .signalSemaphoreCount = 1};
    VkFence fence;
    if (ownership_transfer) {
        fence = VK_NULL_HANDLE;
    } else {
        fence = VkFence(fences[curFrame]);
    }
    auto& sem =
        semaphores[curFrame * (maxRenderPassCount + 1) + currentRenderPass + 1];
    submit_info.pSignalSemaphores = sem.getPointer();
    if (VkResult result = vkQueueSubmit(cur_context().queue_graphics, 1,
                                        &submit_info, fence)) {
        print_error("RenderLoop", "vkQueueSubmit() failed! Code:", result);
        return;
    }
    if (VkResult result = _present_image_semaphore(sem)) {
        print_error("RenderLoop",
                    "_present_image_semaphore() failed! Code:", result);
        return;
    }
    return;
}
}  // namespace BL