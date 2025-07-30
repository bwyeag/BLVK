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
#ifndef BL_UTIL_RESULT_TYPE_FILE
// 第三方库include
#include <vulkan/vulkan.h>
// 标准库include
#include <cstdint>
namespace blt {
enum class CtxResult : int32_t {
  Success = 0,
  WrongArgument = -1,

  WindowCreateFailed = -1024,
  DeviceCreateFailed = -1025,
  InstanceCreateFailed = -1026,
  DebugCreateFailed = -1027,
  VmaCreateFailed = -1028,
  GLFWInitFailed = -1029,

  FuncGetVideoModeFailed = -2048,
  FuncGetPhysicalDeviceSurfaceCapFailed = -2049,
  FuncCreateSwapchainInternalFailed = -2050,

  AcquirePresentModesFailed = -4096,
  AcquireDeviceExtensionsFailed = -4097,
  AcquirePhysicalDevicesFailed = -4098,
  AcquireApiVersionFailed = -4099,
  AcquireGlfwExtFailed = -4100,
  AcquireSurfaceFormatsFailed = -4101,

  NoFitDevice = -8192,
  VulkanAPIVersionTooLow = -8193,

  CheckExtFailed = -16384,
  CheckLayerFailed = -16385
};
enum class RenderResult : int32_t {
  Success = 0,
  NullPointer = -1,
  SwapImageFailed,
  QueueSubmitFailed,
  PresentImageFailed
};
enum class LoadResult : int32_t {
  Success = 0,
  FileNotFound = -1,
  FileStructError = -2
};
struct result_t {
  uint32_t m_category : 32;
  int32_t m_type : 32;

  result_t(VkResult r) : m_category(1u), m_type(static_cast<int32_t>(r)) {}
  result_t(CtxResult r) : m_category(2u), m_type(static_cast<int32_t>(r)) {}
  result_t(RenderResult r) : m_category(3u), m_type(static_cast<int32_t>(r)) {}
  ~result_t() noexcept(false);
};
} // namespace BLT
#endif // !BL_UTIL_RESULT_TYPE_FILE
