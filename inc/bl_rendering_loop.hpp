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
#include "bl_output.hpp"
#include <bl_contexts2.hpp>
#include <bl_vktypes.hpp>
// 标准库include
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <memory>
namespace BLT {
enum class RenderResult {
  Success = 0,
  NullPointer = -1,
  SwapImageFailed,
  QueueSubmitFailed,
  PresentImageFailed
};
template <typename _Ctx = ContextTraits> struct RenderingLoop {
  using fence_t = Fence<_Ctx>;
  using semaphore_t = Semaphore<_Ctx>;
  using cmd_pool_t = CommandPool<_Ctx>;
  using cmd_buf_t = cmd_pool_t::CmdBuffer;
  using Result = RenderResult;
  struct objects_per_frame {
    objects_per_frame *m_Next;
    fence_t m_Fence;
    semaphore_t *m_Semaphores; // 信号量数组指针，有stages_count +1 个
    cmd_buf_t *m_CmdBufs;      // 命令缓冲区数组指针， 有stages_count个
  };

  uint32_t m_StagesCount; // 每帧有几个阶段
  uint32_t m_ResCount;    // 一共有几个图像资源
  uint32_t m_CurrentStages;
  std::unique_ptr<objects_per_frame[]> m_Objects;
  std::unique_ptr<semaphore_t[]> m_Semaphores;
  cmd_pool_t m_CmdPool;
  std::unique_ptr<cmd_buf_t[]> m_CmdBufs;
  objects_per_frame *m_CurrentObject;

  Result create(uint32_t stages_count, uint32_t resource_count) {
    m_StagesCount = stages_count, m_ResCount = resource_count;
    m_Objects = std::make_unique<objects_per_frame[]>(resource_count);
    size_t semaphores_size = (stages_count + 1) * resource_count;
    m_Semaphores = std::make_unique<semaphore_t[]>(semaphores_size);
    size_t cmdbufs_size = stages_count * resource_count;
    m_CmdBufs = std::make_unique<cmd_buf_t[]>(cmdbufs_size);

    auto semaphore_ci = make_semaphore_createinfo();
    for (uint32_t i = 0; i < semaphores_size; ++i)
      m_Semaphores[i].create(semaphore_ci);
    m_CmdPool.create(make_commandpool_createinfo(
        _Ctx::get_queueFamilyIndex_graphics(),
        VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT));
    m_CmdPool.allocate_buffers(m_CmdBufs.get(), cmdbufs_size);

    auto fence_ci = make_fence_createinfo(VK_FENCE_CREATE_SIGNALED_BIT);
    for (uint32_t i = 0; i < resource_count; ++i) {
      auto &obj = m_Objects[i];
      obj.m_Next = m_Objects.get() + (i + 1);
      obj.m_Fence.create(fence_ci);
      obj.m_Semaphores = m_Semaphores.data() + i * (stages_count + 1);
      obj.m_CmdBufs = m_CmdBufs.data() + i * stages_count;
    }
    m_Objects[resource_count - 1].m_Next = m_CurrentObject = m_Objects.get();
  }
  void cleanup() noexcept {
    m_Objects.release(), m_Semaphores.release(), m_CmdPool.~cmd_buf_t(),
        m_CmdBufs.release(), m_CurrentObject = nullptr;
  }
  objects_per_frame *get_current_objects() { return m_CurrentObject; }
  void next_frame() noexcept { m_CurrentObject = m_CurrentObject->m_Next; }
};
struct WindowRenderCtxCreateInfo {
  void *m_pWindowCtx;
  uint32_t m_StagesNum;
  bool m_enableInflight;
};
template <typename BaseCtx, typename _Ctx = ContextTraits>
struct WindowRenderCtx {
  static constexpr const char *s_TypeName = "WindowRenderCtx";
  WindowContext<BaseCtx> *m_WindowCtx{nullptr};
  RenderingLoop<_Ctx> m_RenderLoop;
  uint32_t m_CurrentImageIndex;
  using CreateInfo = WindowRenderCtxCreateInfo;
  using FrameInfo = decltype(m_RenderLoop)::objects_per_frame;
  using cmd_buf_t = decltype(m_RenderLoop)::cmd_buf_t;

