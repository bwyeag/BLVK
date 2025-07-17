#include "bl_rendering_loop.hpp"
#include "bl_util.hpp"
#include "bl_vktypes.hpp"
#include "render_section.hpp"
#include <bl_contexts.hpp>
#include <bl_debug_utils.hpp>
#include <cstdint>
using namespace BLT;
WindowContext<WindowContextBase_glfw> window_context;
ContextBase context;
WindowRenderCtx<WindowContextBase_glfw> window_render_ctx;
RenderSection<WindowContextBase_glfw> window_render_sect;
debug::FpsTitle<WindowContextBase_glfw> debug_FpsTitle;
const char *s_LogType = "Test";
int main() {
  print_log(s_LogType, "Start Program");
  {
    AutoScopedTimer timer("Initial Timer");
    using wState = WindowCreateState;
    InstanceCreateInfo ci_inst{.m_pAppName = "test",
                               .m_AppVersion = VK_MAKE_API_VERSION(0, 0, 1, 0),
                               .m_MinApiVersion = VK_API_VERSION_1_3,
                               .m_isDebuging = true};
    context.create_instance(ci_inst);
    ContextTraits::set_current_context(context);
    WindowCreateInfo_glfw ci_wctx{
        .m_InitState = wState(wState::specified | wState::use_primary_monitor |
                              wState::decorated | wState::resizable),
        .m_InitSizeX = 800,
        .m_InitSizeY = 600,
        .m_InitPosX = 100,
        .m_InitPosY = 100,
        .m_InitTitle = "Test"};
    window_context.create_base(ci_wctx);
    window_context.create_surface(); // -> get_surface
    DeviceCreateInfo ci_devi{};
    auto surfaces = window_context.get_surface();
    context.create_device(ci_devi, std::span(&surfaces, 1));
    SwapchainCreateInfo ci_swch{};
    window_context.create(ci_swch);

    debug_FpsTitle.init(window_context, context);

    WindowRenderCtxCreateInfo ci_wrctx{.m_pWindowCtx = &window_context,
                                       .m_StagesNum = 1,
                                       .m_Flags = WindowRenderCtxFlagBits(0)};
    window_render_ctx.create(ci_wrctx);
    window_render_sect.create(window_context);
  }
  print_log(s_LogType, "Finish Initialization.");
  bool should_end = false;
  VkClearValue clearColor = {.color = {{1.f, 0.f, 0.f, 1.f}}};
  while (!glfwWindowShouldClose(window_context.m_pWindow) && !should_end) {
    window_render_ctx.begin(
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        [clearColor](VkCommandBuffer buf, uint32_t i) {
          VkRenderPassBeginInfo begin_info{
              .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
              .renderPass = window_render_sect.m_RenderPass,
              .framebuffer = window_render_sect.m_Framebuffers[i],
              .renderArea = {{},
                             window_context.m_SwapchainCreateInfo.imageExtent},
              .clearValueCount = 1,
              .pClearValues = &clearColor};
          window_render_sect.m_RenderPass.cmd_begin(buf, begin_info);
          window_render_sect.m_RenderPass.cmd_end(buf);
        },
        true);
    window_render_ctx.end_and_present(
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
    glfwPollEvents();
    context.update();
  }
  print_log(s_LogType, "Program End.");
  {
    window_render_sect.cleanup();
    window_render_ctx.cleanup();
    window_context.cleanup();
    context.cleanup();
  }
  print_log(s_LogType, "End Program");
}
