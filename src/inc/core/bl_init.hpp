#ifndef _BL_CORE_BL_INIT_HPP_
#define _BL_CORE_BL_INIT_HPP_
#define GLFW_INCLUDE_VULKAN
#include <bl_output.hpp>
#include <core/bl_util.hpp>

#include <GLFW/glfw3.h>
#define VMA_VULKAN_VERSION VK_API_VERSION_1_3
#include <vma/vk_mem_alloc.h>
#include <vulkan/vulkan.h>

#include <algorithm>
#include <map>
#include <span>
#include <sstream>
namespace BLVK {
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
    SWAPCHAIN_CREATE_FAILED = -19
};

/// @brief  Vulkan 实例创建信息
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
/// @brief Vulkan 设备创建信息
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
    vk_swapchain_construct,
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
struct SwapchainCreateInfo {
    bool isFrameRateLimited;
    VkSwapchainCreateFlagsKHR flags;
};

struct WindowContext;
struct Context;

/// @brief 一些回调相关的类型注册
namespace _detail_init {
using CB = WindowCallback;
template <CB type>
struct GetType {};
#define _GETTYPE(e, ...)                            \
    template <>                                     \
    struct GetType<e> {                             \
        using Type = Callback<250127, __VA_ARGS__>; \
    }
_GETTYPE(CB::glfw_windowpos_func, WindowContext*, int, int);
_GETTYPE(CB::glfw_windowsize_func, WindowContext*, int, int);
_GETTYPE(CB::glfw_windowclose_func, WindowContext*);
_GETTYPE(CB::glfw_windowrefresh_func, WindowContext*);
_GETTYPE(CB::glfw_windowfocus_func, WindowContext*, int);
_GETTYPE(CB::glfw_windowiconify_func, WindowContext*, int);
_GETTYPE(CB::glfw_windowmaximize_func, WindowContext*, int);
_GETTYPE(CB::glfw_framebuffersize_func, WindowContext*, int, int);
_GETTYPE(CB::glfw_windowcontentscale_func, WindowContext*, float, float);
_GETTYPE(CB::glfw_mousebutton_func, WindowContext*, int, int, int);
_GETTYPE(CB::glfw_cursorpos_func, WindowContext*, double, double);
_GETTYPE(CB::glfw_cursorenter_func, WindowContext*, int);
_GETTYPE(CB::glfw_scroll_func, WindowContext*, double, double);
_GETTYPE(CB::glfw_key_func, WindowContext*, int, int, int, int);
_GETTYPE(CB::glfw_char_func, WindowContext*, unsigned int);
_GETTYPE(CB::glfw_charmods_func, WindowContext*, unsigned int, int);
_GETTYPE(CB::glfw_drop_func, WindowContext*, int, const char*[]);
_GETTYPE(CB::vk_swapchain_construct, );
_GETTYPE(CB::vk_swapchain_destroy, GLFWwindow*, int, const char*[]);
#undef _GETTYPE
void __glfw_callback_windowpos(GLFWwindow* window, int xpos, int ypos);
void __glfw_callback_windowsize(GLFWwindow* window, int width, int height);
void __glfw_callback_windowclose(GLFWwindow* window);
void __glfw_callback_windowrefresh(GLFWwindow* window);
void __glfw_callback_windowfocus(GLFWwindow* window, int focused);
void __glfw_callback_windowiconify(GLFWwindow* window, int iconified);
void __glfw_callback_windowmaximize(GLFWwindow* window, int maximized);
void __glfw_callback_windowcontentscale(GLFWwindow* window,
                                        float xscale,
                                        float yscale);
void __glfw_callback_mousebutton(GLFWwindow* window,
                                 int button,
                                 int action,
                                 int mods);
void __glfw_callback_cursorpos(GLFWwindow* window, double xpos, double ypos);
void __glfw_callback_cursorenter(GLFWwindow* window, int entered);
void __glfw_callback_scroll(GLFWwindow* window, double xoffset, double yoffset);
void __glfw_callback_keybord(GLFWwindow* window,
                             int key,
                             int scancode,
                             int action,
                             int mods);
void __glfw_callback_charinput(GLFWwindow* window, unsigned int codepoint);
void __glfw_callback_charmods(GLFWwindow* window,
                              unsigned int codepoint,
                              int mods);
void __glfw_callback_drop(GLFWwindow* window,
                          int path_count,
                          const char* paths[]);
template <CB Type>
void callback_set(WindowContext* ctx) {
    static_assert(false, "callback_set(): Wrong CBEnum Type!");
}
#define _SETCB(e, sent)                         \
    template <>                                 \
    void callback_set<e>(WindowContext * ctx) { \
        sent;                                   \
    }
_SETCB(CB::glfw_windowpos_func,
       glfwSetWindowPosCallback(ctx->pWindow, __glfw_callback_windowpos))
_SETCB(CB::glfw_windowsize_func,
       glfwSetWindowSizeCallback(ctx->pWindow, __glfw_callback_windowsize))
_SETCB(CB::glfw_windowclose_func,
       glfwSetWindowCloseCallback(ctx->pWindow, __glfw_callback_windowclose))
_SETCB(CB::glfw_windowrefresh_func,
       glfwSetWindowRefreshCallback(ctx->pWindow,
                                    __glfw_callback_windowrefresh))
_SETCB(CB::glfw_windowfocus_func,
       glfwSetWindowFocusCallback(ctx->pWindow, __glfw_callback_windowfocus))
_SETCB(CB::glfw_windowiconify_func,
       glfwSetWindowIconifyCallback(ctx->pWindow,
                                    __glfw_callback_windowiconify))
_SETCB(CB::glfw_windowmaximize_func,
       glfwSetWindowMaximizeCallback(ctx->pWindow,
                                     __glfw_callback_windowmaximize))
_SETCB(CB::glfw_windowcontentscale_func,
       glfwSetWindowContentScaleCallback(ctx->pWindow,
                                         __glfw_callback_windowcontentscale))
_SETCB(CB::glfw_key_func,
       glfwSetKeyCallback(ctx->pWindow, __glfw_callback_keybord))
_SETCB(CB::glfw_char_func,
       glfwSetCharCallback(ctx->pWindow, __glfw_callback_charinput))
_SETCB(CB::glfw_charmods_func,
       glfwSetCharModsCallback(ctx->pWindow, __glfw_callback_charmods))
_SETCB(CB::glfw_mousebutton_func,
       glfwSetMouseButtonCallback(ctx->pWindow, __glfw_callback_mousebutton))
_SETCB(CB::glfw_cursorpos_func,
       glfwSetCursorPosCallback(ctx->pWindow, __glfw_callback_cursorpos))
_SETCB(CB::glfw_cursorenter_func,
       glfwSetCursorEnterCallback(ctx->pWindow, __glfw_callback_cursorenter))
_SETCB(CB::glfw_scroll_func,
       glfwSetScrollCallback(ctx->pWindow, __glfw_callback_scroll))
_SETCB(CB::glfw_drop_func,
       glfwSetDropCallback(ctx->pWindow, __glfw_callback_drop))
#undef _SET_CB
}  // namespace _detail_init

