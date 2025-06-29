#include "GLFW/glfw3.h"
#include "bl_output.hpp"
#include "vulkan/vulkan_core.h"
#include <bl_contexts2.hpp>

using namespace BLT;
WindowContext<WindowContextBase_glfw> window_context;
ContextBase context;
const char *s_LogType = "Test";
int main() {
  print_log(s_LogType, "Start Program");
  {
    using wState = WindowCreateState;
    WindowCreateInfo_glfw ci_wctx{
        .m_InitState = wState(wState::specified | wState::use_primary_monitor),
        .m_InitSizeX = 800,
        .m_InitSizeY = 600,
        .m_InitTitle = "Test"};
    window_context.create_base(ci_wctx);
    InstanceCreateInfo ci_inst{.m_pAppName = "test",
                               .m_AppVersion = VK_MAKE_API_VERSION(0, 0, 1, 0),
                               .m_MinApiVersion = VK_API_VERSION_1_3,
                               .m_isDebuging = true};
    context.create_instance(ci_inst);
    window_context.create_surface(context); // -> get_surface
    DeviceCreateInfo ci_devi{};
    auto surfaces = window_context.m_Surface;
    context.create_device(ci_devi, std::span(&surfaces, 1));
    SwapchainCreateInfo ci_swch{};
    window_context.create(ci_swch, context);
  }
  print_log(s_LogType, "Finish Initialization.");
  bool should_end = false;
  while (!glfwWindowShouldClose(window_context.m_pWindow) && !should_end) {
    glfwPollEvents();
  }
  print_log(s_LogType, "Program End.");
  {
    window_context.cleanup(context);
    context.cleanup();
  }
  print_log(s_LogType, "End Program");
}
