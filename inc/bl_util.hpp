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
#ifndef _BL_CORE_BL_UTIL_HPP_
#define _BL_CORE_BL_UTIL_HPP_
// 本地include
#include <bl_output.hpp>
// 第三方库include
#include <cstdint>
#include <vulkan/vulkan.h>
// 标准库include
#include <chrono>
#include <functional>
#include <list>
#include <print>
namespace BLT {
#define BL_VERSION VK_MAKE_API_VERSION(0, 0, 1, 0)
#ifdef DEBUG
constexpr int8_t is_debuging = 1;
#else
constexpr int8_t is_debuging = 0;
#endif // DEBUG
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
enum class LoadResult:int32_t {
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
//*****************************************************************************
// 回调函数类
//*****************************************************************************
namespace _detail {
template <typename Tag> static constexpr bool has_callback_set = false;
template <typename Tag, size_t Series> void callback_set() {}
} // namespace _detail
template <typename Tag, size_t Series, typename... Args> class Callback2 {
public:
  using Func = std::function<void(Args...)>;
  using List = typename std::list<Func>;
  using Iterator = typename List::iterator;

private:
  List m_Items;

public:
  struct Handle {
    Iterator m_it;
  };
  size_t size() const { return m_Items.size(); }
  Handle insert(Func &&fn) {
    if constexpr (_detail::has_callback_set<Tag>)
      _detail::callback_set<Tag, Series>();
    m_Items.push_back(std::forward<Func>(fn));
    return {m_Items.end()};
  }
  void iterate(Args &&...call) {
    for (Func &fn : m_Items) {
      fn(std::forward<Args>(call)...);
    }
  }
  void erase(Handle &handle) { m_Items.erase(handle.m_it); }
  void clear() { m_Items.clear(); }
};
template <typename Tag, typename... Args>
using Callback = Callback2<Tag, 0u, Args...>;
//*****************************************************************************
// 计时类

/// @brief 标准计时器
class Timer {
  using Clock = std::chrono::high_resolution_clock;
  using TimePoint = Clock::time_point;
  using TimeDuration = Clock::duration;
  TimePoint m_TimeBegin, m_TimeEnd;

public:
  void begin() { m_TimeBegin = Clock::now(); }
  void end() { m_TimeEnd = Clock::now(); }
  double duration() const {
    auto Cast = [](TimeDuration dt) {
      return std::chrono::duration_cast<std::chrono::nanoseconds>(dt);
    };
    return Cast(m_TimeEnd - m_TimeBegin).count() * 1e-6;
  }
};
/// @brief 作用域计时器
class ScopedTimer : protected Timer {
  double &m_target;

public:
  ScopedTimer(double &tar) : m_target(tar) { this->begin(); }
  ~ScopedTimer() {
    this->end();
    m_target = this->duration();
  }
};
/// @brief 自动的作用域计时器，在开始和结束时自动输出计时结果
class AutoScopedTimer : protected Timer {
  const char *m_Name;

public:
  AutoScopedTimer(const char *name) : m_Name(name) {
    std::println("Timer Begin [{}];", m_Name);
    this->begin();
  }
  ~AutoScopedTimer() {
    this->end();
    std::println("Timer End [{}] {}ms;", m_Name, this->duration());
  }
};

//*****************************************************************************
// 字节序反转函数
constexpr uint16_t byte_reverse(uint16_t x) {
  x = (x & 0x00fful) << 8 | (x & 0xff00ul) >> 8;
  return x;
}
constexpr uint32_t byte_reverse(uint32_t x) {
  x = (x & 0x0000fffful) << 16 | (x & 0xffff0000ul) >> 16;
  x = (x & 0x00ff00fful) << 8 | (x & 0xff00ff00ul) >> 8;
  return x;
}
constexpr uint64_t byte_reverse(uint64_t x) {
  x = (x & 0x00000000ffffffffull) << 32 | (x & 0xffffffff00000000ull) >> 32;
  x = (x & 0x0000ffff0000ffffull) << 16 | (x & 0xffff0000ffff0000ull) >> 16;
  x = (x & 0x00ff00ff00ff00ffull) << 8 | (x & 0xff00ff00ff00ff00ull) >> 8;
  return x;
}
} // namespace BLT
#endif //!_BL_CORE_BL_UTIL_HPP_
