#include "bl_window.hpp"
#include "bl_output.hpp"
namespace BL {
std::string WindowErrorCategory::message(int ev) const {
    using Enum = WindowErrorEnum;
    switch (static_cast<Enum>(ev)) {
        case Enum::Success:
            return "no error";
        case Enum::NoMonitor:
            return "system no monitor";
        case Enum::InitInfoLost:
            return "WindowInit_t not enough infomation";
        case Enum::WindowCreateFailed:
            return "failed to create window";
        case Enum::GLFWInitFailed:
            return "failed to init GLFW";
        default:
            return "unknown error";
    }
}
static WindowErrorCategory category;
[[nodiscard]]
std::error_code make_error_code(WindowErrorEnum e) {
    return {static_cast<int>(e), category};
}
namespace _internal_windowcb {
#define _FN_INTERNAL(e, call)                                              \
    WindowContext* ctx = (WindowContext*)glfwGetWindowUserPointer(window); \
    CallbackHandle p = ctx->callback_head[e], end = p;                     \
    while (p) {                                                            \
        auto* fn = std::get_if<FnType<e>>(&p->func);                       \
        if (fn)                                                            \
            call;                                                          \
        p = p->next;                                                       \
        if (p == end)                                                      \
            break;                                                         \
    }
void __glfw_callback_windowpos(GLFWwindow* window, int xpos, int ypos) {
    _FN_INTERNAL(CBEnum::WindowPos, fn->value(ctx, xpos, ypos))
}
void __glfw_callback_windowsize(GLFWwindow* window, int width, int height) {
    _FN_INTERNAL(CBEnum::WindowSize, fn->value(ctx, width, height))
}
void __glfw_callback_windowclose(GLFWwindow* window) {
    _FN_INTERNAL(CBEnum::WindowClose, fn->value(ctx))
}
void __glfw_callback_windowrefresh(GLFWwindow* window) {
    _FN_INTERNAL(CBEnum::WindowRefresh, fn->value(ctx))
}
void __glfw_callback_windowfocus(GLFWwindow* window, int focused) {
    _FN_INTERNAL(CBEnum::WindowFocus, fn->value(ctx, focused))
}
void __glfw_callback_windowiconify(GLFWwindow* window, int iconified) {
    _FN_INTERNAL(CBEnum::WindowIconify, fn->value(ctx, iconified))
}
void __glfw_callback_windowmaximize(GLFWwindow* window, int maximized) {
    _FN_INTERNAL(CBEnum::WindowMaximize, fn->value(ctx, maximized))
}
void __glfw_callback_windowcontentscale(GLFWwindow* window,
                                        float xscale,
                                        float yscale) {
    _FN_INTERNAL(CBEnum::WindowContentScale, fn->value(ctx, xscale, yscale))
}
void __glfw_callback_mousebutton(GLFWwindow* window,
                                 int button,
                                 int action,
                                 int mods) {
    _FN_INTERNAL(CBEnum::MouseButton, fn->value(ctx, button, action, mods))
}
void __glfw_callback_cursorpos(GLFWwindow* window, double xpos, double ypos) {
    _FN_INTERNAL(CBEnum::CursorPos, fn->value(ctx, xpos, ypos))
}
void __glfw_callback_cursorenter(GLFWwindow* window, int entered) {
    _FN_INTERNAL(CBEnum::CursorEnter, fn->value(ctx, entered))
}
void __glfw_callback_scroll(GLFWwindow* window,
                            double xoffset,
                            double yoffset) {
    _FN_INTERNAL(CBEnum::Scroll, fn->value(ctx, xoffset, yoffset))
}
void __glfw_callback_keybord(GLFWwindow* window,
                             int key,
                             int scancode,
                             int action,
                             int mods) {
    _FN_INTERNAL(CBEnum::Keyboard, fn->value(ctx, key, scancode, action, mods))
}
void __glfw_callback_charinput(GLFWwindow* window, unsigned int codepoint) {
    _FN_INTERNAL(CBEnum::CharInput, fn->value(ctx, codepoint))
}
void __glfw_callback_charmods(GLFWwindow* window,
                              unsigned int codepoint,
                              int mods) {
    _FN_INTERNAL(CBEnum::CharMods, fn->value(ctx, codepoint, mods))
}
void __glfw_callback_drop(GLFWwindow* window,
                          int path_count,
                          const char* paths[]) {
    _FN_INTERNAL(CBEnum::DropFile, fn->value(ctx, path_count, paths))
}
#undef _FN_INTERNAL
void __glfw_error_callback(int error_code, const char* description) {
    print_error("GLFW", "Error code:", error_code, "; Desc:", description);
}
}  // namespace _internal_windowcb
void WindowContext::initialize(std::error_code& ec) {
    glfwSetErrorCallback(_internal_windowcb::__glfw_error_callback);
    if (!glfwInit()) {
        ec = make_error_code(WindowErrorEnum::GLFWInitFailed);
    }
}
void WindowContext::terminate() {
    glfwTerminate();
}
void WindowContext::create_window(const WindowInit_t& init,
                                  std::error_code& ec) noexcept {
    using State = WindowInitState;
    using Error = WindowErrorEnum;
    if (init.init_state & State::UsePrimaryMonitor) {
        pMonitor = glfwGetPrimaryMonitor();
    } else {
        int count;
        GLFWmonitor** pMonitors = glfwGetMonitors(&count);
        if (!pMonitors) {
            ec = make_error_code(Error::NoMonitor);
            return;
        }
        if (!init.monitor_choose) {
            ec = make_error_code(Error::EmptyMonitorChooseFunc);
            return;
        }
        for (int i = 0; i < count; i++) {
            if (init.monitor_choose(pMonitors[i])) {
                pMonitor = pMonitors[i];
                break;
            }
        }
    }
    if (!pMonitor) {
        ec = make_error_code(Error::NoMonitor);
        return;
    }
    title = init.init_title;
    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE,
                   static_cast<bool>(init.init_state & State::Resizable));
    glfwWindowHint(GLFW_DECORATED,
                   static_cast<bool>(init.init_state & State::Decorated));
    glfwWindowHint(
        GLFW_CENTER_CURSOR,
        static_cast<bool>(init.init_state & State::InitMouseCentered));
    glfwWindowHint(GLFW_VISIBLE,
                   !static_cast<bool>(init.init_state & State::InitUnvisiable));
    State size_state = State(init.init_state & State::SizeMask);
    const GLFWvidmode* pMode = glfwGetVideoMode(pMonitor);
    if (size_state == State::FullScreen) {
        pWindow = glfwCreateWindow(pMode->width, pMode->height, title.c_str(),
                                   pMonitor, nullptr);
    } else if (size_state == State::Maximized) {
        glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
        pWindow = glfwCreateWindow(pMode->width, pMode->height, title.c_str(),
                                   nullptr, nullptr);
    } else if (size_state == State::Specified) {
        pWindow = glfwCreateWindow(init.init_size_x, init.init_size_y,
                                   title.c_str(), nullptr, nullptr);
    } else {
        pMonitor = nullptr;
        title.clear();
        ec = make_error_code(Error::InitInfoLost);
        return;
    }
    if (!pWindow) {
        pMonitor = nullptr;
        title.clear();
        ec = make_error_code(Error::WindowCreateFailed);
        return;
    }
    if (init.init_pos_x != (~0u) && init.init_pos_y != (~0u))
        glfwSetWindowPos(pWindow, init.init_pos_x, init.init_pos_y);
    glfwSetWindowSizeLimits(pWindow, init.min_size_x, init.min_size_y,
                            init.max_size_x, init.max_size_y);
    glfwSetWindowUserPointer(pWindow, this);
}
void WindowContext::destroy() noexcept {
    if (pWindow)
        glfwDestroyWindow(pWindow);
    pWindow = nullptr;
    pMonitor = nullptr;
}
void WindowContext::update() noexcept {
    double cur_time = glfwGetTime();
    delta_time = cur_time - current_time;
    current_time = cur_time;
}
void WindowContext::erase_callback(CallbackHandle handle) {
    size_t index = handle->func.index();
    if (0 < index && index < WindowCallbackEnum::MaxCallbackEnum) {
        if (callback_head[index] == handle) {
            if (callback_head[index].ptr->next == callback_head[index].ptr) {
                callback_head[index].ptr = nullptr;
            } else {
                callback_head[index].ptr = callback_head[index].ptr->next;
            }
        }
        handle.ptr->rear->next = handle.ptr->next;
        handle.ptr->next->rear = handle.ptr->rear;
        handle.ptr->next = callback_free.ptr;
        callback_free = handle;
    }
}
void FPSTitle::operator()(WindowContext& ctx) {
    delta_frame++;
    double delta_time = ctx.current_time - last_time;
    if (delta_time >= 1.0) {
        str_builder.precision(1);
        str_builder << ctx.title << " " << std::fixed
                    << delta_frame / delta_time << " FPS";
        glfwSetWindowTitle(ctx.pWindow, str_builder.str().c_str());
        str_builder.str("");
        last_time = ctx.current_time;
        delta_time = 0;
    }
}
}  // namespace BL
