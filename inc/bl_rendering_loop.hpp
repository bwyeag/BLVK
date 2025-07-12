/******************************************************************************
MIT License

Copyright (c) 2025 bwyeag

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
******************************************************************************/
#ifndef _BL_RENDERING_LOOP_FLLE_
#define _BL_RENDERING_LOOP_FLLE_
// 本地include
#include <bl_util.hpp>
#include <bl_contexts.hpp>
#include <bl_vktypes.hpp>
// 标准库include
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <winnt.h>
namespace BLT {

//*****************************************************************************
// RenderingLoop 类
//*****************************************************************************

template <typename _Ctx = ContextTraits> struct RenderingLoop {
  using fence_t = Fence<_Ctx>;
  using semaphore_t = Semaphore<_Ctx>;
  using cmd_pool_t = CommandPool<_Ctx>;
  using cmd_buf_t = cmd_pool_t::CmdBuffer;
  struct objects_per_frame {
    objects_per_frame *m_Next;
    fence_t m_Fence;
    semaphore_t *m_Semaphores; // 信号量数组指针，有stages_count +1 个
    cmd_buf_t *m_CmdBufs_g;    // 命令缓冲区数组指针， 有stages_count个
    cmd_buf_t *m_CmdBufs_p;
  };

  uint32_t m_StagesCount; // 每帧有几个阶段
  uint32_t m_ResCount;    // 一共有几个图像资源
  uint32_t m_CurrentStages;
  std::unique_ptr<objects_per_frame[]> m_Objects;
  std::unique_ptr<semaphore_t[]> m_Semaphores;
  cmd_pool_t m_CmdPool_graphics;
  cmd_pool_t m_CmdPool_presentation;
  std::unique_ptr<cmd_buf_t[]> m_CmdBufs;
  objects_per_frame *m_CurrentObject;

  RenderResult create(uint32_t stages_count, uint32_t resource_count);
  void cleanup() noexcept;
  objects_per_frame *get_current_objects() { return m_CurrentObject; }
  void next_frame() noexcept { m_CurrentObject = m_CurrentObject->m_Next; }
};
//*****************************************************************************
// WindowRenderCtx 类
//*****************************************************************************

struct __WindowRenderCtxFlagBits_t {
  enum Flags { none = 0, ForceInfight = 0x1, ForceG2PMemBarrier = 0x2 };
};
using WindowRenderCtxFlagBits = __WindowRenderCtxFlagBits_t::Flags;
struct WindowRenderCtxCreateInfo {
  void *m_pWindowCtx;
  uint32_t m_StagesNum;
  WindowRenderCtxFlagBits m_Flags;
};
template <typename BaseCtx, typename _Ctx = ContextTraits>
struct WindowRenderCtx {
  static constexpr const char *s_TypeName = "WindowRenderCtx";
  WindowContext<BaseCtx> *m_pWindowCtx{nullptr};
  RenderingLoop<_Ctx> m_RenderLoop;
  uint32_t m_CurrentImageIndex{0};
  WindowRenderCtxFlagBits m_Flags;

  using CreateInfo = WindowRenderCtxCreateInfo;
  using FrameInfo = decltype(m_RenderLoop)::objects_per_frame;
  using cmd_buf_t = decltype(m_RenderLoop)::cmd_buf_t;