/// @brief 保存窗口回调
struct WindowCallbacks {
    using CB = WindowCallback;
    _detail_init::GetType<CB::glfw_windowpos_func> callback_windowpos;
    _detail_init::GetType<CB::glfw_windowsize_func> callback_windowsize;
    _detail_init::GetType<CB::glfw_windowclose_func> callback_windowclose;
    _detail_init::GetType<CB::glfw_windowrefresh_func> callback_windowrefresh;
    _detail_init::GetType<CB::glfw_windowfocus_func> callback_windowfocus;
    _detail_init::GetType<CB::glfw_windowiconify_func> callback_windowiconify;
    _detail_init::GetType<CB::glfw_windowmaximize_func> callback_windowmaximize;
    _detail_init::GetType<CB::glfw_framebuffersize_func>
        callback_framebuffersize;
    _detail_init::GetType<CB::glfw_windowcontentscale_func>
        callback_windowcontentscale;
    _detail_init::GetType<CB::glfw_mousebutton_func> callback_mousebutton;
    _detail_init::GetType<CB::glfw_cursorpos_func> callback_cursorpos;
    _detail_init::GetType<CB::glfw_cursorenter_func> callback_cursorenter;
    _detail_init::GetType<CB::glfw_scroll_func> callback_scroll;
    _detail_init::GetType<CB::glfw_key_func> callback_key;
    _detail_init::GetType<CB::glfw_char_func> callback_char;
    _detail_init::GetType<CB::glfw_charmods_func> callback_charmods;
    _detail_init::GetType<CB::glfw_drop_func> callback_drop;
    _detail_init::GetType<CB::glfw_drop_func> callback_swapchain_destroy;
    _detail_init::GetType<CB::glfw_drop_func> callback_swapchain_construct;
};
/// @brief 窗口上下文
struct WindowContext {
    GLFWwindow* pWindow{nullptr};
    GLFWmonitor* pMonitor{nullptr};
    std::string title;

