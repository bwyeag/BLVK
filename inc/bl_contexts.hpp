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
#ifndef _BL_CORE_CONTEXTS_FILE_
#define _BL_CORE_CONTEXTS_FILE_
// 本地include
#include <bl_util.hpp>
// 标准库include
#include <algorithm>
#include <cstdint>
#include <functional>
#include <mutex>
#include <span>
// 外部库include
#include <string>
#include <vector>
#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vma/vk_mem_alloc.h>
namespace blt {
//*****************************************************************************
// 辅助类
//*****************************************************************************

struct vkStructureHead {
  VkStructureType sType;
  void *pNext;
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
  Callback<ContextBase, ContextBase *> m_CallbackUpdate;

  CtxResult create_instance(const InstanceCreateInfo &info);
  CtxResult create_device(const DeviceCreateInfo &info,
                          std::span<VkSurfaceKHR> surfaces);
  void cleanup() noexcept;
  void update();

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
#ifdef DEBUG
  /// @brief 合并pCallbackData内容为一个字符串输出
  /// @param pCallbackData 被合并的debug数据
  /// @return 表示debug信息的字符串
  static std::string combine_debug_message(
      const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData);
  /// @brief 初始化debug部分
  /// @return 是否正确完成
  VkResult create_debugger();
  void insert_debug_ext_layers(std::vector<const char *> &layerNames,
                               std::vector<const char *> &extensionNames);
#endif // DEBUG

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
  VkResult init_vma(VmaAllocatorCreateFlagBits vmaFlags);
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

public:
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
#ifdef DEBUG
  static inline VkDebugUtilsMessengerEXT get_debugger() {
    return s_CurrentContext->m_Debugger;
  }
#endif // DEBUG
  static inline double get_current_time() {
    return s_CurrentContext->m_CurrentTime;
  }
  static inline double get_delta_time() {
    return s_CurrentContext->m_DeltaTime;
  }
};
//*****************************************************************************
// WindowContextBase_***
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
  // X = width, Y = height
  uint32_t m_InitSizeWidth, m_InitSizeHeight;
  uint32_t m_InitPosWidth{~0u}, m_InitPosHeight{~0u};
  uint32_t m_MaxSizeWidth = GLFW_DONT_CARE, m_MaxSizeHeight = GLFW_DONT_CARE,
           m_MinSizeWidth = GLFW_DONT_CARE, m_MinSizeHeight = GLFW_DONT_CARE;
  const char *m_InitTitle{"Window"};
  std::function<bool(GLFWmonitor *)> m_MonitorPred;
};
template <typename T> struct WindowContextBase {
#define ChildPtr static_cast<T *>(this)
  std::string m_Title;

  static CtxResult initialize() { T::init_library(); }
  static CtxResult cleanup() { T::cleanup_library(); }
  CtxResult create_window(const auto &info) { ChildPtr->create_base(info); }
  void cleanup_window() noexcept { ChildPtr->cleanup_base(); }
  VkResult make_surface(VkInstance instance, VkSurfaceKHR &surface) {
    ChildPtr->make_surface_impl(instance, surface);
  }
  void get_window_size(uint32_t &width, uint32_t &height) {
    ChildPtr->get_window_size_impl(width, height);
  }
  void set_window_size(uint32_t width, uint32_t height) {
    ChildPtr->set_window_size_impl(width, height);
  }
  const char *get_window_title() const { return m_Title.c_str(); }
  const std::string get_window_title_str() const { return m_Title; }
  void set_window_title(const char *newTitle) {
    m_Title = newTitle;
    ChildPtr->set_window_title(newTitle);
  }
  void set_window_title_tempo(const char *newTitle) {
    ChildPtr->set_window_title(newTitle);
  }
  void set_window_title(const std::string &newTitle) {
    m_Title = newTitle;
    ChildPtr->set_window_title(newTitle.c_str());
  }
  void set_window_title_tempo(const std::string &newTitle) {
    ChildPtr->set_window_title(newTitle.c_str());
  }
  void reset_window_title() { ChildPtr->set_window_title(m_Title.c_str()); }
  auto native_handle() { return ChildPtr->m_pWindow; }
#undef ChildPtr
};
struct WindowContextBase_glfw
    : public WindowContextBase<WindowContextBase_glfw> {
  friend WindowContextBase<WindowContextBase_glfw>;
  static constexpr const char *s_TypeName = "WindowContextBase_glfw";
  using CreateInfo = WindowCreateInfo_glfw;

private:
  static std::once_flag s_InitOnce_glfw;
  GLFWwindow *m_pWindow{nullptr};
  GLFWmonitor *m_pMonitor{nullptr};

  static CtxResult init_library();
  static void cleanup_library() noexcept;
  CtxResult create_window_impl(const CreateInfo &info);
  void cleanup_window_impl() noexcept;

  VkResult make_surface_impl(VkInstance instance, VkSurfaceKHR &surface);
  void get_window_size_impl(uint32_t &width, uint32_t &height) {
    glfwGetWindowSize(m_pWindow, (int *)&width, (int *)&height);
  }
  void set_window_size_impl(uint32_t width, uint32_t height) {
    glfwSetWindowSize(m_pWindow, width, height);
  }
  void set_window_title_impl(const char *newTitle) {
    glfwSetWindowTitle(m_pWindow, newTitle);
  }
};
//*****************************************************************************
// WindowContext 模板类
//*****************************************************************************

