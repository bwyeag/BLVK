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
// 本地include
#include <bl_output.hpp>
// 标准库include
#include <cstdint>
#include <functional>
#include <list>
#include <mutex>
#include <span>
#include <algorithm>
// 外部库include
#include <vma/vk_mem_alloc.h>
#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
namespace BLT {

#ifdef DEBUG
constexpr int8_t is_debuging = 1;
#else
constexpr int8_t is_debuging = 0;
#endif // DEBUG
enum class CtxResult {
  Success = 0,
  Failed = -1,
  ArgumentError = -2,
  GetVideoModeFailed = -3,
  WindowCreateFailed = -4,
  DeviceCreateFailed,
  GetPhysicalDeviceSurfaceCapFailed = -5,
  FuncCreateSwapchainInternalFailed = -6,
  AcquirePresentModesFailed = -7,
  AcquireDeviceExtensionsFailed = -8,
  VmaCreateFailed = -9,
  AcquirePhysicalDevicesFailed,
  NoFitDevice
};
//*****************************************************************************
// 辅助类
struct vkStructureHead {
  VkStructureType sType;
  void *pNext;
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

struct ContextBase;
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

  VkResult make_surface(ContextBase &ctx, VkSurfaceKHR &surface);
  void get_window_size(uint32_t &width, uint32_t &height);
};
//*****************************************************************************
// WindowContext 模板类
//*****************************************************************************

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

  CtxResult create(const SwapchainCreateInfo &info, ContextBase &ctx);
  void cleanup(ContextBase &ctx) noexcept;

protected:
  /// @brief 创建窗口表面, 应当委托到BaseCtx执行
  /// @param ctx 使用的Vulkan上下文
  /// @return 是否成功执行
  VkResult create_surface(ContextBase &ctx);
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

/// @brief 实例阶段创建信息
struct InstanceCreateInfo {
  const char *m_pAppName{nullptr};
  uint32_t m_AppVersion{0u};
  uint32_t m_MinApiVersion{0u};
  bool m_isDebuging{true};
  std::vector<const char *> m_LayerNames{};
  std::vector<const char *> m_ExtensionNames{};
  VkInstanceCreateFlags m_InstanceFlags = 0;
  void *m_pNextInstance{nullptr};
};
/// @brief 设备阶段创建信息
struct DeviceCreateInfo {
  VkDeviceCreateFlags m_DiviceFlags = 0u;
  VmaAllocatorCreateFlags m_VmaFlags = 0u;
  std::vector<const char *> m_ExtensionNames{};
  void *m_pNextDivice{nullptr};
};
struct ContextBase {
  static constexpr const char *s_TypeName = "ContextBase";

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
  VmaAllocator m_Allocator{VK_NULL_HANDLE};

  CtxResult m_ErrorState{CtxResult::Success};

