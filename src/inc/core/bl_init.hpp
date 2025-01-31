#ifndef _BL_CORE_BL_INIT_HPP_
#define _BL_CORE_BL_INIT_HPP_
#include <bl_output.hpp>
#include <core/bl_util.hpp>

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#define VMA_VULKAN_VERSION 1003000
#include <vma/vk_mem_alloc.h>
#include <vulkan/vk_enum_string_helper.h>

#include <algorithm>
#include <initializer_list>
#include <map>
#include <span>
#include <sstream>
namespace BL {
/// @brief 函数返回结果枚举
enum class CtxResult {
    SUCCESS = 0,
    ACQUIRE_API_VERSION_FAILED = -1,
    API_VERSION_TOO_LOW = -2,
    INSTANCE_CREATE_FAILED = -3,
    CHECK_EXT_FAILED = -4,
    CHECK_LAYER_FAILED = -5,
    DEBUG_CREATE_FAILED = -6,
    INIT_GLFW_FAILED = -6,
    NO_MONITOR = -7,
    NO_MONITOR_CHOOSE_FUNCT = -8,
    NO_FIT_MONITOR = -9,
    GET_VIDEO_MODE_FAILED = -10,
    WRONG_ARGUMENT = -11,
    WINDOW_CREATE_FAILED = -12,
    WINDOW_SURFACE_CREATE_FAILED = -13,
    ACQUIRE_PHYSICAL_DEVICES_FAILED = -14,
    NO_FIT_PHYDEVICE = -15,
    ACQUIRE_DEVICE_EXTENSIONS_FAILED = -16,
    CREATE_DEVICE_FAILED = -17,
    VMA_CREATE_FAILED = -18,
    SWAPCHAIN_CREATE_FAILED = -19,
    SURFACE_ACQUIRE_FAILED = -20,
    ACQUIRE_GLFW_EXT_FAILED = -21
};

/// @brief 实例阶段创建信息
struct InstanceCreateInfo {
    const char* pAppName{nullptr};
    uint32_t appVersion{0u};
    uint32_t min_api_version{0u};
    bool isDebuging{true};
    std::vector<const char*> layerNames{};
    std::vector<const char*> extensionNames{};
    VkInstanceCreateFlags instanceFlag = 0;
    void* pNextInstance{nullptr};
};
/// @brief 设备阶段创建信息
struct DeviceCreateInfo {
    VkDeviceCreateFlags diviceFlags = 0u;
    VmaAllocatorCreateFlags vmaFlags = 0u;
    std::vector<const char*> extensionNames{};
    void* pNextDivice{nullptr};
};
/// @brief 窗口回调函数的枚举类型
enum class WindowCallback {
    /// @brief 窗口位置改变回调
    glfw_windowpos_func = 0,
    /// @brief 窗口大小改变回调
    glfw_windowsize_func,
    /// @brief 窗口关闭回调
    glfw_windowclose_func,
    /// @brief 窗口刷新回调
    glfw_windowrefresh_func,
    /// @brief 窗口成为输入焦点回调
    glfw_windowfocus_func,
    /// @brief 窗口最小化回调
    glfw_windowiconify_func,
    /// @brief 窗口最大化回调
    glfw_windowmaximize_func,
    /// @brief 窗口帧缓冲大小改变回调
    glfw_framebuffersize_func,
    /// @brief 窗口内容缩放改变回调
    glfw_windowcontentscale_func,
    /// @brief 鼠标按键回调
    glfw_mousebutton_func,
    /// @brief 鼠标位置回调
    glfw_cursorpos_func,
    /// @brief 鼠标进入窗口回调
    glfw_cursorenter_func,
    /// @brief 滚轮回调
    glfw_scroll_func,
    /// @brief 键盘回调
    glfw_key_func,
    /// @brief 文字输入回调
    glfw_char_func,
    /// @brief 组合键回调
    glfw_charmods_func,
    /// @brief 窗口拖入文件回调
    glfw_drop_func,
    /// @brief 创建交换链回调
    vk_swapchain_construct,
    /// @brief 销毁交换链回调
    vk_swapchain_destroy,
    MAX_ENUM
};

/// @brief 窗口创建的设置，可通过位运算组合
struct WindowCreateState_t {
    enum Type : uint16_t {
        InitUnvisiable = 0x1,  // 初始不可见
        FullScreen = 0x2,      // 窗口全屏
        Maximized = 0x4,       // 窗口最大化
        Specified = 0x6,       // 窗口自定义大小
        SizeMask = 0x6,        // 用于提取窗口大小相关信息
        Decorated = 0x8,       // 窗口是否有边框
        MousePassThrough =
            0x10,          // 鼠标操作是否会到下一层窗口（仅支持未修饰的窗口）
        Resizable = 0x20,  // 窗口是否可调节大小
        InitMouseCentered = 0x40,  // 初始鼠标是否在窗口中心
        UsePrimaryMonitor = 0x80,  // 是否使用主显示器
        AutoIconify =
            0x100,  // 全屏窗口是否在输入焦点丢失时自动图标化并恢复以前的视频模式
        WindowFloating = 0x200  // 窗口是否浮动在其他常规窗口上
    };
};
using WindowCreateState = WindowCreateState_t::Type;
/// @brief 窗口创建信息
struct WindowCreateInfo {
    using State = WindowCreateState;
    State init_state{State(State::Specified | State::Decorated |
                           State::Resizable | State::UsePrimaryMonitor)};
    uint32_t init_size_x, init_size_y;
    uint32_t init_pos_x{~0u}, init_pos_y{~0u};
    uint32_t max_size_x = GLFW_DONT_CARE, max_size_y = GLFW_DONT_CARE,
             min_size_x = GLFW_DONT_CARE, min_size_y = GLFW_DONT_CARE;
    const char* init_title;
    std::function<bool(GLFWmonitor*)> monitor_choose;
};
/// @brief 窗口交换链创建信息
struct SwapchainCreateInfo {
    bool isFrameRateLimited;
    VkSwapchainCreateFlagsKHR flags;
};
/// @brief 创建信息的包装
struct ContextCreateInfo {
    InstanceCreateInfo* instance_info;
    DeviceCreateInfo* device_info;
    std::span<std::pair<WindowCreateInfo, SwapchainCreateInfo>> window_info;
};

struct WindowContext;
struct ContextBase;
/// @brief 一些回调相关的类型注册
namespace _detail {
// void __glfw_callback_windowpos(GLFWwindow* window, int xpos, int ypos);
// void __glfw_callback_windowsize(GLFWwindow* window, int width, int height);
// void __glfw_callback_windowclose(GLFWwindow* window);
// void __glfw_callback_windowrefresh(GLFWwindow* window);
// void __glfw_callback_windowfocus(GLFWwindow* window, int focused);
// void __glfw_callback_windowiconify(GLFWwindow* window, int iconified);
// void __glfw_callback_windowmaximize(GLFWwindow* window, int maximized);
// void __glfw_callback_windowcontentscale(GLFWwindow* window,
//                                         float xscale,
//                                         float yscale);
// void __glfw_callback_mousebutton(GLFWwindow* window,
//                                  int button,
//                                  int action,
//                                  int mods);
// void __glfw_callback_cursorpos(GLFWwindow* window, double xpos, double ypos);
// void __glfw_callback_cursorenter(GLFWwindow* window, int entered);
// void __glfw_callback_scroll(GLFWwindow* window, double xoffset, double yoffset);
// void __glfw_callback_keybord(GLFWwindow* window,
//                              int key,
//                              int scancode,
//                              int action,
//                              int mods);
// void __glfw_callback_charinput(GLFWwindow* window, unsigned int codepoint);
// void __glfw_callback_charmods(GLFWwindow* window,
//                               unsigned int codepoint,
//                               int mods);
// void __glfw_callback_drop(GLFWwindow* window,
//                           int path_count,
//                           const char* paths[]);
}  // namespace _detail

/// @brief 窗口上下文基类, 负责与GLFW交互部分
struct WindowContextBase {
    GLFWwindow* pWindow{nullptr};
    GLFWmonitor* pMonitor{nullptr};
    std::string title;

