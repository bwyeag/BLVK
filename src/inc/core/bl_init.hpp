#ifndef _BL_CORE_BL_INIT_HPP_
#define _BL_CORE_BL_INIT_HPP_
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define VMA_VULKAN_VERSION VK_API_VERSION_1_3
#include <vma/vk_mem_alloc.h>
#include <vulkan/vulkan.h>
#include <algorithm>
#include <bl_output.hpp>
#include <core/bl_util.hpp>
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
    ENUM_DEVICE_EXT_FAILED = -16,
    CREATE_DEVICE_FAILED = -17,
    VMA_CREATE_FAILED = -18
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
    VkDeviceCreateFlags diviceFlags = 0;
    uint32_t surfaceCount;
    VmaAllocatorCreateFlags vmaFlags = 0u;
    std::vector<const char*> extensionNames{};
    bool debug_print_deviceExtension{false};
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
_GETTYPE(CB::glfw_windowpos_func, GLFWwindow*, int, int);
_GETTYPE(CB::glfw_windowsize_func, GLFWwindow*, int, int);
_GETTYPE(CB::glfw_windowclose_func, GLFWwindow*);
_GETTYPE(CB::glfw_windowrefresh_func, GLFWwindow*);
_GETTYPE(CB::glfw_windowfocus_func, GLFWwindow*, int);
_GETTYPE(CB::glfw_windowiconify_func, GLFWwindow*, int);
_GETTYPE(CB::glfw_windowmaximize_func, GLFWwindow*, int);
_GETTYPE(CB::glfw_framebuffersize_func, GLFWwindow*, int, int);
_GETTYPE(CB::glfw_windowcontentscale_func, GLFWwindow*, float, float);
_GETTYPE(CB::glfw_mousebutton_func, GLFWwindow*, int, int, int);
_GETTYPE(CB::glfw_cursorpos_func, GLFWwindow*, double, double);
_GETTYPE(CB::glfw_cursorenter_func, GLFWwindow*, int);
_GETTYPE(CB::glfw_scroll_func, GLFWwindow*, double, double);
_GETTYPE(CB::glfw_key_func, GLFWwindow*, int, int, int, int);
_GETTYPE(CB::glfw_char_func, GLFWwindow*, unsigned int);
_GETTYPE(CB::glfw_charmods_func, GLFWwindow*, unsigned int, int);
_GETTYPE(CB::glfw_drop_func, GLFWwindow*, int, const char*[]);
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
#define _SET_CB(e, sent)                        \
    template <>                                 \
    void callback_set<e>(WindowContext * ctx) { \
        sent;                                   \
    }
_SET_CB(CB::glfw_windowpos_func,
        glfwSetWindowPosCallback(ctx->pWindow, __glfw_callback_windowpos))
_SET_CB(CB::glfw_windowsize_func,
        glfwSetWindowSizeCallback(ctx->pWindow, __glfw_callback_windowsize))
_SET_CB(CB::glfw_windowclose_func,
        glfwSetWindowCloseCallback(ctx->pWindow, __glfw_callback_windowclose))
_SET_CB(CB::glfw_windowrefresh_func,
        glfwSetWindowRefreshCallback(ctx->pWindow,
                                     __glfw_callback_windowrefresh))
_SET_CB(CB::glfw_windowfocus_func,
        glfwSetWindowFocusCallback(ctx->pWindow, __glfw_callback_windowfocus))
_SET_CB(CB::glfw_windowiconify_func,
        glfwSetWindowIconifyCallback(ctx->pWindow,
                                     __glfw_callback_windowiconify))
_SET_CB(CB::glfw_windowmaximize_func,
        glfwSetWindowMaximizeCallback(ctx->pWindow,
                                      __glfw_callback_windowmaximize))
_SET_CB(CB::glfw_windowcontentscale_func,
        glfwSetWindowContentScaleCallback(ctx->pWindow,
                                          __glfw_callback_windowcontentscale))
_SET_CB(CB::glfw_key_func,
        glfwSetKeyCallback(ctx->pWindow, __glfw_callback_keybord))
_SET_CB(CB::glfw_char_func,
        glfwSetCharCallback(ctx->pWindow, __glfw_callback_charinput))
_SET_CB(CB::glfw_charmods_func,
        glfwSetCharModsCallback(ctx->pWindow, __glfw_callback_charmods))
_SET_CB(CB::glfw_mousebutton_func,
        glfwSetMouseButtonCallback(ctx->pWindow, __glfw_callback_mousebutton))
_SET_CB(CB::glfw_cursorpos_func,
        glfwSetCursorPosCallback(ctx->pWindow, __glfw_callback_cursorpos))
_SET_CB(CB::glfw_cursorenter_func,
        glfwSetCursorEnterCallback(ctx->pWindow, __glfw_callback_cursorenter))
_SET_CB(CB::glfw_scroll_func,
        glfwSetScrollCallback(ctx->pWindow, __glfw_callback_scroll))
