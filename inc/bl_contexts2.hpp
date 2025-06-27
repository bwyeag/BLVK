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
#ifndef _BL_CORE_CONTEXTS_HPP_
#define _BL_CORE_CONTEXTS_HPP_
#include <bl_contexts.hpp>
#include <bl_output.hpp>
#include "vulkan/vulkan_core.h"
#include <cstdint>
#include <functional>
#include <list>
#include <mutex>
#include <psdk_inc/_ip_types.h>
#include <vma/vk_mem_alloc.h>
#include <vulkan/vk_enum_string_helper.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
namespace BLT {


#ifdef DEBUG
constexpr int8_t is_debuging = 1;
#else
constexpr int8_t is_debuging = 0;
#endif // DEBUG
enum class CtxResult { Success = 0, Failed = -1 ,ArgumentError,GetVideoModeFailed,
WindowCreateFailed};

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
    m_Items.push_back(std::forward(fn));
    return {m_Items.end()};
  }
  void iterate(Args &&...call) {
    for (Func &fn : m_Items) {
      fn(std::forward(call)...);
    }
  }
  void erase(Handle &handle) { m_Items.erase(handle.m_it); }
  void clear() { m_Items.clear(); }
};
template <typename Tag, typename... Args>
using Callback = Callback2<Tag, 0u, Args...>;
//*****************************************************************************
// WindowContextBase_*** 类
//*****************************************************************************

/// @brief 窗口创建的设置，可通过位运算组合
struct _WindowCreateState_type {
  enum Type : uint16_t {
    init_unvisiable = 0x1, // 初始不可见
    full_screen = 0x2,     // 窗口全屏
    maximized = 0x4,       // 窗口最大化
    specified = 0x6,       // 窗口自定义大小
    size_mask = 0x6,       // 用于提取窗口大小相关信息
    decorated = 0x8,       // 窗口是否有边框
    mouse_pass_through =
        0x10,         // 鼠标操作是否会到下一层窗口（仅支持未修饰的窗口）
    resizable = 0x20, // 窗口是否可调节大小
    init_mouse_centered = 0x40, // 初始鼠标是否在窗口中心
    use_primary_monitor = 0x80, // 是否使用主显示器
    auto_iconify =
        0x100, // 全屏窗口是否在输入焦点丢失时自动图标化并恢复以前的视频模式
    window_floating = 0x200 // 窗口是否浮动在其他常规窗口上
  };
};
using WindowCreateState = _WindowCreateState_type::Type;
/// @brief 窗口创建信息
struct WindowCreateInfo_glfw {
  using State = WindowCreateState;
  State m_InitState{State(State::specified | State::decorated |
                          State::resizable | State::use_primary_monitor)};
  uint32_t m_InitSizeX, m_InitSizeY;
  uint32_t m_InitPosX{~0u}, m_InitPosY{~0u};
  uint32_t m_MaxSizeX = GLFW_DONT_CARE, m_MaxSizeY = GLFW_DONT_CARE,
           m_MinSizeX = GLFW_DONT_CARE, m_MinSizeY = GLFW_DONT_CARE;
  const char *m_InitTitle;
  std::function<bool(GLFWmonitor *)> m_MonitorPred;
};
struct WindowContextBase_glfw {
  static std::once_flag s_InitOnce;
  static constexpr const char *s_TypeName = "WindowContextBase_glfw";
  GLFWwindow *m_pWindow{nullptr};
  GLFWmonitor *m_pMonitor{nullptr};
  std::string m_Title;

  static CtxResult init_glfw();
  static void cleanup_glfw() noexcept;
  CtxResult create(const WindowCreateInfo_glfw &info);
  void cleanup() noexcept;
};
//*****************************************************************************
// WindowContext 模板类
//*****************************************************************************
struct ContextBase;