    /// @brief 创建窗口
    /// @param info 窗口创建信息
    /// @return 是否成功执行
    CtxResult prepare_window(const WindowCreateInfo& info);
    void cleanup();
};
/// @brief 窗口上下文
struct WindowContext : public WindowContextBase {
    VkSurfaceKHR surface{VK_NULL_HANDLE};

    VkSwapchainKHR swapchain{VK_NULL_HANDLE};
    std::vector<VkImage> swapchainImages;
    std::vector<VkImageView> swapchainImageViews;
    VkSwapchainCreateInfoKHR swapchainCreateInfo{};

    std::vector<VkSurfaceFormatKHR> availableFormats;

    Callback<WindowContext, WindowContext*> callback_swapchain_destroy;
    Callback<WindowContext, WindowContext*> callback_swapchain_construct;

    /// @brief 创建窗口表面
    /// @param ctx 使用的Vulkan上下文
    /// @return 是否成功执行
    VkResult prepare_surface(ContextBase& ctx);
    /// @brief 重建交换链
    /// @param ctx 使用的Vulkan上下文
    /// @return 是否成功执行
    VkResult recreate_swapchain(ContextBase& ctx);
    /// @brief 直接创建交换链，并且获取交换链图像和视图，不调用回调
    /// @param ctx 使用的Vulkan上下文
    /// @return 是否成功执行
    VkResult create_swapchain_Internal(ContextBase& ctx);
    /// @brief
    /// @param ctx 使用的Vulkan上下文
    /// @return 是否成功执行
    VkResult acquire_surface_formats(ContextBase& ctx);
    /// @brief
    /// @param presentModes
    /// @param ctx 使用的Vulkan上下文
    /// @return 是否成功执行
    VkResult acquire_present_modes(std::vector<VkPresentModeKHR>& presentModes,
                                   ContextBase& ctx);
    /// @brief 设定当前窗口表面格式
    /// @param surfaceFormat
    /// @return 是否成功执行
    VkResult set_surface_format(VkSurfaceFormatKHR surfaceFormat,
                                ContextBase& ctx);
    /// @brief
    /// @param info
    /// @param ctx 使用的Vulkan上下文
    /// @return 是否成功执行
    VkResult create_swapchain(const SwapchainCreateInfo& info,
                              ContextBase& ctx);
    /// @brief 创建交换链
    /// @param info 交换链创建信息
    /// @param ctx 使用的Vulkan上下文
    /// @return 是否成功执行
    CtxResult prepare_swapchain(const SwapchainCreateInfo info,
                                ContextBase& ctx);