  CtxResult create_instance(const InstanceCreateInfo &info);
  CtxResult create_device(const DeviceCreateInfo &info);

protected:
  /// @brief 获取VulkanAPI的版本
  /// @param version 返回版本
  /// @return 是否正确查询
  VkResult acquire_vkapi_version(uint32_t &version);
  /// @brief 检查实例扩展是否可用
  /// @param extensionNames 被检查的数组
  /// @param layerName 扩展所在的层级, 一律为nullptr
  /// @return 是否正确检查
  VkResult check_instance_extension(std::span<const char *> extensionNames,
                                    const char *layerName = nullptr);
  /// @brief 检查实例层级是否可用
  /// @param layerNames 被检查的数组
  /// @return 是否正确检查
  VkResult check_instance_layer(std::span<const char *> layerNames);
  /// @brief 合并pCallbackData内容为一个字符串输出
  /// @param pCallbackData 被合并的debug数据
  /// @return 表示debug信息的字符串
  static std::string combine_debug_message(
      const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData);
  /// @brief 初始化debug部分
  /// @return 是否正确完成
  VkResult create_debugger();

protected:
  /// @brief 取得物理设备列表
  /// @param availablePhysicalDevices 返回该列表
  /// @return 是否正确完成
  VkResult acquire_physical_devices(
      std::vector<VkPhysicalDevice> &availablePhysicalDevices);
  /// @brief 获取设备所用的队列族
  /// @param physicalDevice 被获取的设备
  /// @param enableGraphicsQueue 是否查找图形队列
  /// @param enableComputeQueue 是否查找计算队列
  /// @param queueFamilyIndices 返回队列族索引, 依次为图形,呈现,计算
  /// @return 是否正确完成
  VkResult acquire_queue_family_indices(VkPhysicalDevice physicalDevice,
                                        uint32_t (&queueFamilyIndices)[3],
                                        std::span<VkSurfaceKHR> windowData,
                                        bool enableGraphicsQueue = true,
                                        bool enableComputeQueue = true);
  /// @brief 决定使用的物理设备, 呈现队列取决于当前是否创建窗口
  /// @param availablePhysicalDevices 可用的物理设备
  /// @param deviceIndex 被决定的设备索引
  /// @param enableGraphicsQueue 是否启用图形队列
  /// @param enableComputeQueue 是否启用计算队列
  /// @return 是否正确完成
  VkResult determine_physical_device(
      std::vector<VkPhysicalDevice> &availablePhysicalDevices,
      uint32_t deviceIndex = 0, std::span<VkSurfaceKHR> windowData = {},
      bool enableGraphicsQueue = true, bool enableComputeQueue = true);
  /// @brief 获取物理设备属性
  void acquire_physical_divice_properties();
  /// @brief 获取物理设备特性
  void acquire_physical_divice_features();
  /// @brief 初始化物理设备
  /// @param windowData 当前需要与设备匹配的各个窗口
  /// @return 是否正确完成
  CtxResult init_physical_device(std::span<VkSurfaceKHR> windowData);
  VkResult
  acquire_device_extensions(std::vector<VkExtensionProperties> &extensionNames,
                            const char *layerName = nullptr);
  /// @brief 获取VMA可使用的所有扩展并加入到extensionNames中
  /// @return 使用的VMA扩展标志
  VmaAllocatorCreateFlagBits
  acquire_vma_extensions(std::vector<const char *> &extensionNames);

  /// @brief 检查设备扩展
  /// @param extensionNames 扩展名称
  /// @param layerName 被检查的层级, 一律为nullptr
  /// @return 是否正确完成
  void check_device_extension(std::span<const char *> extensionNames,
                              const char *layerName = nullptr);
  /// @brief 初始化VMA库(内存分配)
  /// @param info 创建信息
  /// @return 是否正确完成
  VkResult init_vma(const DeviceCreateInfo &info);
};

//*****************************************************************************
// LocalData 部分
//*****************************************************************************

/// @brief 线程本地数据，方便获取上下文
struct ThreadData {
  std::stringstream m_LocalSstream;
};
/// @brief 获取线程本地数据
/// @return 线程本地数据
inline ThreadData &acquire_local_data() {
  static thread_local ThreadData local_data;
  return local_data;
}
/// @brief 默认使用的获取当前上下文的静态类和各种常数设置
class ContextTraits {
  inline static ContextBase *s_CurrentContext{nullptr};
  static inline void set_current_context(ContextBase &ctx) {
    s_CurrentContext = &ctx;
  }