    WindowCallbacks callback;

    VkSurfaceKHR surface{VK_NULL_HANDLE};
    VkSwapchainKHR swapchain{VK_NULL_HANDLE};
    std::vector<VkImage> swapchainImages;
    std::vector<VkImageView> swapchainImageViews;
    VkSwapchainCreateInfoKHR swapchainCreateInfo{};

    std::vector<VkSurfaceFormatKHR> availableFormats;

    /// @brief 创建窗口
    /// @param info 窗口创建信息
    /// @param ctx 使用的Vulkan上下文
    /// @return 是否成功执行
    CtxResult prepare_window(WindowCreateInfo& info, Context& ctx);
    /// @brief 创建窗口表面
    /// @param ctx 使用的Vulkan上下文
    /// @return 是否成功执行
    VkResult prepare_surface(Context& ctx);
    /// @brief 创建交换链
    /// @param info 交换链创建信息
    /// @param ctx 使用的Vulkan上下文
    /// @return 是否成功执行
    CtxResult prepare_swapchain(SwapchainCreateInfo info, Context& ctx);

    /// @brief 重建交换链
    /// @param ctx 使用的Vulkan上下文
    /// @return 是否成功执行
    VkResult recreateSwapchain(Context& ctx);
    /// @brief 直接创建交换链，并且获取交换链图像和视图，不调用回调
    /// @param ctx 使用的Vulkan上下文
    /// @return 是否成功执行
    VkResult create_swapchain_Internal(Context& ctx);
    /// @brief 
    /// @param ctx 使用的Vulkan上下文
    /// @return 是否成功执行
    VkResult acquire_surface_formats(Context& ctx);
    /// @brief 
    /// @param presentModes 
    /// @param ctx 使用的Vulkan上下文
    /// @return 是否成功执行
    VkResult acquire_present_modes(std::vector<VkPresentModeKHR>& presentModes,Context& ctx);
    /// @brief 
    /// @param surfaceFormat 
    /// @return 是否成功执行
    VkResult set_surface_format(VkSurfaceFormatKHR surfaceFormat);
    /// @brief 
    /// @param info 
    /// @param ctx 使用的Vulkan上下文
    /// @return 是否成功执行
    VkResult create_swapchain(const SwapchainCreateInfo& info, Context& ctx);

    void cleanup(Context& ctx);

    template <WindowCallback type>
    auto insert(_detail_init::GetType<type>::Func&& fn)
        -> _detail_init::GetType<type>::Handle;
    template <WindowCallback type>
    void erase(_detail_init::GetType<type>::Handle handle);
    template <WindowCallback type, typename... Args>
    void iterate(Args... args);
};
const char* get_present_mode_string(VkPresentModeKHR mode);
/// @brief Vulkan 上下文
struct Context {
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

