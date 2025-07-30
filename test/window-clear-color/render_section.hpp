#include "bl_contexts.hpp"
#include <bl_vktypes.hpp>
#include <cstddef>
#include <memory>
using namespace blt;

template <typename BaseCtx, typename _Ctx = ContextTraits>
struct RenderSection {
  WindowContext<BaseCtx, _Ctx> *m_pWindowCtx;
  RenderPass<_Ctx> m_RenderPass;
  std::unique_ptr<Framebuffer<_Ctx>[]> m_Framebuffers;

  void create(WindowContext<BaseCtx, _Ctx> &window_ctx) {
    m_pWindowCtx = &window_ctx;
    // 对渲染使用的图像附件的描述
    VkAttachmentDescription attachment_description = {
        .format = window_ctx.m_SwapchainCreateInfo.imageFormat,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR};
    // 渲染使用的渲染阶段
    VkAttachmentReference attachment_reference = {
        0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};
    VkSubpassDescription subpass_description = {
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .colorAttachmentCount = 1,
        .pColorAttachments = &attachment_reference};
    VkSubpassDependency subpass_dependency = {
        .srcSubpass = VK_SUBPASS_EXTERNAL,
        .dstSubpass = 0,
        .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        .dstStageMask =
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, // 不早于提交命令缓冲区时等待semaphore对应的waitDstStageMask
        .srcAccessMask = 0,
        .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        .dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT};
    VkRenderPassCreateInfo render_pass_ci = {
        .attachmentCount = 1,
        .pAttachments = &attachment_description,
        .subpassCount = 1,
        .pSubpasses = &subpass_description,
        .dependencyCount = 1,
        .pDependencies = &subpass_dependency};
    m_RenderPass.create(render_pass_ci);

    auto CreateFramebuffers =
        [this](WindowContext<WindowContextBase_glfw, _Ctx> *_) {
          auto &window_ctx = *m_pWindowCtx;
          VkExtent2D &windowSize = window_ctx.m_SwapchainCreateInfo.imageExtent;
          m_Framebuffers = std::make_unique<Framebuffer<_Ctx>[]>(
              window_ctx.m_SwapchainImages.size());
          VkFramebufferCreateInfo framebufferCreateInfo = {
              .renderPass = m_RenderPass,
              .attachmentCount = 1,
              .width = windowSize.width,
              .height = windowSize.height,
              .layers = 1};
          for (size_t i = 0; i < window_ctx.m_SwapchainImages.size(); i++) {
            VkImageView attachment = window_ctx.m_SwapchainImageViews[i];
            framebufferCreateInfo.pAttachments = &attachment;
            m_Framebuffers[i].create(framebufferCreateInfo);
          }
        };
    auto DestroyFramebuffers =
        [this](WindowContext<WindowContextBase_glfw, _Ctx> *_) {
          m_Framebuffers.release();
        };

    window_ctx.m_CallbackSwapchainConstruct.insert(CreateFramebuffers);
    window_ctx.m_CallbackSwapchainDestroy.insert(DestroyFramebuffers);
    CreateFramebuffers(m_pWindowCtx);
  }
  void cleanup() noexcept {
    m_pWindowCtx = nullptr;
    m_RenderPass.destroy();
    m_Framebuffers.release();
  }
};