  static inline VkInstance get_instance() {
    return s_CurrentContext->m_Instance;
  }
  static inline VkDevice get_device() { return s_CurrentContext->m_Device; }
  static inline VmaAllocator get_allocator() {
    return s_CurrentContext->m_Allocator;
  }
  static inline const VkPhysicalDeviceMemoryProperties &
  get_phydevice_memory_properties() {
    return s_CurrentContext->m_PhysicalDeviceMemoryProperties.memoryProperties;
  }
  static inline uint32_t get_vulkanApiVersion() {
    return s_CurrentContext->m_VulkanApiVersion;
  }
  static inline VkPhysicalDevice get_phyDevice() {
    return s_CurrentContext->m_PhysicalDevice;
  }
  static inline uint32_t get_queueFamilyIndex_graphics() {
    return s_CurrentContext->m_QueueFamilyIndex_graphics;
  }
  static inline uint32_t get_queueFamilyIndex_compute() {
    return s_CurrentContext->m_QueueFamilyIndex_compute;
  }
  static inline uint32_t get_queueFamilyIndex_presentation() {
    return s_CurrentContext->m_QueueFamilyIndex_presentation;
  }
  static inline VkQueue get_queue_graphics() {
    return s_CurrentContext->m_Queue_graphics;
  }
  static inline VkQueue get_queue_compute() {
    return s_CurrentContext->m_Queue_compute;
  }
  static inline VkQueue get_queue_presentation() {
    return s_CurrentContext->m_Queue_presentation;
  }
  static inline const VkPhysicalDeviceProperties2 &get_phyDeviceProperties() {
    return s_CurrentContext->m_PhysicalDeviceProperties;
  }
  static inline const VkPhysicalDeviceVulkan11Properties &
  get_phyDeviceVulkan11Properties() {
    return s_CurrentContext->m_PhysicalDeviceVulkan11Properties;
  }
  static inline const VkPhysicalDeviceVulkan12Properties &
  get_phyDeviceVulkan12Properties() {
    return s_CurrentContext->m_PhysicalDeviceVulkan12Properties;
  }
  static inline const VkPhysicalDeviceVulkan13Properties &
  get_phyDeviceVulkan13Properties() {
    return s_CurrentContext->m_PhysicalDeviceVulkan13Properties;
  }
  static inline const VkPhysicalDeviceVulkan14Properties &
  get_phyDeviceVulkan14Properties() {
    return s_CurrentContext->m_PhysicalDeviceVulkan14Properties;
  }
  static inline const VkPhysicalDeviceMemoryProperties2 &
  get_phyDeviceMemoryProperties() {
    return s_CurrentContext->m_PhysicalDeviceMemoryProperties;
  }
  static inline const VkPhysicalDeviceFeatures2 &get_phyDeviceFeatures() {
    return s_CurrentContext->m_PhysicalDeviceFeatures;
  }
  static inline const VkPhysicalDeviceVulkan11Features &
  get_phyDeviceVulkan11Features() {
    return s_CurrentContext->m_PhysicalDeviceVulkan11Features;
  }
  static inline const VkPhysicalDeviceVulkan12Features &
  get_phyDeviceVulkan12Features() {
    return s_CurrentContext->m_PhysicalDeviceVulkan12Features;
  }
  static inline const VkPhysicalDeviceVulkan13Features &
  get_phyDeviceVulkan13Features() {
    return s_CurrentContext->m_PhysicalDeviceVulkan13Features;
  }
  static inline const VkPhysicalDeviceVulkan14Features &
  get_phyDeviceVulkan14Features() {
    return s_CurrentContext->m_PhysicalDeviceVulkan14Features;
  }
  static inline VkDebugUtilsMessengerEXT get_debugger() {
    return s_CurrentContext->m_Debugger;
  }
  static inline double get_current_time() {
    return s_CurrentContext->m_CurrentTime;
  }
  static inline double get_delta_time() {
    return s_CurrentContext->m_DeltaTime;
  }
};
} // namespace BLT

