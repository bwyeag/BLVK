#ifndef _BOUNDLESS_WINDOW_HPP_FILE_
#define _BOUNDLESS_WINDOW_HPP_FILE_
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <cstdint>
#include <deque>
#include <functional>
#include <sstream>
#include <string>
#include <system_error>
#include <variant>
#include <vector>
namespace BL {
struct WindowInitState_t {
    enum Type : uint16_t {
        InitUnvisiable = 0x1,
        FullScreen = 0x2,
        Maximized = 0x4,
        Specified = 0x6,
        SizeMask = 0x6,
        Decorated = 0x8,
        // MousePassThrough = 0x10,
        Resizable = 0x20,
        InitMouseCentered = 0x40,
        UsePrimaryMonitor = 0x80
    };
};
using WindowInitState = WindowInitState_t::Type;
struct WindowInit_t {
    using State = WindowInitState;
    State init_state = State(State::Specified | State::Decorated |
                             State::Resizable | State::UsePrimaryMonitor);
    uint32_t init_size_x, init_size_y;
    uint32_t init_pos_x = (~0u), init_pos_y = (~0u);
    uint32_t max_size_x = GLFW_DONT_CARE, max_size_y = GLFW_DONT_CARE,
             min_size_x = GLFW_DONT_CARE, min_size_y = GLFW_DONT_CARE;
    const char* init_title;
    std::function<bool(GLFWmonitor*)> monitor_choose;
};
struct WindowErrorEnum_t {
    enum Type {
        Success = 0,
        NoMonitor,
        InitInfoLost,
        WindowCreateFailed,
        EmptyMonitorChooseFunc,
        GLFWInitFailed
    };
};
using WindowErrorEnum = WindowErrorEnum_t::Type;
class WindowErrorCategory : public std::error_category {
   public:
    WindowErrorCategory() {}
    [[nodiscard]]
    const char* name() const noexcept override {
        return "Window error";
    }
    [[nodiscard]]
    std::string message(int ev) const override;
};
[[nodiscard]]
std::error_code make_error_code(WindowErrorEnum e);
}  // namespace BL
namespace std {
template <>
struct is_error_code_enum<BL::WindowErrorEnum> : public true_type {};
}  // namespace std
namespace BL {
struct WindowContext;
struct WindowCallbackEnum_t {
    enum Type {
        AllCallbacks = 0,
        WindowPos,
        WindowSize,
        WindowClose,
        WindowRefresh,
        WindowFocus,
        WindowIconify,
        WindowMaximize,
        WindowContentScale,
        Keyboard,
        CharInput,
        CharMods,
        MouseButton,
        CursorPos,
        CursorEnter,
        Scroll,
        DropFile,
        MaxCallbackEnum  // 最后一个，标记枚举类型的数量
    };
};
using WindowCallbackEnum = WindowCallbackEnum_t::Type;
namespace _internal_windowcb {
using CBEnum = WindowCallbackEnum;
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
template <CBEnum Type>
struct FnType {};
#define _FNTYPE(e, fn)                  \
    template <>                         \
    struct FnType<e> {                  \
        using Type = std::function<fn>; \
        Type value;                     \
    };
_FNTYPE(CBEnum::WindowPos, void(WindowContext*, int, int))
_FNTYPE(CBEnum::WindowSize, void(WindowContext*, int, int))
_FNTYPE(CBEnum::WindowClose, void(WindowContext*))
_FNTYPE(CBEnum::WindowRefresh, void(WindowContext*))
_FNTYPE(CBEnum::WindowFocus, void(WindowContext*, int))
_FNTYPE(CBEnum::WindowIconify, void(WindowContext*, int))
_FNTYPE(CBEnum::WindowMaximize, void(WindowContext*, int))
_FNTYPE(CBEnum::WindowContentScale, void(WindowContext*, float, float))
_FNTYPE(CBEnum::Keyboard, void(WindowContext*, int, int, int, int))
_FNTYPE(CBEnum::CharInput, void(WindowContext*, unsigned int))
_FNTYPE(CBEnum::CharMods, void(WindowContext*, unsigned int, int))
_FNTYPE(CBEnum::MouseButton, void(WindowContext*, int, int, int))
_FNTYPE(CBEnum::CursorPos, void(WindowContext*, double, double))
_FNTYPE(CBEnum::CursorEnter, void(WindowContext*, int))
_FNTYPE(CBEnum::Scroll, void(WindowContext*, double, double))
_FNTYPE(CBEnum::DropFile, void(WindowContext*, int, const char*[]))
#undef _FNTYPE
template <CBEnum Type>
void callback_set(WindowContext* ctx) {
    static_assert(false, "callback_set(): Wrong CBEnum Type!");
}
// 此处类型的顺序必须与 WindowCallbackEnum 中的枚举顺序相同
using Wrapper = std::variant<std::monostate,
                             FnType<CBEnum::WindowPos>,
                             FnType<CBEnum::WindowSize>,
                             FnType<CBEnum::WindowClose>,
                             FnType<CBEnum::WindowRefresh>,
                             FnType<CBEnum::WindowFocus>,
                             FnType<CBEnum::WindowIconify>,
                             FnType<CBEnum::WindowMaximize>,
                             FnType<CBEnum::WindowContentScale>,
                             FnType<CBEnum::Keyboard>,
                             FnType<CBEnum::CharInput>,
                             FnType<CBEnum::CharMods>,
                             FnType<CBEnum::MouseButton>,
                             FnType<CBEnum::CursorPos>,
                             FnType<CBEnum::CursorEnter>,
                             FnType<CBEnum::Scroll>,
                             FnType<CBEnum::DropFile>>;
}  // namespace _internal_windowcb
template <WindowCallbackEnum Type>
using WindowCallbackFunc = _internal_windowcb::FnType<Type>;
using WindowCallbackWrapper = _internal_windowcb::Wrapper;
struct CallbackNode {
    CallbackNode *next, *rear;
    WindowCallbackWrapper func;
    template <typename T>
    CallbackNode(T&& fn) {
        func = std::forward<T>(fn);
    }
};
struct CallbackHandle {
    CallbackNode* ptr;
    CallbackHandle& operator=(CallbackNode* p) { ptr = p; }
    bool operator==(CallbackHandle&& rhs) { return ptr == rhs.ptr; }
    bool operator!=(CallbackHandle&& rhs) { return ptr != rhs.ptr; }
    operator bool() { return ptr != nullptr; }
    CallbackNode* operator->() { return ptr; }
};
struct WindowContext {
    GLFWwindow* pWindow = nullptr;
    GLFWmonitor* pMonitor = nullptr;
    std::string title;
    double current_time = 0.0, delta_time = 0.0;
    std::deque<CallbackNode> callbacks;
    CallbackHandle callback_free{nullptr};
    CallbackHandle callback_head[size_t(WindowCallbackEnum::MaxCallbackEnum)]{};