_SET_CB(CB::glfw_drop_func,
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

    CtxResult prepare_window(Context& ctx, WindowCreateInfo& info);
    CtxResult prepare_swapchain(Context& ctx);
    void cleanup(Context& ctx);

    template <WindowCallback type>
    auto insert(_detail_init::GetType<type>::Func&& fn)
        -> _detail_init::GetType<type>::Handle;
    template <WindowCallback type>
    void erase(_detail_init::GetType<type>::Handle handle);
    template <WindowCallback type, typename... Args>
    void iterate(Args... args);
};
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

    // Callback callbacks;

    VkPhysicalDeviceProperties2 phyDeviceProperties;
    VkPhysicalDeviceVulkan11Properties phyDeviceVulkan11Properties;
    VkPhysicalDeviceVulkan12Properties phyDeviceVulkan12Properties;
    VkPhysicalDeviceVulkan13Properties phyDeviceVulkan13Properties;

    VkPhysicalDeviceMemoryProperties2 phyDeviceMemoryProperties;

    VkPhysicalDeviceFeatures2 phyDeviceFeatures;
    VkPhysicalDeviceVulkan11Features phyDeviceVulkan11Features;
    VkPhysicalDeviceVulkan12Features phyDeviceVulkan12Features;
    VkPhysicalDeviceVulkan13Features phyDeviceVulkan13Features;

    VkDebugUtilsMessengerEXT debugger{VK_NULL_HANDLE};

    VmaAllocator allocator;

    double current_time{0.0}, delta_time{0.0};

    int monitorCount;
    GLFWmonitor** pMonitors;
    std::vector<WindowContext> windowData;

    /// @brief
    /// @param version
    /// @return
    VkResult acquire_vkapi_version(uint32_t& version);
    /// @brief
    /// @param extensionNames
    /// @param layerName
    /// @return
    VkResult check_instance_extension(std::span<const char*> extensionNames,
                                      const char* layerName = nullptr);
    /// @brief
    /// @param layerNames
    /// @return
    VkResult check_instance_layer(std::span<const char*> layerNames);
    /// @brief
    /// @param pCallbackData
    /// @return
    static std::string combine_debug_message(
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData);
    /// @brief
    /// @return
    VkResult prepare_debugger();
    /// @brief
    /// @param info
    /// @return
    CtxResult prepare_instance(InstanceCreateInfo& info);
    /// @brief
    /// @return
    CtxResult prepare_window();
    /// @brief
    /// @param info
    /// @param ret
    /// @return
    CtxResult create_window(WindowCreateInfo& info, WindowContext*& ret);
    /// @brief
    /// @param availablePhysicalDevices
    /// @return
    VkResult acquire_physical_devices(
        std::vector<VkPhysicalDevice>& availablePhysicalDevices);
    /// @brief
    /// @param physicalDevice
    /// @param enableGraphicsQueue
    /// @param enableComputeQueue
    /// @param queueFamilyIndices
    /// @return
    VkResult get_queue_family_indices(VkPhysicalDevice physicalDevice,
                                      bool enableGraphicsQueue,
                                      bool enableComputeQueue,
                                      uint32_t (&queueFamilyIndices)[3]);
    /// @brief
    /// @param availablePhysicalDevices
    /// @param deviceIndex
    /// @param enableGraphicsQueue
    /// @param enableComputeQueue
    /// @return
    VkResult determine_physical_device(
        std::vector<VkPhysicalDevice>& availablePhysicalDevices,
        uint32_t deviceIndex = 0,
        bool enableGraphicsQueue = true,
        bool enableComputeQueue = true);
    /// @brief
    void acquire_physical_divice_properties();
    /// @brief
    void acquire_physical_divice_features();
    /// @brief
    /// @return
    CtxResult prepare_physical_device();
    VkResult acquire_device_extensions(
        std::vector<VkExtensionProperties>& extensionNames,
        const char* layerName = nullptr);
    /// @brief
    /// @param extensionNames
    /// @return
    VkResult insert_device_extensions(std::vector<const char*>& extensionNames);
    /// @brief
    /// @param extensionNames
    /// @param layerName
    VkResult check_device_extension(std::span<const char*> extensionNames,
                                    const char* layerName = nullptr);
    VkResult prepare_VMA(DeviceCreateInfo& info);
    /// @brief
    /// @param info
    /// @return
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

/// @brief
/// @tparam type
/// @param fn
/// @return
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
#undef _CASE
            default:
                static_assert(false, "Wrong Callback type!");
                break;
        }
}
/// @brief
/// @tparam type
/// @param handle
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
#undef _CASE
            default:
                static_assert(false, "Wrong Callback type!");
                break;
        }
}
/// @brief
/// @tparam ...Args
/// @tparam type
/// @param ...args
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
#define _CASE(e, name)               \
    case e:                          \
        callback.name.iterate(args); \
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
#undef _CASE
        }
}
}  // namespace BLVK
#endif  //!_BL_CORE_BL_INIT_HPP_