// 模板函数实现
namespace BLT {
//*****************************************************************************
// WindowContext 类
//*****************************************************************************
template <class BaseCtx>
CtxResult WindowContext<BaseCtx>::create(const SwapchainCreateInfo &info,
                                         ContextBase &ctx) {
  VkSurfaceCapabilitiesKHR surface_capabilities;
  // 获取surface支持能力
  if (VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
          ctx.m_PhysicalDevice, m_Surface, &surface_capabilities)) {
    print_error(s_TypeName,
                "Failed to get physical device surface capabilities! Code:",
                string_VkResult(result));
    return CtxResult::GetPhysicalDeviceSurfaceCapFailed;
  }
  auto &cInfo = m_SwapchainCreateInfo;
  // 如果容许的最大数量与最小数量不等，那么使用最小数量+1
  cInfo.minImageCount =
      surface_capabilities.minImageCount +
      (surface_capabilities.maxImageCount > surface_capabilities.minImageCount);
  // 决定窗口大小
  uint32_t width, height;
  BaseCtx::get_window_size(width, height);
  // surface_capabilities.currentExtent.width为 ~0u 表示大小未确定
  cInfo.imageExtent =
      surface_capabilities.currentExtent.width == (~0u)
          ? VkExtent2D{std::clamp(width,
                                  surface_capabilities.minImageExtent.width,
                                  surface_capabilities.maxImageExtent.width),
                       std::clamp(height,
                                  surface_capabilities.minImageExtent.height,
                                  surface_capabilities.maxImageExtent.height)}
          : surface_capabilities.currentExtent;
  cInfo.imageArrayLayers = 1;
  cInfo.preTransform = surface_capabilities.currentTransform;
  // 指定处理交换链图像透明通道方式
  if (surface_capabilities.supportedCompositeAlpha &
      VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR)
    cInfo.compositeAlpha = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;
  else
    for (size_t i = 0; i < 4; ++i)
      if (surface_capabilities.supportedCompositeAlpha & 1 << i) {
        cInfo.compositeAlpha = VkCompositeAlphaFlagBitsKHR(
            surface_capabilities.supportedCompositeAlpha & 1 << i);
        break;
      }
  // 指定图像的用途
  cInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  cInfo.imageUsage |= surface_capabilities.supportedUsageFlags &
                      VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
  if (surface_capabilities.supportedUsageFlags &
      VK_IMAGE_USAGE_TRANSFER_DST_BIT)
    cInfo.imageUsage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
  else
    print_warning(s_TypeName,
                  "VK_IMAGE_USAGE_TRANSFER_DST_BIT isn't supported!");
  // 指定图像格式
  if (m_AvailableFormats.empty() && acquire_surface_formats(ctx))
    return CtxResult::Failed;
  if (!cInfo.imageFormat)
    if (set_surface_format(
            {VK_FORMAT_R8G8B8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},
            ctx) &&
        set_surface_format(
            {VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},
            ctx)) {
      // 如果找不到上述图像格式和色彩空间的组合，那只能有什么用什么，采用availableSurfaceFormats中的第一组
      cInfo.imageFormat = m_AvailableFormats[0].format;
      cInfo.imageColorSpace = m_AvailableFormats[0].colorSpace;
      print_warning(s_TypeName,
                    "Failed to select a four-component UNORM surface format!");
    }
  // 指定呈现模式
  std::vector<VkPresentModeKHR> surfacePresentModes;
  if (acquire_present_modes(surfacePresentModes, ctx))
    return CtxResult::AcquirePresentModesFailed;
  cInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
  if (!info.m_isFrameRateLimited)
    for (size_t i = 0; i < surfacePresentModes.size(); i++)
      if (surfacePresentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
        cInfo.presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
        break;
      }
  const char *mode_str = string_VkPresentModeKHR(cInfo.presentMode);
  print_log(s_TypeName, "Present Mode", mode_str);
  //---------------------------------------------------------------------------
  cInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  cInfo.flags = info.m_flags;
  cInfo.surface = m_Surface;
  cInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  cInfo.clipped = VK_TRUE;
  cInfo.oldSwapchain = VK_NULL_HANDLE;
  cInfo.pNext = nullptr;
  //---------------------------------------------------------------------------
  if (VkResult result = create_swapchain_Internal(ctx))
    return CtxResult::FuncCreateSwapchainInternalFailed;
  m_CallbackSwapchainConstruct.iterate(this);
  return CtxResult::Success;
}
template <class BaseCtx>
void WindowContext<BaseCtx>::cleanup(ContextBase &ctx) noexcept {
  if (m_Swapchain) {
    m_CallbackSwapchainDestroy.iterate(this);
    for (auto &i : m_SwapchainImageViews)
      if (i)
        vkDestroyImageView(ctx.m_Device, i, nullptr);
    vkDestroySwapchainKHR(ctx.m_Device, m_Swapchain, nullptr);
    m_SwapchainImages.clear();
    m_SwapchainImageViews.clear();
    m_Swapchain = VK_NULL_HANDLE;
    m_SwapchainCreateInfo = {};

    m_AvailableFormats.clear();
    m_CallbackSwapchainDestroy.clear();
    m_CallbackSwapchainConstruct.clear();
  }
  if (m_Surface) {
    vkDestroySurfaceKHR(ctx.m_Instance, m_Surface, nullptr);
    m_Surface = VK_NULL_HANDLE;
    BaseCtx::cleanup();
  }
}
template <class BaseCtx>
VkResult WindowContext<BaseCtx>::create_surface(ContextBase &ctx) {
  VkSurfaceKHR surface = VK_NULL_HANDLE;
  if (VkResult result = BaseCtx::make_surface(ctx, surface)) {
    print_error(s_TypeName, "Failed to create a window surface! Code:",
                string_VkResult(result));
    return VK_RESULT_MAX_ENUM;
  }
  this->m_Surface = surface;
  return VK_SUCCESS;
}
template <class BaseCtx>
VkResult WindowContext<BaseCtx>::recreate_swapchain(ContextBase &ctx) {
  auto &cInfo = m_SwapchainCreateInfo;
  VkSurfaceCapabilitiesKHR surface_capabilities{};
  // 获取窗口表面能力，每次使用必须重新获取
  if (VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
          ctx.m_PhysicalDevice, m_Surface, &surface_capabilities)) {
    print_error(s_TypeName,
                "Failed to get physical device surface capabilities! Code:",
                string_VkResult(result));
    return result;
  }
  if (surface_capabilities.currentExtent.width == 0 ||
      surface_capabilities.currentExtent.height == 0)
    return VK_SUBOPTIMAL_KHR;
  cInfo.imageExtent = surface_capabilities.currentExtent;
  cInfo.oldSwapchain = m_Swapchain;
  VkResult result = vkQueueWaitIdle(ctx.m_Queue_graphics);
  // 仅在等待图形队列成功，且图形与呈现所用队列不同时等待呈现队列
  if (!result && ctx.m_Queue_graphics != ctx.m_Queue_presentation)
    result = vkQueueWaitIdle(ctx.m_Queue_presentation);
  if (result) {
    print_error(s_TypeName, "Failed to wait for the queue to be idle! Code:",
                string_VkResult(result));
    return result;
  }
  m_CallbackSwapchainDestroy.iterate(this);
  std::ranges::for_each(m_SwapchainImageViews, [&ctx](VkImageView view) {
    if (view)
      vkDestroyImageView(ctx.m_Device, view, nullptr);
  });
  if (VkResult result = create_swapchain_Internal(ctx)) {
    print_error(s_TypeName,
                "Create swapchain failed! Code:", string_VkResult(result));
    return result;
  }
  m_CallbackSwapchainConstruct.iterate(this);
  print_log(s_TypeName, "Swapchain recreated!");
  return VK_SUCCESS;
}
template <class BaseCtx>
VkResult WindowContext<BaseCtx>::create_swapchain_Internal(ContextBase &ctx) {
  auto &cInfo = m_SwapchainCreateInfo;
  // 直接创建交换链
  if (VkResult result =
          vkCreateSwapchainKHR(ctx.m_Device, &cInfo, nullptr, &m_Swapchain)) {
    print_error(s_TypeName,
                "Failed to create a swapchain! Code:", string_VkResult(result));
    return result;
  }
  // 获取交换链图像
  uint32_t swapchainImageCount;
  if (VkResult result = vkGetSwapchainImagesKHR(
          ctx.m_Device, m_Swapchain, &swapchainImageCount, nullptr)) {
    print_error(s_TypeName,
                "Failed to get the count of swapchain images! Code:",
                string_VkResult(result));
    return result;
  }
  m_SwapchainImages.resize(swapchainImageCount);
  if (VkResult result = vkGetSwapchainImagesKHR(ctx.m_Device, m_Swapchain,
                                                &swapchainImageCount,
                                                m_SwapchainImages.data())) {
    print_error(s_TypeName, "Failed to get swapchain images! Code:",
                string_VkResult(result));
    return result;
  }
  // 直接创建交换链，并且获取交换链图像和视图
  m_SwapchainImageViews.resize(swapchainImageCount);
  VkImageViewCreateInfo imageViewCreateInfo = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
      .viewType = VK_IMAGE_VIEW_TYPE_2D,
      .format = cInfo.imageFormat,
      //.components = {}, // 四个成员皆为VK_COMPONENT_SWIZZLE_IDENTITY
      .subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}};
  for (size_t i = 0; i < swapchainImageCount; i++) {
    imageViewCreateInfo.image = m_SwapchainImages[i];
    if (VkResult result =
            vkCreateImageView(ctx.m_Device, &imageViewCreateInfo, nullptr,
                              &m_SwapchainImageViews[i])) {
      print_error(s_TypeName, "Failed to create a swapchain image view! Code:",
                  string_VkResult(result));
      return result;
    }
  }
}
template <class BaseCtx>
VkResult WindowContext<BaseCtx>::acquire_surface_formats(ContextBase &ctx) {
  // 获取窗口表面格式例程
  uint32_t surfaceFormatCount;
  if (VkResult result = vkGetPhysicalDeviceSurfaceFormatsKHR(
          ctx.m_PhysicalDevice, m_Surface, &surfaceFormatCount, nullptr)) {
    print_error(s_TypeName,
                "Failed to get the count of surface "
                "formats! Code:",
                string_VkResult(result));
    return result;
  }
  if (!surfaceFormatCount)
    print_error(s_TypeName, "Failed to find any supported surface "
                            "format!"),
        abort();
  m_AvailableFormats.resize(surfaceFormatCount);
  VkResult result = vkGetPhysicalDeviceSurfaceFormatsKHR(
      ctx.m_PhysicalDevice, m_Surface, &surfaceFormatCount,
      m_AvailableFormats.data());
  if (result)
    print_error(s_TypeName,
                "Failed to get surface formats! "
                "Code:",
                string_VkResult(result));
  return VK_SUCCESS;
}
template <class BaseCtx>
VkResult WindowContext<BaseCtx>::acquire_present_modes(
    std::vector<VkPresentModeKHR> &presentModes, ContextBase &ctx) {
  // 获取呈现模式例程
  uint32_t surfacePresentModeCount;
  if (VkResult result = vkGetPhysicalDeviceSurfacePresentModesKHR(
          ctx.m_PhysicalDevice, m_Surface, &surfacePresentModeCount, nullptr)) {
    print_error(s_TypeName,
                "Failed to get the count of surface present modes! Code:",
                string_VkResult(result));
    return result;
  }
  if (!surfacePresentModeCount)
    print_error(s_TypeName, "Failed to find any surface present mode!"),
        abort();
  presentModes.resize(surfacePresentModeCount);
  if (VkResult result = vkGetPhysicalDeviceSurfacePresentModesKHR(
          ctx.m_PhysicalDevice, m_Surface, &surfacePresentModeCount,
          presentModes.data())) {
    print_error(s_TypeName,
                "Failed to get surface present "
                "modes! Code:",
                string_VkResult(result));
    return result;
  }
  return VK_SUCCESS;
}
template <class BaseCtx>
VkResult
WindowContext<BaseCtx>::set_surface_format(VkSurfaceFormatKHR surfaceFormat,
                                           ContextBase &ctx) {
  bool formatIsAvailable = false;
  if (!surfaceFormat.format) {
    // 如果格式未指定，只匹配色彩空间，图像格式有啥就用啥
    for (auto &i : m_AvailableFormats)
      if (i.colorSpace == surfaceFormat.colorSpace) {
        m_SwapchainCreateInfo.imageFormat = i.format;
        m_SwapchainCreateInfo.imageColorSpace = i.colorSpace;
        formatIsAvailable = true;
        break;
      }
  } else
    // 否则匹配格式和色彩空间
    for (auto &i : m_AvailableFormats)
      if (i.format == surfaceFormat.format &&
          i.colorSpace == surfaceFormat.colorSpace) {
        m_SwapchainCreateInfo.imageFormat = i.format;
        m_SwapchainCreateInfo.imageColorSpace = i.colorSpace;
        formatIsAvailable = true;
        break;
      }
  // 如果没有符合的格式, 返回错误
  if (!formatIsAvailable)
    return VK_ERROR_FORMAT_NOT_SUPPORTED;
  // 如果交换链已存在，调用recreate_swapchain()重建交换链
  if (m_Swapchain)
    return recreate_swapchain(ctx);
  return VK_SUCCESS;
}
} // namespace BLT
#endif // !_BL_CORE_CONTEXTS_HPP_
