#include <core/bl_init.hpp>

BL::Context ctx;
int main() {
    {
        BL::ContextCreateInfo info{
            {.appVersion = VK_MAKE_API_VERSION(0, 0, 1, 0),
             .isDebuging = true,
             .min_api_version = VK_API_VERSION_1_2,
             .pAppName = "BLVK test",
             .extensionNames = {},
             .layerNames = {},
             .pNextInstance = nullptr},
            {.extensionNames = {}, .pNextDivice = nullptr}};
        std::array<std::pair<BL::WindowCreateInfo, BL::SwapchainCreateInfo>，1>
            window_info;
        window_info[0] = {
            BL::WindowCreateInfo{
                .init_size_x = 800, .init_size_y = 600, .init_title = "BLVK"},
            BL::SwapchainCreateInfo{.isFrameRateLimited = false}};
        std::array<BL::WindowContext*, 1> windows;
        if (auto result = ctx.prepare_context(info, window_info, windows);
            result != BL::CtxResult::SUCCESS) {
            std::cout << "Error in init" << int32_t(result) << '\n';
            exit(-1);
        }
        BL::make_current_context(ctx);
        while (!glfwWindowShouldClose(windows[0]->pWindow)) {
            // do something
            glfwPollEvents();
        }
        ctx.cleanup();
    }
}