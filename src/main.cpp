#include <core/bl_init.hpp>

BL::Context ctx;
int main() {
    {
        BL::InstanceCreateInfo instance_info{
            .pAppName = "BLVK test",
            .appVersion = VK_MAKE_API_VERSION(0, 0, 1, 0),
            .min_api_version = VK_API_VERSION_1_2,
            .isDebuging = true,
            .layerNames = {},
            .extensionNames = {},
            .pNextInstance = nullptr
        };
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
        std::array<BL::WindowContext*, 1> get_window;
        if (auto result = ctx.prepare_context(info, get_window);
            result != BL::CtxResult::SUCCESS) {
            std::cout << "Error in init" << int32_t(result) << '\n';
            exit(-1);
        }
        BL::make_current_context(ctx);
        while (!glfwWindowShouldClose(get_window[0]->pWindow)) {
            // do something
            glfwPollEvents();
        }
        ctx.cleanup();
        system("pause");
    }
}