struct SwapchainCreateInfo {
  bool m_isFrameRateLimited;
  VkSwapchainCreateFlagsKHR m_flags;
};
template <class BaseCtx> struct WindowContext : BaseCtx {
  static constexpr const char *s_TypeName = "WindowContext";
  VkSurfaceKHR m_Surface{VK_NULL_HANDLE};

  VkSwapchainKHR m_Swapchain{VK_NULL_HANDLE};
  std::vector<VkImage> m_SwapchainImages;
  std::vector<VkImageView> m_SwapchainImageViews;
  VkSwapchainCreateInfoKHR m_SwapchainCreateInfo{};

  std::vector<VkSurfaceFormatKHR> m_AvailableFormats;

  Callback<WindowContext, WindowContext *> m_CallbackSwapchainDestroy;
  Callback<WindowContext, WindowContext *> m_CallbackSwapchainConstruct;

  CtxResult create(const SwapchainCreateInfo& info, ContextBase& ctx);
  void cleanup() noexcept;

protected:
  /// @brief 创建窗口表面, 应当委托到BaseCtx执行
  /// @param ctx 使用的Vulkan上下文
  /// @return 是否成功执行
  VkResult prepare_surface(ContextBase &ctx);
  /// @brief 重建交换链
  /// @param ctx 使用的Vulkan上下文
  /// @return 是否成功执行
  VkResult recreate_swapchain(ContextBase &ctx);
  /// @brief 直接创建交换链，并且获取交换链图像和视图，不调用回调
  /// @param ctx 使用的Vulkan上下文
  /// @return 是否成功执行
  VkResult create_swapchain_Internal(ContextBase &ctx);
  /// @brief 获取窗口表面格式，在create()中调用
  /// @param ctx 使用的Vulkan上下文
  /// @return 是否成功执行
  VkResult acquire_surface_formats(ContextBase &ctx);
  /// @brief 获取窗口呈现模式，在create()中调用
  /// @param presentModes
  /// @param ctx 使用的Vulkan上下文
  /// @return 是否成功执行
  VkResult acquire_present_modes(std::vector<VkPresentModeKHR> &presentModes,
                                 ContextBase &ctx);
  /// @brief 设定当前窗口表面格式
  /// @param surfaceFormat
  /// @param ctx 使用的Vulkan上下文
  /// @return 是否成功执行
  VkResult set_surface_format(VkSurfaceFormatKHR surfaceFormat,
                              ContextBase &ctx);
};

//*****************************************************************************
// Context 类
//*****************************************************************************
struct ContextBase {
  double m_CurrentTime{0.0}, m_DeltaTime{0.0};

  uint32_t m_VulkanApiVersion;
  VkInstance m_Instance{VK_NULL_HANDLE};
  VkPhysicalDevice m_PhysicalDevice{VK_NULL_HANDLE};
  VkDevice m_Device{VK_NULL_HANDLE};

  uint32_t m_QueueFamilyIndex_graphics{VK_QUEUE_FAMILY_IGNORED};
  uint32_t m_QueueFamilyIndex_compute{VK_QUEUE_FAMILY_IGNORED};
  uint32_t m_QueueFamilyIndex_presentation{VK_QUEUE_FAMILY_IGNORED};

  VkQueue m_Queue_graphics{VK_NULL_HANDLE};
  VkQueue m_Queue_compute{VK_NULL_HANDLE};
  VkQueue m_Queue_presentation{VK_NULL_HANDLE};

  VkPhysicalDeviceProperties2 m_PhysicalDeviceProperties;
  VkPhysicalDeviceVulkan11Properties m_PhysicalDeviceVulkan11Properties;
  VkPhysicalDeviceVulkan12Properties m_PhysicalDeviceVulkan12Properties;
  VkPhysicalDeviceVulkan13Properties m_PhysicalDeviceVulkan13Properties;
  VkPhysicalDeviceVulkan14Properties m_PhysicalDeviceVulkan14Properties;

  VkPhysicalDeviceMemoryProperties2 m_PhysicalDeviceMemoryProperties;

  VkPhysicalDeviceFeatures2 m_PhysicalDeviceFeatures;
  VkPhysicalDeviceVulkan11Features m_PhysicalDeviceVulkan11Features;
  VkPhysicalDeviceVulkan12Features m_PhysicalDeviceVulkan12Features;
  VkPhysicalDeviceVulkan13Features m_PhysicalDeviceVulkan13Features;
  VkPhysicalDeviceVulkan14Features m_PhysicalDeviceVulkan14Features;

  /// @brief 当前设备可用的扩展
  std::vector<VkExtensionProperties> m_AvailableExtensions;
  /// @brief 指向availableExtensions的扩展名称, 已按照字典序排列
  std::vector<const char *> m_Extensions;
#ifdef DEBUG
  VkDebugUtilsMessengerEXT m_Debugger{VK_NULL_HANDLE};
#endif // DEBUG
  VmaAllocator m_Allocator;
  
  CtxResult m_ErrorState{CtxResult::Success};
   
};
} // namespace BLT
#endif // !_BL_CORE_CONTEXTS_HPP_