    int monitorCount;
    GLFWmonitor** pMonitors;
    std::vector<WindowContext> windowData;

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
    CtxResult prepare_window();
    /// @brief 创建窗口
    /// @param info 创建信息
    /// @param ret 返回窗口数据
    /// @return 是否正确完成
    CtxResult create_window(WindowCreateInfo& info, WindowContext*& ret);
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
        bool enableGraphicsQueue = true,
        bool enableComputeQueue = true);
    /// @brief 获取物理设备属性
    void acquire_physical_divice_properties();
    /// @brief 获取物理设备特性
    void acquire_physical_divice_features();
    /// @brief 初始化物理设备
    /// @return 是否正确完成
    CtxResult prepare_physical_device();
    VkResult acquire_device_extensions(
        std::vector<VkExtensionProperties>& extensionNames,
        const char* layerName = nullptr);
    VmaAllocatorCreateFlagBits check_VMA_extensions(
        std::vector<const char*>& extensionNames);

    /// @brief 检查设备扩展
    /// @param extensionNames 扩展名称
    /// @param layerName 被检查的层级, 一律为nullptr
    /// @return 是否正确完成
    VkResult check_device_extension(std::span<const char*> extensionNames,
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

    Context() {}
    ~Context() {}
};
struct vkStructureHead {
    VkStructureType sType;
    void* pNext;
};
/// @brief 线程本地数据，方便获取上下文
struct ThreadData {
    Context* current_vkcontext{nullptr};
    std::stringstream local_sstm;
};
thread_local static ThreadData local_data{};
/// @brief 获取当前线程的Vulkan上下文
/// @return
inline Context& cur_context() {
    return *local_data.current_vkcontext;
}
/// @brief 获取线程本地数据
/// @return 线程本地数据
inline ThreadData& acquire_local_data() {
    return local_data;
}
/// @brief 设置ctx为本线程的Vulkan上下文
/// @param ctx Vulkan上下文
inline void make_current_context(Context& ctx) {
    local_data.current_vkcontext = &ctx;
}

