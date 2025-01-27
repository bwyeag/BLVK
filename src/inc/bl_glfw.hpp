#ifndef _BL_CORE_GLFW_API_HPP_FILE_
#define _BL_CORE_GLFW_API_HPP_FILE_
/*
GLFW 的c++类版本
*/
#define GLFW_INCLUDE_VULKAN
namespace _glfw_internal {
#include <GLFW/glfw3.h>
}  // namespace _glfw_internal
#include <vulkan/vulkan.h>
#include <array>

namespace glfw {
using namespace _glfw_internal;
inline int initialize() {
    return glfwInit();
}
inline void terminate() {
    glfwTerminate();
}
inline void init_hint(int hint, int value) {
    glfwInitHint(hint, value);
}
inline void init_allocator(const GLFWallocator* allocator) {
    glfwInitAllocator(allocator);
}
inline void init_vulkan_loader(PFN_vkGetInstanceProcAddr loader) {
    glfwInitVulkanLoader(loader);
}
std::array<int, 3> get_version() {
    std::array<int, 3> res;
    glfwGetVersion(&res[0], &res[1], &res[2]);
    return res;
}
inline const char* get_version_string() {
    return glfwGetVersionString();
}
inline int get_error(const char** description) {
    return glfwGetError(description);
}
inline int get_platform() {
    return glfwGetPlatform();
}
inline int platform_supported(int platform) {
    return glfwPlatformSupported(platform);
}
GLFWerrorfun set_error_callback(GLFWerrorfun callback) {
    return glfwSetErrorCallback(callback);
}

GLFWmonitorfun set_monitor_callback(GLFWmonitorfun callback) {
    return glfwSetMonitorCallback(callback);
}

GLFWjoystickfun set_joystick_callback(GLFWjoystickfun callback) {
    return glfwSetJoystickCallback(callback);
}
struct Event {
    static void poll_events() { glfwPollEvents(); }
    static void wait_events() { glfwWaitEvents(); }
    static void wait_events_timeout(double timeout) {
        glfwWaitEventsTimeout(timeout);
    }
    static void post_empty_event() { glfwPostEmptyEvent(); }
};
struct Time {
    static double get_time() { return glfwGetTime(); }
    static void set_time(double time) { glfwSetTime(time); }
    static uint64_t get_timer_value() { return glfwGetTimerValue(); }
    static uint64_t get_timer_frequency() { return glfwGetTimerFrequency(); }
};
struct Window {
    GLFWwindow* handle = nullptr;
    int create(int width,
               int height,
               const char* title,
               GLFWmonitor* monitor = nullptr,
               GLFWwindow* share = nullptr) {
        handle = glfwCreateWindow(width, height, title, monitor, share);
        return handle ? GLFW_FALSE : GLFW_TRUE;
    }
    void destroy() { glfwDestroyWindow(handle); }

    /* 创建提示设置 */
    static void default_window_hints() { glfwDefaultWindowHints(); }
    static void window_hint(int hint, int value) {
        glfwWindowHint(hint, value);
    }
    static void window_hint_string(int hint, const char* value) {
        glfwWindowHintString(hint, value);
    }

    /* 基本窗口操作 */

    void iconify_window() { glfwIconifyWindow(handle); }
    void restore_window() { glfwRestoreWindow(handle); }
    void maximize_window() { glfwMaximizeWindow(handle); }
    void show_window() { glfwShowWindow(handle); }
    void hide_window() { glfwHideWindow(handle); }
    void focus_window() { glfwFocusWindow(handle); }
    void request_window_attention() { glfwRequestWindowAttention(handle); }

    /* set*和get*函数 */

    float get_window_opacity() { return glfwGetWindowOpacity(handle); }
    void set_window_opacity(float opacity) {
        glfwSetWindowOpacity(handle, opacity);
    }

    GLFWmonitor* get_window_monitor() { return glfwGetWindowMonitor(handle); }
    void set_window_monitor(GLFWmonitor* monitor,
                            int xpos,
                            int ypos,
                            int width,
                            int height,
                            int refreshRate) {
        glfwSetWindowMonitor(handle, monitor, xpos, ypos, width, height,
                             refreshRate);
    }

    int window_should_close() { return glfwWindowShouldClose(handle); }
    void set_window_should_close(int value = GLFW_TRUE) {
        glfwSetWindowShouldClose(handle, value);
    }

    const char* get_window_title() { return glfwGetWindowTitle(handle); }
    void set_window_title(const char* title) {
        glfwSetWindowTitle(handle, title);
    }