struct SwapchainCreateInfo {
  bool m_isFrameRateLimited;
  VkSwapchainCreateFlagsKHR m_flags;
};
template <typename BaseCtx, typename _Ctx = ContextTraits>
struct WindowContext : public BaseCtx {
  static constexpr const char *s_TypeName = "WindowContext";
  VkSurfaceKHR m_Surface{VK_NULL_HANDLE};

  VkSwapchainKHR m_Swapchain{VK_NULL_HANDLE};
  std::vector<VkImage> m_SwapchainImages;
  std::vector<VkImageView> m_SwapchainImageViews;
  VkSwapchainCreateInfoKHR m_SwapchainCreateInfo{};

  std::vector<VkSurfaceFormatKHR> m_AvailableFormats;

  Callback<WindowContext, WindowContext *> m_CallbackSwapchainDestroy;
  Callback<WindowContext, WindowContext *> m_CallbackSwapchainConstruct;

  CtxResult create(const SwapchainCreateInfo &info);
  void cleanup() noexcept;

  /// @brief 创建窗口表面, 应当委托到BaseCtx执行
  /// @param ctx 使用的Vulkan上下文
  /// @return 是否成功执行
  VkResult create_surface();
  VkSurfaceKHR get_surface() { return m_Surface; }

  /// @brief 重建交换链
  /// @param ctx 使用的Vulkan上下文
  /// @return 是否成功执行
  VkResult recreate_swapchain();

protected:
  /// @brief 直接创建交换链，并且获取交换链图像和视图，不调用回调
  /// @param ctx 使用的Vulkan上下文
  /// @return 是否成功执行
  VkResult create_swapchain_Internal();
  /// @brief 获取窗口表面格式，在create()中调用
  /// @param ctx 使用的Vulkan上下文
  /// @return 是否成功执行
  VkResult acquire_surface_formats();
  /// @brief 获取窗口呈现模式，在create()中调用
  /// @param presentModes
  /// @param ctx 使用的Vulkan上下文
  /// @return 是否成功执行
  VkResult acquire_present_modes(std::vector<VkPresentModeKHR> &presentModes);
  /// @brief 设定当前窗口表面格式
  /// @param surfaceFormat
  /// @param ctx 使用的Vulkan上下文
  /// @return 是否成功执行
  VkResult set_surface_format(VkSurfaceFormatKHR surfaceFormat);
};
} // namespace blt