    // VkSurfaceKHR surface = VK_NULL_HANDLE;
    // VkSwapchainKHR swapchain = VK_NULL_HANDLE;
    // std::vector<VkImage> swapchainImages;
    // std::vector<VkImageView> swapchainImageViews;
    // VkSwapchainCreateInfoKHR swapchainCreateInfo = {};

    WindowContext() = default;
    WindowContext(const WindowInit_t& pInit, std::error_code& ec) {
        create_window(pInit, ec);
    }
    WindowContext(WindowContext&&) = delete;
    ~WindowContext() {}
    static void initialize(std::error_code& ec);
    static void terminate();
    static void wait_events() { glfwWaitEvents(); }
    static void wait_events(double seconds) { glfwWaitEventsTimeout(seconds); }
    static void poll_events() { glfwPollEvents(); }
    static void post_empty_event() { glfwPostEmptyEvent(); }

    void create_window(const WindowInit_t& pInit, std::error_code& ec) noexcept;
    void destroy() noexcept;
    void update() noexcept;

    void set_window_size(uint32_t w, uint32_t h) {
        glfwSetWindowSize(pWindow, w, h);
    }
    void get_window_size(uint32_t& w, uint32_t& h) {
        glfwGetWindowSize(pWindow, (int*)(&w), (int*)(&h));
    }
    void set_window_pos(uint32_t w, uint32_t h) {
        glfwSetWindowPos(pWindow, w, h);
    }
    void get_window_pos(uint32_t& w, uint32_t& h) {
        glfwGetWindowPos(pWindow, (int*)(&w), (int*)(&h));
    }
    void iconify_window() { glfwIconifyWindow(pWindow); }
    void restore_window() { glfwRestoreWindow(pWindow); }
    void maximize_window() { glfwMaximizeWindow(pWindow); }
    [[nodiscard]]
    int is_maximized() {
        return glfwGetWindowAttrib(pWindow, GLFW_MAXIMIZED);
    }
    void hide_window() { glfwHideWindow(pWindow); }
    void show_window() { glfwShowWindow(pWindow); }
    [[nodiscard]]
    int is_visible() {
        return glfwGetWindowAttrib(pWindow, GLFW_VISIBLE);
    }
    void focus_window() { glfwFocusWindow(pWindow); }
    [[nodiscard]]
    int is_focused() {
        return glfwGetWindowAttrib(pWindow, GLFW_FOCUSED);
    }
    void request_attention() { glfwRequestWindowAttention(pWindow); }
    void set_size_limits(uint32_t min_w,
                         uint32_t min_h,
                         uint32_t max_w,
                         uint32_t max_h) {
        glfwSetWindowSizeLimits(pWindow, min_w, min_h, max_w, max_h);
    }
    void clear_size_limits() {
        glfwSetWindowSizeLimits(pWindow, GLFW_DONT_CARE, GLFW_DONT_CARE,
                                GLFW_DONT_CARE, GLFW_DONT_CARE);
    }
    void set_size_aspect_ratio(uint32_t w, uint32_t h) {
        glfwSetWindowAspectRatio(pWindow, w, h);
    }