    void get_window_pos(int* xpos, int* ypos) {
        glfwGetWindowPos(handle, xpos, ypos);
    }
    void set_window_pos(int xpos, int ypos) {
        glfwSetWindowPos(handle, xpos, ypos);
    }

    void get_window_size(int* width, int* height) {
        glfwGetWindowSize(handle, width, height);
    }
    void set_window_size(int width, int height) {
        glfwSetWindowSize(handle, width, height);
    }
    void set_window_size_limits(int minwidth,
                                int minheight,
                                int maxwidth,
                                int maxheight) {
        glfwSetWindowSizeLimits(handle, minwidth, minheight, maxwidth,
                                maxheight);
    }
    void set_window_aspect_ratio(int numer = GLFW_DONT_CARE,
                                 int denom = GLFW_DONT_CARE) {
        glfwSetWindowAspectRatio(handle, numer, denom);
    }

    void get_framebuffer_size(int* width, int* height) {
        glfwGetFramebufferSize(handle, width, height);
    }
    void get_window_frame_size(int* left, int* top, int* right, int* bottom) {
        glfwGetWindowFrameSize(handle, left, top, right, bottom);
    }

    void get_window_content_scale(float* xscale, float* yscale) {
        glfwGetWindowContentScale(handle, xscale, yscale);
    }

    void* get_window_user_pointer() { return glfwGetWindowUserPointer(handle); }
    void set_window_user_pointer(void* pointer) {
        glfwSetWindowUserPointer(handle, pointer);
    }

    void get_cursor_pos(double* xpos, double* ypos) {
        glfwGetCursorPos(handle, xpos, ypos);
    }
    void set_cursor_pos(double xpos, double ypos) {
        glfwSetCursorPos(handle, xpos, ypos);
    }
    void set_cursor(GLFWcursor* cursor) { glfwSetCursor(handle, cursor); }

    void set_window_icon(int count, const GLFWimage* images) {
        glfwSetWindowIcon(handle, count, images);
    }

    void set_window_attrib(int attrib, int value) {
        glfwSetWindowAttrib(handle, attrib, value);
    }
    int get_window_attrib(int attrib) {
        return glfwGetWindowAttrib(handle, attrib);
    }

    int get_input_mode(int mode) { return glfwGetInputMode(handle, mode); }
    void set_input_mode(int mode, int value) {
        glfwSetInputMode(handle, mode, value);
    }

    void set_clipboard_string(const char* string) {
        glfwSetClipboardString(handle, string);
    }
    const char* get_clipboard_string() { const char* glfwGetClipboardString(); }

    int get_key(int key) { return glfwGetKey(handle, key); }

    int get_mouse_button(int button) {
        return glfwGetMouseButton(handle, button);
    }
    /* vulkan 相关 */
    VkResult create_window_surface(VkInstance instance,
                                   const VkAllocationCallbacks* allocator,
                                   VkSurfaceKHR* surface) {
        return glfwCreateWindowSurface(instance, handle, allocator, surface);
    }
    /* 回调相关 */
    GLFWwindowposfun set_window_pos_callback(GLFWwindowposfun callback) {
        return glfwSetWindowPosCallback(handle, callback);
    }

    GLFWwindowsizefun set_window_size_callback(GLFWwindowsizefun callback) {
        return glfwSetWindowSizeCallback(handle, callback);
    }

    GLFWwindowclosefun set_window_close_callback(GLFWwindowclosefun callback) {
        return glfwSetWindowCloseCallback(handle, callback);
    }

    GLFWwindowrefreshfun set_window_refresh_callback(
        GLFWwindowrefreshfun callback) {
        return glfwSetWindowRefreshCallback(handle, callback);
    }

    GLFWwindowfocusfun set_window_focus_callback(GLFWwindowfocusfun callback) {
        return glfwSetWindowFocusCallback(handle, callback);
    }

    GLFWwindowiconifyfun set_window_iconify_callback(
        GLFWwindowiconifyfun callback) {
        return glfwSetWindowIconifyCallback(handle, callback);
    }

    GLFWwindowmaximizefun set_window_maximize_callback(
        GLFWwindowmaximizefun callback) {
        return glfwSetWindowMaximizeCallback(handle, callback);
    }

    GLFWframebuffersizefun set_framebuffer_size_callback(
        GLFWframebuffersizefun callback) {
        return glfwSetFramebufferSizeCallback(handle, callback);
    }

    GLFWwindowcontentscalefun set_window_content_scale_callback(
        GLFWwindowcontentscalefun callback) {
        return glfwSetWindowContentScaleCallback(handle, callback);
    }