    void cleanup(ContextBase& ctx);
};
/// @brief Vulkan 上下文
struct ContextBase {
    uint32_t vulkanApiVersion;
    VkInstance instance{VK_NULL_HANDLE};
    VkPhysicalDevice phyDevice{VK_NULL_HANDLE};
    VkDevice device{VK_NULL_HANDLE};

    uint32_t queueFamilyIndex_graphics{VK_QUEUE_FAMILY_IGNORED};
    uint32_t queueFamilyIndex_compute{VK_QUEUE_FAMILY_IGNORED};
    uint32_t queueFamilyIndex_presentation{VK_QUEUE_FAMILY_IGNORED};

    VkQueue queue_graphics{VK_NULL_HANDLE};
    VkQueue queue_compute{VK_NULL_HANDLE};
    VkQueue queue_presentation{VK_NULL_HANDLE};

    VkPhysicalDeviceProperties2 phyDeviceProperties;
    VkPhysicalDeviceVulkan11Properties phyDeviceVulkan11Properties;
    VkPhysicalDeviceVulkan12Properties phyDeviceVulkan12Properties;
    VkPhysicalDeviceVulkan13Properties phyDeviceVulkan13Properties;

    VkPhysicalDeviceMemoryProperties2 phyDeviceMemoryProperties;

    VkPhysicalDeviceFeatures2 phyDeviceFeatures;
    VkPhysicalDeviceVulkan11Features phyDeviceVulkan11Features;
    VkPhysicalDeviceVulkan12Features phyDeviceVulkan12Features;
    VkPhysicalDeviceVulkan13Features phyDeviceVulkan13Features;

    /// @brief 当前设备可用的扩展
    std::vector<VkExtensionProperties> availableExtensions;
    /// @brief 指向availableExtensions的扩展名称, 已按照字典序排列
    std::vector<const char*> extensions;

    VkDebugUtilsMessengerEXT debugger{VK_NULL_HANDLE};

    VmaAllocator allocator;

    double current_time{0.0}, delta_time{0.0};

