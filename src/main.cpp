#include <bl_vkcontext.hpp>
#include <bl_window.hpp>

BL::Context ctx;
BL::WindowContext window;
int main() {
    {
        BL::WindowContext::initialize();
        BL::ContextInstanceInit ctxinit{
            .pAppName = "BLVK test",
            .appVersion = VK_MAKE_API_VERSION(0, 0, 1, 1),
            .minApiVersion = VK_API_VERSION_1_3,
            .isDebuging = true,
        };
        if (ctx.create_instance(&ctxinit)) {
            BL::WindowContext::initialize();
            exit(-1);
        }
        BL::make_current_context(ctx);
        BL::WindowInit_t wininit{
            .init_size_x = 800, .init_size_y = 600, .init_title = "BLVK test"};
        if (window.create_window(&wininit)) {
            BL::WindowContext::terminate();
            exit(-1);
        }
        BL::ContextDeviceInit diviceinit{.surfaceCount = 1,
                                         .surface = &window.surface};
        if (ctx.create_device(&diviceinit)) {
            BL::WindowContext::terminate();
            exit(-1);
        }
        BL::WindowContextSwapchainInit_t swapchaininit{
            .isFrameRateLimited = false,
        };
        if (window.createSwapchain(&swapchaininit)) {
            BL::WindowContext::terminate();
            exit(-1);
        }
        while (glfwWindowShouldClose(window.pWindow)) {
            while (glfwGetWindowAttrib(window.pWindow, GLFW_ICONIFIED))
                glfwWaitEvents();

            glfwPollEvents();
        }
        window.destroy();
        ctx.destroy_device();
        ctx.destroy_instance();
        BL::WindowContext::terminate();
    }
}