    GLFWkeyfun set_key_callback(GLFWkeyfun callback) {
        return glfwSetKeyCallback(handle, callback);
    }

    GLFWcharfun set_char_callback(GLFWcharfun callback) {
        return glfwSetCharCallback(handle, callback);
    }

    GLFWcharmodsfun set_char_mods_callback(GLFWcharmodsfun callback) {
        return glfwSetCharModsCallback(handle, callback);
    }

    GLFWmousebuttonfun set_mouse_button_callback(GLFWmousebuttonfun callback) {
        return glfwSetMouseButtonCallback(handle, callback);
    }

    GLFWcursorposfun set_cursor_pos_callback(GLFWcursorposfun callback) {
        return glfwSetCursorPosCallback(handle, callback);
    }

    GLFWcursorenterfun set_cursor_enter_callback(GLFWcursorenterfun callback) {
        return glfwSetCursorEnterCallback(handle, callback);
    }

    GLFWscrollfun set_scroll_callback(GLFWscrollfun callback) {
        return glfwSetScrollCallback(handle, callback);
    }

    GLFWdropfun set_drop_callback(GLFWdropfun callback) {
        return glfwSetDropCallback(handle, callback);
    }
};
inline GLFWmonitor** get_monitors(int* count) {
    return glfwGetMonitors(count);
}
inline GLFWmonitor* get_primary_monitors() {
    return glfwGetPrimaryMonitor();
}
struct monitor {
    GLFWmonitor* handle = nullptr;

    void get_monitor_pos(int* xpos, int* ypos) {
        glfwGetMonitorPos(handle, xpos, ypos);
    }
    void get_monitor_workarea(int* xpos, int* ypos, int* width, int* height) {
        glfwGetMonitorWorkarea(handle, xpos, ypos, width, height);
    }
    void get_monitor_physical_size(int* widthMM, int* heightMM) {
        glfwGetMonitorPhysicalSize(handle, widthMM, heightMM);
    }
    void get_monitor_content_scale(float* xscale, float* yscale) {
        glfwGetMonitorContentScale(handle, xscale, yscale);
    }
    const char* get_monitor_name() { return glfwGetMonitorName(handle); }

    void* get_monitor_user_pointer() {
        return glfwGetMonitorUserPointer(handle);
    }
    void set_monitor_user_pointer(void* pointer) {
        glfwSetMonitorUserPointer(handle, pointer);
    }

    const GLFWvidmode* get_video_modes(int* count) {
        return glfwGetVideoModes(handle, count);
    }
    const GLFWvidmode* get_video_mode() { return glfwGetVideoMode(handle); }

    void set_gamma(float gamma) { glfwSetGamma(handle, gamma); }

    const GLFWgammaramp* get_gamma_ramp() { return glfwGetGammaRamp(handle); }
    void set_gamma_ramp(const GLFWgammaramp* ramp) {
        glfwSetGammaRamp(handle, ramp);
    }
};
struct Joystick {
    int jid;

    Joystick(int jid) : jid(jid) {}

    int joystick_present() { return glfwJoystickPresent(jid); }
    const float* get_joystick_axes(int* count) {
        return glfwGetJoystickAxes(jid, count);
    }
    const unsigned char* get_joystick_buttons(int* count) {
        return glfwGetJoystickButtons(jid, count);
    }
    const unsigned char* get_joystick_hats(int* count) {
        return glfwGetJoystickHats(jid, count);
    }
    const char* get_joystick_name() { return glfwGetJoystickName(jid); }
    const char* get_joystick_guid() { return glfwGetJoystickGUID(jid); }
    void glfw_set_joystick_user_pointer(void* pointer) {
        glfwSetJoystickUserPointer(jid, pointer);
    }
    void* get_joystick_user_pointer() {
        return glfwGetJoystickUserPointer(jid);
    }
    int joystick_is_gamepad() { return glfwJoystickIsGamepad(jid); }
    const char* get_gamepad_name() { return glfwGetGamepadName(jid); }
    int get_gamepad_state(int jid, GLFWgamepadstate* state) {
        return glfwGetGamepadState(jid, state);
    }
    int update_gamepad_mappings(const char* string) {
        return glfwUpdateGamepadMappings(string);
    }
};
struct Cursor {
    GLFWcursor* cursor;
    void create(const GLFWimage* image, int xhot, int yhot) {
        cursor = glfwCreateCursor(image, xhot, yhot);
    }
    void create(int shape) { cursor = glfwCreateStandardCursor(shape); }
    void destroy() { glfwDestroyCursor(cursor); }
};
}  // namespace glfw
#endif  //!_BL_CORE_GLFW_API_HPP_FILE_