// 模板函数实现
namespace blt {
//*****************************************************************************
// WindowContext 类
//*****************************************************************************
template <typename BaseCtx, typename _Ctx>
CtxResult
WindowContext<BaseCtx, _Ctx>::create(const SwapchainCreateInfo &info) {
  VkSurfaceCapabilitiesKHR surface_capabilities;
  // 获取surface支持能力
  if (VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
          _Ctx::get_phyDevice(), m_Surface, &surface_capabilities)) {
    print_error(s_TypeName,
                "Failed to get physical device surface capabilities! Code:",
                string_VkResult(result));
    return CtxResult::FuncGetPhysicalDeviceSurfaceCapFailed;
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
  if (m_AvailableFormats.empty() && acquire_surface_formats())
    return CtxResult::AcquireSurfaceFormatsFailed;
  if (!cInfo.imageFormat)
    if (set_surface_format(
            {VK_FORMAT_R8G8B8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR}) &&
        set_surface_format(
            {VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR})) {
      // 如果找不到上述图像格式和色彩空间的组合，则采用availableSurfaceFormats中的第一组
      cInfo.imageFormat = m_AvailableFormats[0].format;
      cInfo.imageColorSpace = m_AvailableFormats[0].colorSpace;
      print_warning(s_TypeName,
                    "Failed to select a four-component UNORM surface format!");
    }
  // 指定呈现模式
  std::vector<VkPresentModeKHR> surfacePresentModes;
  if (acquire_present_modes(surfacePresentModes))
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
  if (create_swapchain_Internal())
    return CtxResult::FuncCreateSwapchainInternalFailed;
  m_CallbackSwapchainConstruct.iterate(this);
  return CtxResult::Success;
}
template <typename BaseCtx, typename _Ctx>
void WindowContext<BaseCtx, _Ctx>::cleanup() noexcept {
  if (m_Swapchain) {
    m_CallbackSwapchainDestroy.iterate(this);
    for (auto &i : m_SwapchainImageViews)
      if (i)
        vkDestroyImageView(_Ctx::get_device(), i, nullptr);
    vkDestroySwapchainKHR(_Ctx::get_device(), m_Swapchain, nullptr);
    m_SwapchainImages.clear();
    m_SwapchainImageViews.clear();
    m_Swapchain = VK_NULL_HANDLE;
    m_SwapchainCreateInfo = {};

    m_AvailableFormats.clear();
    m_CallbackSwapchainDestroy.clear();
    m_CallbackSwapchainConstruct.clear();
  }
  if (m_Surface) {
    vkDestroySurfaceKHR(_Ctx::get_instance(), m_Surface, nullptr);
    m_Surface = VK_NULL_HANDLE;
    BaseCtx::cleanup_base();
  }
}
template <typename BaseCtx, typename _Ctx>
VkResult WindowContext<BaseCtx, _Ctx>::create_surface() {
  VkSurfaceKHR surface = VK_NULL_HANDLE;
  if (VkResult result = BaseCtx::make_surface(_Ctx::get_instance(), surface)) {
    print_error(s_TypeName, "Failed to create a window surface! Code:",
                string_VkResult(result));
    return VK_RESULT_MAX_ENUM;
  }
  this->m_Surface = surface;
  return VK_SUCCESS;
}
template <typename BaseCtx, typename _Ctx>
VkResult WindowContext<BaseCtx, _Ctx>::recreate_swapchain() {
  auto &cInfo = m_SwapchainCreateInfo;
  VkSurfaceCapabilitiesKHR surface_capabilities{};
  // 获取窗口表面能力，每次使用必须重新获取
  if (VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
          _Ctx::get_phyDevice(), m_Surface, &surface_capabilities)) {
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
  VkResult result = vkQueueWaitIdle(_Ctx::get_queue_graphics());
  // 仅在等待图形队列成功，且图形与呈现所用队列不同时等待呈现队列
  if (!result && _Ctx::get_queue_graphics() != _Ctx::get_queue_presentation())
    result = vkQueueWaitIdle(_Ctx::get_queue_presentation());
  if (result) {
    print_error(s_TypeName, "Failed to wait for the queue to be idle! Code:",
                string_VkResult(result));
    return result;
  }
  m_CallbackSwapchainDestroy.iterate(this);
  std::ranges::for_each(m_SwapchainImageViews, [](VkImageView view) {
    if (view)
      vkDestroyImageView(_Ctx::get_device(), view, nullptr);
  });
  if (VkResult result = create_swapchain_Internal()) {
    print_error(s_TypeName,
                "Create swapchain failed! Code:", string_VkResult(result));
    return result;
  }
  m_CallbackSwapchainConstruct.iterate(this);
  print_log(s_TypeName, std::format("Swapchain recreated! New extent:{},{}",
                                    m_SwapchainCreateInfo.imageExtent.width,
                                    m_SwapchainCreateInfo.imageExtent.height));
  return VK_SUCCESS;
}
template <typename BaseCtx, typename _Ctx>
VkResult WindowContext<BaseCtx, _Ctx>::create_swapchain_Internal() {
  auto &cInfo = m_SwapchainCreateInfo;
  // 直接创建交换链
  if (VkResult result = vkCreateSwapchainKHR(_Ctx::get_device(), &cInfo,
                                             nullptr, &m_Swapchain)) {
    print_error(s_TypeName,
                "Failed to create a swapchain! Code:", string_VkResult(result));
    return result;
  }
  // 获取交换链图像
  uint32_t swapchainImageCount;
  if (VkResult result = vkGetSwapchainImagesKHR(
          _Ctx::get_device(), m_Swapchain, &swapchainImageCount, nullptr)) {
    print_error(s_TypeName,
                "Failed to get the count of swapchain images! Code:",
                string_VkResult(result));
    return result;
  }
  m_SwapchainImages.resize(swapchainImageCount);
  if (VkResult result = vkGetSwapchainImagesKHR(_Ctx::get_device(), m_Swapchain,
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
            vkCreateImageView(_Ctx::get_device(), &imageViewCreateInfo, nullptr,
                              &m_SwapchainImageViews[i])) {
      print_error(s_TypeName, "Failed to create a swapchain image view! Code:",
                  string_VkResult(result));
      return result;
    }
  }
  return VK_SUCCESS;
}
template <typename BaseCtx, typename _Ctx>
VkResult WindowContext<BaseCtx, _Ctx>::acquire_surface_formats() {
  // 获取窗口表面格式例程
  uint32_t surfaceFormatCount;
  if (VkResult result = vkGetPhysicalDeviceSurfaceFormatsKHR(
          _Ctx::get_phyDevice(), m_Surface, &surfaceFormatCount, nullptr)) {
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
      _Ctx::get_phyDevice(), m_Surface, &surfaceFormatCount,
      m_AvailableFormats.data());
  if (result)
    print_error(s_TypeName,
                "Failed to get surface formats! "
                "Code:",
                string_VkResult(result));
  return VK_SUCCESS;
}
template <typename BaseCtx, typename _Ctx>
VkResult WindowContext<BaseCtx, _Ctx>::acquire_present_modes(
    std::vector<VkPresentModeKHR> &presentModes) {
  // 获取呈现模式例程
  uint32_t surfacePresentModeCount;
  if (VkResult result = vkGetPhysicalDeviceSurfacePresentModesKHR(
          _Ctx::get_phyDevice(), m_Surface, &surfacePresentModeCount,
          nullptr)) {
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
          _Ctx::get_phyDevice(), m_Surface, &surfacePresentModeCount,
          presentModes.data())) {
    print_error(s_TypeName,
                "Failed to get surface present "
                "modes! Code:",
                string_VkResult(result));
    return result;
  }
  return VK_SUCCESS;
}
template <typename BaseCtx, typename _Ctx>
VkResult WindowContext<BaseCtx, _Ctx>::set_surface_format(
    VkSurfaceFormatKHR surfaceFormat) {
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
    return recreate_swapchain();
  return VK_SUCCESS;
}
} // namespace blt
#endif // !_BL_CORE_CONTEXTS_FILE_