/// @brief 在对应Window的type回调函数中加入一个回调
/// @tparam type 回调类型
/// @param fn 函数对象
/// @return 回调句柄
template <WindowCallback type>
auto WindowContext::insert(_detail_init::GetType<type>::Func&& fn)
    -> _detail_init::GetType<type>::Handle {
    switch
        constexpr(type) {
#define _CASE(e, name)             \
    case e:                        \
        if (name.size() == 0u)     \
            callback_set<e>(this); \
        return callback.name.insert(std::forward(fn));
#define _CASE2(e, name) \
    case e:             \
        return callback.name.insert(std::forward(fn));
            _CASE(WindowCallback::glfw_windowpos_func, callback_windowpos)
            _CASE(WindowCallback::glfw_windowsize_func, callback_windowsize)
            _CASE(WindowCallback::glfw_windowclose_func, callback_windowclose)
            _CASE(WindowCallback::glfw_windowrefresh_func,
                  callback_windowrefresh)
            _CASE(WindowCallback::glfw_windowfocus_func, callback_windowfocus)
            _CASE(WindowCallback::glfw_windowiconify_func,
                  callback_windowiconify)
            _CASE(WindowCallback::glfw_windowmaximize_func,
                  callback_windowmaximize)
            _CASE(WindowCallback::glfw_framebuffersize_func,
                  callback_framebuffersize)
            _CASE(WindowCallback::glfw_windowcontentscale_func,
                  callback_windowcontentscale)
            _CASE(WindowCallback::glfw_mousebutton_func, callback_mousebutton)
            _CASE(WindowCallback::glfw_cursorpos_func, callback_cursorpos)
            _CASE(WindowCallback::glfw_cursorenter_func, callback_cursorenter)
            _CASE(WindowCallback::glfw_scroll_func, callback_scroll)
            _CASE(WindowCallback::glfw_key_func, callback_key)
            _CASE(WindowCallback::glfw_char_func, callback_char)
            _CASE(WindowCallback::glfw_charmods_func, callback_charmods)
            _CASE(WindowCallback::glfw_drop_func, callback_drop)
            _CASE2(WindowCallback::vk_swapchain_destroy,
                   callback_swapchain_destroy)
            _CASE2(WindowCallback::vk_swapchain_construct,
                   callback_swapchain_construct)
#undef _CASE2
#undef _CASE
            default:
                static_assert(false, "Wrong Callback type!");
                break;
        }
}
/// @brief 删除回调函数
/// @tparam type 对应的回调类型
/// @param handle 回调句柄
template <WindowCallback type>
void WindowContext::erase(_detail_init::GetType<type>::Handle handle) {
    switch
        constexpr(type) {
#define _CASE(e, name)               \
    case e:                          \
        callback.name.erase(handle); \
        break;
            _CASE(WindowCallback::glfw_windowpos_func, callback_windowpos)
            _CASE(WindowCallback::glfw_windowsize_func, callback_windowsize)
            _CASE(WindowCallback::glfw_windowclose_func, callback_windowclose)
            _CASE(WindowCallback::glfw_windowrefresh_func,
                  callback_windowrefresh)
            _CASE(WindowCallback::glfw_windowfocus_func, callback_windowfocus)
            _CASE(WindowCallback::glfw_windowiconify_func,
                  callback_windowiconify)
            _CASE(WindowCallback::glfw_windowmaximize_func,
                  callback_windowmaximize)
            _CASE(WindowCallback::glfw_framebuffersize_func,
                  callback_framebuffersize)
            _CASE(WindowCallback::glfw_windowcontentscale_func,
                  callback_windowcontentscale)
            _CASE(WindowCallback::glfw_mousebutton_func, callback_mousebutton)
            _CASE(WindowCallback::glfw_cursorpos_func, callback_cursorpos)
            _CASE(WindowCallback::glfw_cursorenter_func, callback_cursorenter)
            _CASE(WindowCallback::glfw_scroll_func, callback_scroll)
            _CASE(WindowCallback::glfw_key_func, callback_key)
            _CASE(WindowCallback::glfw_char_func, callback_char)
            _CASE(WindowCallback::glfw_charmods_func, callback_charmods)
            _CASE(WindowCallback::glfw_drop_func, callback_drop)
            _CASE(WindowCallback::vk_swapchain_destroy,
                  callback_swapchain_destroy)
            _CASE(WindowCallback::vk_swapchain_construct,
                  callback_swapchain_construct)
#undef _CASE
            default:
                static_assert(false, "Wrong Callback type!");
                break;
        }
}
/// @brief 调用回调函数
/// @tparam type 回调类型
template <WindowCallback type, typename... Args>
void WindowContext::iterate(Args... args) {
    constexpr size_t typen{static_cast<size_t>(type)};
    static_assert(
        0 < typen && typen < static_cast<size_t>(WindowCallback::MAX_ENUM),
        "Wrong enum type");
    static_assert(
        requires(_detail_init::GetType<type>::Func fn, Args... args) {
            fn(this, args...);
        }, "Wrong argument");
    switch
        constexpr(type) {
#define _CASE(e, name)                  \
    case e:                             \
        callback.name.iterate(args...); \
        break;
            _CASE(WindowCallback::glfw_windowpos_func, callback_windowpos)
            _CASE(WindowCallback::glfw_windowsize_func, callback_windowsize)
            _CASE(WindowCallback::glfw_windowclose_func, callback_windowclose)
            _CASE(WindowCallback::glfw_windowrefresh_func,
                  callback_windowrefresh)
            _CASE(WindowCallback::glfw_windowfocus_func, callback_windowfocus)
            _CASE(WindowCallback::glfw_windowiconify_func,
                  callback_windowiconify)
            _CASE(WindowCallback::glfw_windowmaximize_func,
                  callback_windowmaximize)
            _CASE(WindowCallback::glfw_framebuffersize_func,
                  callback_framebuffersize)
            _CASE(WindowCallback::glfw_windowcontentscale_func,
                  callback_windowcontentscale)
            _CASE(WindowCallback::glfw_mousebutton_func, callback_mousebutton)
            _CASE(WindowCallback::glfw_cursorpos_func, callback_cursorpos)
            _CASE(WindowCallback::glfw_cursorenter_func, callback_cursorenter)
            _CASE(WindowCallback::glfw_scroll_func, callback_scroll)
            _CASE(WindowCallback::glfw_key_func, callback_key)
            _CASE(WindowCallback::glfw_char_func, callback_char)
            _CASE(WindowCallback::glfw_charmods_func, callback_charmods)
            _CASE(WindowCallback::glfw_drop_func, callback_drop)
            _CASE(WindowCallback::vk_swapchain_destroy,
                  callback_swapchain_destroy)
            _CASE(WindowCallback::vk_swapchain_construct,
                  callback_swapchain_construct)
#undef _CASE
        }
}
}  // namespace BLVK
#endif  //!_BL_CORE_BL_INIT_HPP_