  RenderResult create(const CreateInfo &info);
  void cleanup() noexcept;
  template <typename Callable>
  RenderResult begin(VkPipelineStageFlags wait_flag, Callable func,
                     bool final_stage = false);
  template <typename Callable>
  RenderResult next(VkPipelineStageFlags wait_flag, Callable func,
                    bool final_stage = false);
  RenderResult end_and_present(VkPipelineStageFlags wait_flag);

protected:
  VkResult present_image(VkPresentInfoKHR &presentInfo);
  VkResult present_image(VkSemaphore semaphore_rendering_over);
  VkResult swap_image(VkSemaphore semaphore_to_set, VkFence fence_to_set,
                      uint64_t timeout = UINT64_MAX);
  void g2p_membarrier_release(VkCommandBuffer buf);
  void g2p_membarrier_acquire(VkCommandBuffer buf);
};

//*****************************************************************************
// RenderSection 相关
//*****************************************************************************

} // namespace BLT
namespace BLT {
//*****************************************************************************
// RenderingLoop 类实现
//*****************************************************************************

template <typename _Ctx>
RenderResult
RenderingLoop<_Ctx>::create(uint32_t stages_count, uint32_t resource_count) {
  m_StagesCount = stages_count, m_ResCount = resource_count;
  m_Objects = std::make_unique<objects_per_frame[]>(resource_count);
  size_t semaphores_size = (stages_count + 1) * resource_count;
  m_Semaphores = std::make_unique<semaphore_t[]>(semaphores_size);
  size_t cmdbufs_size = stages_count * resource_count;
  m_CmdBufs = std::make_unique<cmd_buf_t[]>(cmdbufs_size + resource_count);

  auto semaphore_ci = make_semaphore_createinfo();
  for (uint32_t i = 0; i < semaphores_size; ++i)
    m_Semaphores[i].create(semaphore_ci);
  m_CmdPool_graphics.create(_Ctx::get_queueFamilyIndex_graphics(),
                            VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
  m_CmdPool_graphics.allocate_buffers(m_CmdBufs.get(), cmdbufs_size);
  m_CmdPool_presentation.create(
      _Ctx::get_queueFamilyIndex_presentation(),
      VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
  m_CmdPool_presentation.allocate_buffers(m_CmdBufs.get() + cmdbufs_size,
                                          resource_count);

  auto fence_ci = make_fence_createinfo(VK_FENCE_CREATE_SIGNALED_BIT);
  for (uint32_t i = 0; i < resource_count; ++i) {
    auto &obj = m_Objects[i];
    obj.m_Next = m_Objects.get() + (i + 1);
    obj.m_Fence.create(fence_ci);
    obj.m_Semaphores = m_Semaphores.get() + i * (stages_count + 1);
    obj.m_CmdBufs_g = m_CmdBufs.get() + i * stages_count;
    obj.m_CmdBufs_p = m_CmdBufs.get() + cmdbufs_size + i;
  }
  m_Objects[resource_count - 1].m_Next = m_CurrentObject = m_Objects.get();
  return RenderResult::Success;
}
template <typename _Ctx> void RenderingLoop<_Ctx>::cleanup() noexcept {
  m_Objects.release(), m_Semaphores.release(), m_CmdPool_graphics.~cmd_pool_t(),
      m_CmdPool_presentation.~cmd_pool_t(), m_CmdBufs.release(),
      m_CurrentObject = nullptr;
}
//*****************************************************************************
// WindowRenderCtx 类实现
//*****************************************************************************

template <typename BaseCtx, typename _Ctx>
RenderResult WindowRenderCtx<BaseCtx, _Ctx>::create(
    const WindowRenderCtx<BaseCtx, _Ctx>::CreateInfo &info) {
  if (!info.m_pWindowCtx)
    return RenderResult::NullPointer;
  bool g2p_membarrier_flag =
      (m_Flags & WindowRenderCtxFlagBits::ForceG2PMemBarrier) ||
      _Ctx::get_queueFamilyIndex_graphics() !=
          _Ctx::get_queueFamilyIndex_presentation();
  m_Flags = WindowRenderCtxFlagBits(
      info.m_Flags |
      (g2p_membarrier_flag ? WindowRenderCtxFlagBits::ForceG2PMemBarrier
                           : WindowRenderCtxFlagBits::none));
  m_pWindowCtx = reinterpret_cast<decltype(m_pWindowCtx)>(info.m_pWindowCtx);
  m_RenderLoop.create(info.m_StagesNum + g2p_membarrier_flag,
                      (m_Flags & WindowRenderCtxFlagBits::ForceInfight)
                          ? m_pWindowCtx->m_SwapchainImages.size()
                          : 1);
  return RenderResult::Success;
}
template <typename BaseCtx, typename _Ctx>
void WindowRenderCtx<BaseCtx, _Ctx>::cleanup() noexcept {
  m_pWindowCtx = nullptr;
  m_RenderLoop.cleanup();
}
template <typename BaseCtx, typename _Ctx>
template <typename Callable>
RenderResult
WindowRenderCtx<BaseCtx, _Ctx>::begin(VkPipelineStageFlags wait_flag,
                                      Callable func, bool final_stage) {
  static_assert(
      requires(Callable fn, VkCommandBuffer buf, uint32_t i) {
        { fn(buf, i) } -> std::same_as<void>;
      }, "'fn' must be callable as void(cmd_buf_t, i)");
  m_RenderLoop.m_CurrentStages = 0;
  FrameInfo &objects = *m_RenderLoop.get_current_objects();
  objects.m_Fence.wait_and_reset();
  auto &semaphore_to_signal =
      objects.m_Semaphores[m_RenderLoop.m_CurrentStages];
  if (swap_image(semaphore_to_signal, VK_NULL_HANDLE))
    return RenderResult::SwapImageFailed;
  next(wait_flag, func, final_stage);
  return RenderResult::Success;
}
template <typename BaseCtx, typename _Ctx>
template <typename Callable>
RenderResult
WindowRenderCtx<BaseCtx, _Ctx>::next(VkPipelineStageFlags wait_flag,
                                     Callable func, bool final_stage) {
  static_assert(
      requires(Callable fn, VkCommandBuffer buf, uint32_t i) {
        { fn(buf, i) } -> std::same_as<void>;
      }, "'fn' must be callable as void(cmd_buf_t, i)");
  FrameInfo &object = *m_RenderLoop.get_current_objects();
  auto &semaphore_to_wait = object.m_Semaphores[m_RenderLoop.m_CurrentStages];
  auto &semaphore_to_signal =
      object.m_Semaphores[m_RenderLoop.m_CurrentStages + 1];
  auto &buf = object.m_CmdBufs_g[m_RenderLoop.m_CurrentStages];
  buf.begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
  func(buf, m_CurrentImageIndex);
  if (final_stage && (_Ctx::get_queueFamilyIndex_graphics() !=
                          _Ctx::get_queueFamilyIndex_presentation() ||
                      (m_Flags & WindowRenderCtxFlagBits::ForceG2PMemBarrier)))
    g2p_membarrier_release(buf);
  buf.end();
  VkSubmitInfo submit{
      .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
      .waitSemaphoreCount = 1,
      .pWaitSemaphores = semaphore_to_wait.get_pointer(),
      .pWaitDstStageMask = &wait_flag,
      .commandBufferCount = 1,
      .pCommandBuffers = buf.get_pointer(),
      .signalSemaphoreCount = 1,
      .pSignalSemaphores = semaphore_to_signal.get_pointer(),
  };
  if (VkResult result =
          vkQueueSubmit(_Ctx::get_queue_graphics(), 1, &submit,
                        final_stage ? object.m_Fence : VK_NULL_HANDLE)) {
    print_error(s_TypeName, "vkQueueSubmit() failed:", string_VkResult(result));
    return RenderResult::QueueSubmitFailed;
  }
  ++m_RenderLoop.m_CurrentStages;
  return RenderResult::Success;
}
template <typename BaseCtx, typename _Ctx>
RenderResult WindowRenderCtx<BaseCtx, _Ctx>::end_and_present(
    VkPipelineStageFlags wait_flag) {
  int offset = 0;
  if (_Ctx::get_queueFamilyIndex_graphics() !=
          _Ctx::get_queueFamilyIndex_presentation() ||
      (m_Flags & WindowRenderCtxFlagBits::ForceG2PMemBarrier)) {
    FrameInfo &object = *m_RenderLoop.get_current_objects();
    cmd_buf_t &buf = *(object.m_CmdBufs_p);
    buf.begin();
    g2p_membarrier_acquire(buf);
    buf.end();
    VkSubmitInfo submit{
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores =
            object.m_Semaphores->get_pointer() + m_RenderLoop.m_CurrentStages,
        .commandBufferCount = 1,
        .pCommandBuffers = buf.get_pointer(),
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = object.m_Semaphores->get_pointer() +
                             m_RenderLoop.m_CurrentStages + 1,
    };
    if (VkResult result = vkQueueSubmit(_Ctx::get_queue_graphics(), 1, &submit,
                                        VK_NULL_HANDLE)) {
      print_error(s_TypeName,
                  "vkQueueSubmit() failed:", string_VkResult(result));
      return RenderResult::QueueSubmitFailed;
    }
    offset = 1;
  }
  FrameInfo &object = *m_RenderLoop.get_current_objects();
  if (present_image(
          object.m_Semaphores[m_RenderLoop.m_CurrentStages + offset])) {
    print_error(s_TypeName, "present_image() failed!");
    return RenderResult::PresentImageFailed;
  }
  return RenderResult::Success;
}

template <typename BaseCtx, typename _Ctx>
VkResult
WindowRenderCtx<BaseCtx, _Ctx>::present_image(VkPresentInfoKHR &presentInfo) {
  switch (VkResult result =
              vkQueuePresentKHR(_Ctx::get_queue_presentation(), &presentInfo)) {
  case VK_SUCCESS:
    return VK_SUCCESS;
  case VK_SUBOPTIMAL_KHR:
  case VK_ERROR_OUT_OF_DATE_KHR:
    return m_pWindowCtx->recreate_swapchain();
  default:
    print_error(s_TypeName,
                "vkQueuePresentKHR() failed:", string_VkResult(result));
    return result;
  }
}
template <typename BaseCtx, typename _Ctx>
VkResult WindowRenderCtx<BaseCtx, _Ctx>::present_image(
    VkSemaphore semaphore_rendering_over) {
  VkPresentInfoKHR presentInfo = {.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
                                  .swapchainCount = 1,
                                  .pSwapchains = &m_pWindowCtx->m_Swapchain,
                                  .pImageIndices = &m_CurrentImageIndex};
  if (semaphore_rendering_over)
    presentInfo.waitSemaphoreCount = 1,
    presentInfo.pWaitSemaphores = &semaphore_rendering_over;
  return present_image(presentInfo);
}
template <typename BaseCtx, typename _Ctx>
VkResult WindowRenderCtx<BaseCtx, _Ctx>::swap_image(
    VkSemaphore semaphore_to_set, VkFence fence_to_set, uint64_t timeout) {
  VkSwapchainCreateInfoKHR &cInfo = m_pWindowCtx->m_SwapchainCreateInfo;
  if (cInfo.oldSwapchain && cInfo.oldSwapchain != m_pWindowCtx->m_Swapchain) {
    vkDestroySwapchainKHR(_Ctx::get_device(), cInfo.oldSwapchain, nullptr);
    cInfo.oldSwapchain = VK_NULL_HANDLE;
  }
  while (VkResult result = vkAcquireNextImageKHR(
             _Ctx::get_device(), m_pWindowCtx->m_Swapchain, timeout,
             semaphore_to_set, fence_to_set, &m_CurrentImageIndex))
    switch (result) {
    case VK_SUBOPTIMAL_KHR:
    case VK_ERROR_OUT_OF_DATE_KHR:
      if (VkResult result = m_pWindowCtx->recreate_swapchain())
        return result;
      break;
    case VK_TIMEOUT:
    case VK_NOT_READY:
      // 如果timeout = ~0u 则不会发生
      print_log(s_TypeName, "Acquire next image with", string_VkResult(result));
      return result;
    default:
      print_error(s_TypeName, "Failed to acquire next image! Code:",
                  string_VkResult(result));
      return result;
    }
  return VK_SUCCESS;
}
template <typename BaseCtx, typename _Ctx>
void WindowRenderCtx<BaseCtx, _Ctx>::g2p_membarrier_release(
    VkCommandBuffer buf) {
  VkImageMemoryBarrier image_memory_barrier_g2p_release = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
      .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
      .dstAccessMask =
          0, // 因为vkCmdPipelineBarrier(...)的参数中dstStage是BOTTOM_OF_PIPE，不需要dstAccessMask
      .oldLayout =
          VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, // 内存布局已经在渲染通道结束时转换（这是下一节的内容），此处oldLayout和newLayout相同，不发生转变
      .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
      .srcQueueFamilyIndex = _Ctx::get_queueFamilyIndex_graphics(),
      .dstQueueFamilyIndex = _Ctx::get_queueFamilyIndex_presentation(),
      .image = m_pWindowCtx->m_SwapchainImages[m_CurrentImageIndex],
      .subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}};
  vkCmdPipelineBarrier((VkCommandBuffer)buf,
                       VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                       VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0,
                       nullptr, 1, &image_memory_barrier_g2p_release);
}
template <typename BaseCtx, typename _Ctx>
void WindowRenderCtx<BaseCtx, _Ctx>::g2p_membarrier_acquire(
    VkCommandBuffer buf) {
  VkImageMemoryBarrier imageMemoryBarrier_g2p_acquire = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
      .oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
      .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
      .srcQueueFamilyIndex = _Ctx::get_queueFamilyIndex_graphics(),
      .dstQueueFamilyIndex = _Ctx::get_queueFamilyIndex_presentation(),
      .image = m_pWindowCtx->m_SwapchainImages[m_CurrentImageIndex],
      .subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}};
  vkCmdPipelineBarrier((VkCommandBuffer)buf, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                       VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0,
                       nullptr, 1, &imageMemoryBarrier_g2p_acquire);
}
} // namespace BLT
#endif // !_BL_RENDERING_LOOP_FLLE_
