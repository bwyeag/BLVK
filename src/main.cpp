#include <core/bl_renderloop.hpp>

BL::Context ctx;
BL::RenderLoop loop;

auto& CreateRpwf_Screen() {
    static BL::RenderPassWithFramebuffers rpwf;
    auto& window = *loop.windowContext;

    VkAttachmentDescription attachmentDescription = {
        .format = window.swapchainCreateInfo.imageFormat,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR};
    VkAttachmentReference attachmentReference = {
        0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};
    VkSubpassDescription subpassDescription = {
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .colorAttachmentCount = 1,
        .pColorAttachments = &attachmentReference};
    VkSubpassDependency subpassDependency = {
        .srcSubpass = VK_SUBPASS_EXTERNAL,
        .dstSubpass = 0,
        .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        .srcAccessMask = 0,
        .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        .dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT};
    VkRenderPassCreateInfo renderPassCreateInfo = {
        .attachmentCount = 1,
        .pAttachments = &attachmentDescription,
        .subpassCount = 1,
        .pSubpasses = &subpassDescription,
        .dependencyCount = 1,
        .pDependencies = &subpassDependency};
    rpwf.renderPass.create(renderPassCreateInfo);

    auto CreateFramebuffers = [] (BL::WindowContext* window) {
        rpwf.framebuffers.resize(window->swapchainImageViews.size());
        VkFramebufferCreateInfo framebufferCreateInfo = {
            .renderPass = rpwf.renderPass,
            .attachmentCount = 1,
            .width = window->swapchainCreateInfo.imageExtent.width,
            .height = window->swapchainCreateInfo.imageExtent.height,
            .layers = 1};
        for (size_t i = 0; i < window->swapchainImageViews.size(); i++) {
            VkImageView attachment = window->swapchainImageViews[i];
            framebufferCreateInfo.pAttachments = &attachment;
            rpwf.framebuffers[i].create(framebufferCreateInfo);
        }
    };
    auto DestroyFramebuffers = [] (BL::WindowContext*) { rpwf.framebuffers.clear(); };
    CreateFramebuffers(&window);

    window.callback_swapchain_construct.insert(CreateFramebuffers);
    window.callback_swapchain_destroy.insert(DestroyFramebuffers);
    return rpwf;
}
int main() {
    std::array<BL::WindowContext*, 1> get_window;
    {
        BL::InstanceCreateInfo instance_info{
            .pAppName = "BLVK test",
            .appVersion = VK_MAKE_API_VERSION(0, 0, 1, 0),
            .min_api_version = VK_API_VERSION_1_2,
            .isDebuging = true,
            .layerNames = {},
            .extensionNames = {},
            .pNextInstance = nullptr};
        BL::DeviceCreateInfo device_info{};
        std::array<std::pair<BL::WindowCreateInfo, BL::SwapchainCreateInfo>, 1>
            windowInfos;
        windowInfos[0] = {
            BL::WindowCreateInfo{
                .init_size_x = 800, .init_size_y = 600, .init_title = "BLVK"},
            BL::SwapchainCreateInfo{.isFrameRateLimited = false}};
        BL::ContextCreateInfo info{.instance_info = &instance_info,
                                   .device_info = &device_info,
                                   .window_info = windowInfos};
        if (auto result = ctx.prepare_context(info, get_window);
            result != BL::CtxResult::SUCCESS) {
            std::cout << "Error in init" << int32_t(result) << '\n';
            exit(-1);
        }
        BL::make_current_context(ctx);
        BL::RenderLoopInfo loop_info{.windowContext = &ctx.windowData[0],
                                     .renderPassCount = 1,
                                     .force_ownership_transfer = false};
        loop.prepare(loop_info);
    }
    {
        auto& [renderPass, framebuffers] = CreateRpwf_Screen();
        const auto& windowSize =
            loop.windowContext->swapchainCreateInfo.imageExtent;
        VkClearValue clearColor = {.color = {1.f, 0.f, 0.f, 1.f}};  // 红色
        while (!glfwWindowShouldClose(get_window[0]->pWindow)) {
            while (glfwGetWindowAttrib(get_window[0]->pWindow, GLFW_ICONIFIED))
                glfwWaitEvents();
            auto i = loop.image_index;
            VkCommandBuffer buf = loop.begin_render();

            renderPass.cmd_begin(buf, framebuffers[i], {{}, windowSize},
                                 &clearColor,1);
            renderPass.cmd_end(buf);

            loop.end_render();
            loop.present();
            glfwPollEvents();
        }
        renderPass.destroy();
        framebuffers.clear();
        loop.cleanup();
        ctx.cleanup();
        system("pause");
    }
}