    template <WindowCallbackEnum Type>
    [[nodiscard]]
    CallbackHandle insert_callback(WindowCallbackFunc<Type>::Type&& func);
    void erase_callback(CallbackHandle handle);
};
template <WindowCallbackEnum Type>
[[nodiscard]]
CallbackHandle WindowContext::insert_callback(
    WindowCallbackFunc<Type>::Type&& func) {
    using FuncType = WindowCallbackFunc<Type>::Type;
    CallbackHandle result;
    if (callback_free) {
        result = callback_free;
        callback_free = callback_free->next;
        result.ptr->func =
            WindowCallbackFunc<Type>{std::forward<FuncType>(func)};
    } else {
        callbacks.emplace_back(
            WindowCallbackFunc<Type>{std::forward<FuncType>(func)});
        result= &callbacks.back();
    }
    if (!callback_head[size_t(Type)]) {
        callback_set<Type>(this);
        result.ptr->next = result;
        result.ptr->rear = result;
    } else {
        CallbackHandle left{callback_head[size_t(Type)].ptr->rear};
        CallbackHandle right{callback_head[size_t(Type)]};
        left.ptr->next = result.ptr;
        result.ptr->next = right.ptr;
        right.ptr->rear = result.ptr;
        result.ptr->rear = left.ptr;
    }
    callback_head[size_t(Type)] = result;
    return result;
}
namespace _internal_windowcb {
#define _SET_CB(e, sent)                        \
    template <>                                 \
    void callback_set<e>(WindowContext * ctx) { \
        sent;                                   \
    }
_SET_CB(CBEnum::WindowPos, glfwSetWindowPosCallback(ctx->pWindow, __glfw_callback_windowpos))
_SET_CB(CBEnum::WindowSize, glfwSetWindowSizeCallback(ctx->pWindow, __glfw_callback_windowsize))
_SET_CB(CBEnum::WindowClose, glfwSetWindowCloseCallback(ctx->pWindow, __glfw_callback_windowclose))
_SET_CB(CBEnum::WindowRefresh, glfwSetWindowRefreshCallback(ctx->pWindow, __glfw_callback_windowrefresh))
_SET_CB(CBEnum::WindowFocus, glfwSetWindowFocusCallback(ctx->pWindow, __glfw_callback_windowfocus))
_SET_CB(CBEnum::WindowIconify, glfwSetWindowIconifyCallback(ctx->pWindow, __glfw_callback_windowiconify))
_SET_CB(CBEnum::WindowMaximize, glfwSetWindowMaximizeCallback(ctx->pWindow, __glfw_callback_windowmaximize))
_SET_CB(CBEnum::WindowContentScale, glfwSetWindowContentScaleCallback(ctx->pWindow, __glfw_callback_windowcontentscale))
_SET_CB(CBEnum::Keyboard, glfwSetKeyCallback(ctx->pWindow, __glfw_callback_keybord))
_SET_CB(CBEnum::CharInput, glfwSetCharCallback(ctx->pWindow, __glfw_callback_charinput))
_SET_CB(CBEnum::CharMods, glfwSetCharModsCallback(ctx->pWindow, __glfw_callback_charmods))
_SET_CB(CBEnum::MouseButton, glfwSetMouseButtonCallback(ctx->pWindow, __glfw_callback_mousebutton))
_SET_CB(CBEnum::CursorPos, glfwSetCursorPosCallback(ctx->pWindow, __glfw_callback_cursorpos))
_SET_CB(CBEnum::CursorEnter, glfwSetCursorEnterCallback(ctx->pWindow, __glfw_callback_cursorenter))
_SET_CB(CBEnum::Scroll, glfwSetScrollCallback(ctx->pWindow, __glfw_callback_scroll))
_SET_CB(CBEnum::DropFile, glfwSetDropCallback(ctx->pWindow, __glfw_callback_drop))
#undef _SET_CB
}  // namespace _internal_windowcb
struct FPSTitle {
    uint32_t delta_frame = 0u;
    std::stringstream str_builder;
    double last_time;

    void operator()(WindowContext& ctx);
};
}  // namespace BL
#endif  //!_BOUNDLESS_WINDOW_HPP_FILE_