  RenderResult create(const CreateInfo &info) {
    if (!info.m_pWindowCtx)
      return RenderResult::NullPointer;
    m_WindowCtx = reinterpret_cast<decltype(m_WindowCtx)>(info.m_pWindowCtx);
    m_RenderLoop.create(
        info.m_StagesNum,
        info.m_enableInflight ? m_WindowCtx->m_SwapchainImages.size() : 1);
  }
  void cleanup() noexcept {
    m_WindowCtx = nullptr;
    m_RenderLoop.cleanup();
  }
  template <typename Callable>
  RenderResult begin_frame(VkPipelineStageFlags wait_flag, Callable func,
                           bool final_stage = false) {
    static_assert(
        requires(Callable fn, cmd_buf_t buf) {
          { fn(buf) } -> std::same_as<void>;
        }, "'fn' must be callable");
    m_RenderLoop.m_CurrentStages = 0;
    FrameInfo &objects = *m_RenderLoop.get_current_objects();
    objects.m_Fence.wait_and_reset();
    auto semaphore_to_wait = objects.m_Semaphores[m_RenderLoop.m_CurrentStages];
    if (swap_image(semaphore_to_wait, VK_NULL_HANDLE))
      return RenderResult::SwapImageFailed;
    next_stage(wait_flag, func, final_stage);
    return RenderResult::Success;
  }
  template <typename Callable>
  RenderResult next_stage(VkPipelineStageFlags wait_flag, Callable func,
                          bool final_stage = false) {
    static_assert(
        requires(Callable fn, cmd_buf_t buf) {
          { fn(buf) } -> std::same_as<void>;
        }, "'fn' must be callable");
    FrameInfo &object = *m_RenderLoop.get_current_objects();
    auto semaphore_to_wait = object.m_Semaphores[m_RenderLoop.m_CurrentStages];
    auto semaphore_to_set =
        object.m_Semaphores[m_RenderLoop.m_CurrentStages + 1];
    auto buf = object.m_CmdBufs[m_RenderLoop.m_CurrentStages];
    func(buf);
    VkSubmitInfo submit{
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &semaphore_to_wait,
        .pWaitDstStageMask = &wait_flag,
        .commandBufferCount = 1,
        .pCommandBuffers = &buf,
        .signalSemaphoreCount = 1,
        .signalSemaphoreCount = &semaphore_to_set,
    };
    if (VkResult result =
            vkQueueSubmit(_Ctx::get_queue_graphics(), 1, &submit,
                          final_stage ? object.m_Fence : VK_NULL_HANDLE)) {
      print_error(s_TypeName,
                  "vkQueueSubmit() failed:", string_VkResult(result));
      return RenderResult::QueueSubmitFailed;
    }
    ++m_RenderLoop.m_CurrentStages;
    return RenderResult::Success;
  }
  RenderResult end_frame() {
    FrameInfo &object = *m_RenderLoop.get_current_objects();
    if (present_image(object.m_Semaphores[m_RenderLoop.m_CurrentStages])) {
      print_error(s_TypeName, "present_image() failed!");
      return RenderResult::PresentImageFailed;
    }
    return RenderResult::Success;
  }

protected:
  VkResult present_image(VkPresentInfoKHR &presentInfo) {
    switch (VkResult result = vkQueuePresentKHR(_Ctx::get_queue_presentation(),
                                                &presentInfo)) {
    case VK_SUCCESS:
      return VK_SUCCESS;
    case VK_SUBOPTIMAL_KHR:
    case VK_ERROR_OUT_OF_DATE_KHR:
      return m_WindowCtx->recreate_swapchain();
    default:
      print_error(s_TypeName,
                  "vkQueuePresentKHR() failed:", string_VkResult(result));
      return result;
    }
  }
  VkResult present_image(VkSemaphore semaphore_rendering_over) {
    VkPresentInfoKHR presentInfo = {.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
                                    .swapchainCount = 1,
                                    .pSwapchains = &m_WindowCtx->m_Swapchain,
                                    .pImageIndices = &m_CurrentImageIndex};
    if (semaphore_rendering_over)
      presentInfo.waitSemaphoreCount = 1,
      presentInfo.pWaitSemaphores = &semaphore_rendering_over;
    return present_image(presentInfo);
  }
  VkResult swap_image(Semaphore<_Ctx> semaphore_to_set,
                      Fence<_Ctx> fence_to_set, uint64_t timeout = UINT64_MAX) {
    VkSwapchainCreateInfoKHR &cInfo = m_WindowCtx->m_SwapchainCreateInfo;
    if (cInfo.oldSwapchain && cInfo.oldSwapchain != m_WindowCtx->m_Swapcahin) {
      vkDestroySwapchainKHR(_Ctx::get_device(), cInfo.oldSwapchain, nullptr);
      cInfo.oldSwapchain = VK_NULL_HANDLE;
    }
    while (VkResult result = vkAcquireNextImageKHR(
               _Ctx::get_device(), m_WindowCtx->m_Swapcahin, timeout,
               semaphore_to_set, fence_to_set, &m_CurrentImageIndex))
      switch (result) {
      case VK_SUBOPTIMAL_KHR:
      case VK_ERROR_OUT_OF_DATE_KHR:
        if (VkResult result = m_WindowCtx->recreate_swapchain())
          return result;
        break;
      case VK_TIMEOUT:
      case VK_NOT_READY:
        // 如果timeout = ~0u 则不会发生
        print_log(s_TypeName, "Acquire next image with",
                  string_VkResult(result));
        return result;
      default:
        print_error(s_TypeName, "Failed to acquire next image! Code:",
                    string_VkResult(result));
        return result;
      }
    return VK_SUCCESS;
  }
};
} // namespace BLT
#endif // !_BL_RENDERING_LOOP_FLLE_
