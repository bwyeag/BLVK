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
#include <bit>
#include <new>
#include <stdexcept>
#include <utils/bl_result_type.hpp>

namespace blt {
const std::array<result_category_t, err_category_count> s_CategoryInfo{
    {{"VkResult",
      [](int32_t err) -> const char * {
        return string_VkResult(static_cast<VkResult>(err));
      }},
     {"CtxResult",
      [](int32_t err) -> const char * {
        switch (static_cast<CtxResult>(err)) {
        case CtxResult::Success:
          return "Success";
        case CtxResult::WindowCreateFailed:
          return "WindowCreateFailed";
        case CtxResult::DeviceCreateFailed:
          return "DeviceCreateFailed";
        case CtxResult::InstanceCreateFailed:
          return "InstanceCreateFailed";
        case CtxResult::DebugCreateFailed:
          return "DebugCreateFailed";
        case CtxResult::VmaCreateFailed:
          return "VmaCreateFailed";
        case CtxResult::GLFWInitFailed:
          return "GLFWInitFailed";
        case CtxResult::FuncGetVideoModeFailed:
          return "FuncGetVideoModeFailed";
        case CtxResult::FuncGetPhysicalDeviceSurfaceCapFailed:
          return "FuncGetPhysicalDeviceSurfaceCapFailed";
        case CtxResult::FuncCreateSwapchainInternalFailed:
          return "FuncCreateSwapchainInternalFailed";
        case CtxResult::AcquirePresentModesFailed:
          return "AcquirePresentModesFailed";
        case CtxResult::AcquireDeviceExtensionsFailed:
          return "AcquireDeviceExtensionsFailed";
        case CtxResult::AcquirePhysicalDevicesFailed:
          return "AcquirePhysicalDevicesFailed";
        case CtxResult::AcquireApiVersionFailed:
          return "AcquireApiVersionFailed";
        case CtxResult::AcquireGlfwExtFailed:
          return "AcquireGlfwExtFailed";
        case CtxResult::AcquireSurfaceFormatsFailed:
          return "AcquireSurfaceFormatsFailed";
        case CtxResult::NoFitDevice:
          return "NoFitDevice";
        case CtxResult::VulkanAPIVersionTooLow:
          return "VulkanAPIVersionTooLow";
        case CtxResult::CheckExtFailed:
          return "CheckExtFailed";
        case CtxResult::CheckLayerFailed:
          return "CheckLayerFailed";
        }
      }},
     {"RenderResult",
      [](int32_t err) -> const char * {
        switch (static_cast<RenderResult>(err)) {
        case RenderResult::Success:
          return "Success";
        case RenderResult::NullPointer:
          return "NullPointer";
        case RenderResult::SwapImageFailed:
          return "SwapImageFailed";
        case RenderResult::QueueSubmitFailed:
          return "QueueSubmitFailed";
        case RenderResult::PresentImageFailed:
          return "PresentImageFailed";
        }
      }},
     {"LoadResult", [](int32_t err) -> const char * {
        switch (static_cast<LoadResult>(err)) {
        case LoadResult::Success:
          return "Success";
        case LoadResult::FileNotFound:
          return "FileNotFound";
        case LoadResult::FileStructError:
          return "FileStructError";
        case LoadResult::FileHeadError:
          return "FileHeadError";
        }
      }}}};
thread_local std::array<result_value_t, err_info_capacity> s_ResultValue{};
thread_local result_head_t s_ResultValue_head{};
result_head_t::result_head_t() {
  ptr = s_ResultValue.data();
  for (int i = 0; i < err_info_capacity - 1;)
    s_ResultValue[i].m_NextVal = s_ResultValue.data() + ++i;
  s_ResultValue[err_info_capacity - 1].m_NextVal = nullptr;
}
auto result_t::install_internal(const result_t *next) -> result_value_data_t & {
#ifdef DEBUG
  if (next && next->m_index == result_t::NullIndex)
    throw std::logic_error("uninstalled next result_t");
  if (!s_ResultValue_head.ptr)
    throw std::runtime_error("no more result solt");
#endif // DEBUG
  result_value_t *p = s_ResultValue_head.ptr;
  s_ResultValue_head.ptr = p->m_NextVal;
  m_index = p - s_ResultValue.data();
  p->m_Head = uint64_t(*this);
  p->m_NextVal = next ? s_ResultValue.data() + next->m_index : nullptr;
  return p->m_Data;
}
void result_t::remove() {
#ifdef DEBUG
  if (m_index == result_t::NullIndex)
    throw std::logic_error("uninstalled result_t");
#endif // DEBUG
  result_value_t *p = s_ResultValue.data() + m_index, *q = p;
  while (p->m_NextVal)
    p = p->m_NextVal;
  p->m_NextVal = s_ResultValue_head.ptr;
  s_ResultValue_head.ptr = q;
}
} // namespace blt