    /// @brief 获取VulkanAPI的版本
    /// @param version 返回版本
    /// @return 是否正确查询
    VkResult acquire_vkapi_version(uint32_t& version);
    /// @brief 检查实例扩展是否可用
    /// @param extensionNames 被检查的数组
    /// @param layerName 扩展所在的层级, 一律为nullptr
    /// @return 是否正确检查
    VkResult check_instance_extension(std::span<const char*> extensionNames,
                                      const char* layerName = nullptr);
    /// @brief 检查实例层级是否可用
    /// @param layerNames 被检查的数组
    /// @return 是否正确检查
    VkResult check_instance_layer(std::span<const char*> layerNames);
    /// @brief 合并pCallbackData内容为一个字符串输出
    /// @param pCallbackData 被合并的debug数据
    /// @return 表示debug信息的字符串
    static std::string combine_debug_message(
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData);
    /// @brief 初始化debug部分
    /// @return 是否正确完成
    VkResult prepare_debugger();
    /// @brief 初始化Vulkan实例
    /// @param info 创建信息
    /// @return 是否正确完成
    CtxResult prepare_instance(InstanceCreateInfo& info);
    /// @brief 初始化GLFW及其debug
    /// @return 是否正确完成
    CtxResult prepare_glfw();
    /// @brief 取得物理设备列表
    /// @param availablePhysicalDevices 返回该列表
    /// @return 是否正确完成
    VkResult acquire_physical_devices(
        std::vector<VkPhysicalDevice>& availablePhysicalDevices);
    /// @brief 获取设备所用的队列族
    /// @param physicalDevice 被获取的设备
    /// @param enableGraphicsQueue 是否查找图形队列
    /// @param enableComputeQueue 是否查找计算队列
    /// @param queueFamilyIndices 返回队列族索引, 依次为图形,呈现,计算
    /// @return 是否正确完成
    VkResult acquire_queue_family_indices(VkPhysicalDevice physicalDevice,
                                          uint32_t (&queueFamilyIndices)[3],
                                          std::span<WindowContext> windowData,
                                          bool enableGraphicsQueue = true,
                                          bool enableComputeQueue = true);
    /// @brief 决定使用的物理设备, 呈现队列取决于当前是否创建窗口
    /// @param availablePhysicalDevices 可用的物理设备
    /// @param deviceIndex 被决定的设备索引
    /// @param enableGraphicsQueue 是否启用图形队列
    /// @param enableComputeQueue 是否启用计算队列
    /// @return 是否正确完成
    VkResult determine_physical_device(
        std::vector<VkPhysicalDevice>& availablePhysicalDevices,
        uint32_t deviceIndex = 0,
        std::span<WindowContext> windowData = {},
        bool enableGraphicsQueue = true,
        bool enableComputeQueue = true);
    /// @brief 获取物理设备属性
    void acquire_physical_divice_properties();
    /// @brief 获取物理设备特性
    void acquire_physical_divice_features();
    /// @brief 初始化物理设备
    /// @param windowData 当前需要与设备匹配的各个窗口
    /// @return 是否正确完成
    CtxResult prepare_physical_device(std::span<WindowContext> windowData);
    VkResult acquire_device_extensions(
        std::vector<VkExtensionProperties>& extensionNames,
        const char* layerName = nullptr);
    VmaAllocatorCreateFlagBits check_VMA_extensions(
        std::vector<const char*>& extensionNames);

    /// @brief 检查设备扩展
    /// @param extensionNames 扩展名称
    /// @param layerName 被检查的层级, 一律为nullptr
    /// @return 是否正确完成
    void check_device_extension(std::span<const char*> exstensionNames,
                                const char* layerName = nullptr);
    /// @brief 初始化VMA库(内存分配)
    /// @param info 创建信息
    /// @return 是否正确完成
    VkResult prepare_VMA(DeviceCreateInfo& info);
    /// @brief 初始化设备
    /// @param info 创建信息
    /// @return 是否正确完成
    CtxResult prepare_device(DeviceCreateInfo& info);

    /// @brief 更新状态变量
    void update();

    /// @brief 清理数据
    void cleanup();

    ContextBase() {}
    ~ContextBase() {}
};
struct Context : public ContextBase {
    std::vector<WindowContext> windowData;

    /// @brief 创建窗口
    /// @param info 创建信息
    /// @param ret 返回窗口数据
    /// @return 是否正确完成
    CtxResult create_window(const WindowCreateInfo& info, WindowContext*& ret);
    /// @brief 准备整个上下文
    /// @param info 上下文创建信息
    /// @param list 各个窗口的创建信息
    /// @param ret 返回各个窗口指针
    /// @return 是否正确完成
    CtxResult prepare_context(ContextCreateInfo& info,
                              std::span<WindowContext*> ret);

    /// @brief 清理数据
    void cleanup();
};
struct vkStructureHead {
    VkStructureType sType;
    void* pNext;
};
/// @brief 线程本地数据，方便获取上下文
struct ThreadData {
    ContextBase* current_vkcontext{nullptr};
    std::stringstream local_sstm;
};
thread_local static ThreadData local_data{};
/// @brief 获取当前线程的Vulkan上下文
/// @return
inline ContextBase& cur_context() {
    return *local_data.current_vkcontext;
}
/// @brief 获取线程本地数据
/// @return 线程本地数据
inline ThreadData& acquire_local_data() {
    return local_data;
}
/// @brief 设置ctx为本线程的Vulkan上下文
/// @param ctx Vulkan上下文
inline void make_current_context(ContextBase& ctx) {
    local_data.current_vkcontext = &ctx;
}
}  // namespace BL
#endif  //!_BL_CORE_BL_INIT_HPP_