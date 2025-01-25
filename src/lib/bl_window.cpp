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
void __glfw_callback_windowpos(GLFWwindow* window, int xpos, int ypos) {
    WindowContext* ctx = (WindowContext*)glfwGetWindowUserPointer(window);
    ctx->iterate_callback<CBEnum::WindowPos>(xpos, ypos);
}
void __glfw_callback_windowsize(GLFWwindow* window, int width, int height) {
    WindowContext* ctx = (WindowContext*)glfwGetWindowUserPointer(window);
    ctx->iterate_callback<CBEnum::WindowSize>(width, height);
}
void __glfw_callback_windowclose(GLFWwindow* window) {
    WindowContext* ctx = (WindowContext*)glfwGetWindowUserPointer(window);
    ctx->iterate_callback<CBEnum::WindowClose>();
}
void __glfw_callback_windowrefresh(GLFWwindow* window) {
    WindowContext* ctx = (WindowContext*)glfwGetWindowUserPointer(window);
    ctx->iterate_callback<CBEnum::WindowRefresh>();
}
void __glfw_callback_windowfocus(GLFWwindow* window, int focused) {
    WindowContext* ctx = (WindowContext*)glfwGetWindowUserPointer(window);
    ctx->iterate_callback<CBEnum::WindowFocus>(focused);
}
void __glfw_callback_windowiconify(GLFWwindow* window, int iconified) {
    WindowContext* ctx = (WindowContext*)glfwGetWindowUserPointer(window);
    ctx->iterate_callback<CBEnum::WindowIconify>(iconified);
}
void __glfw_callback_windowmaximize(GLFWwindow* window, int maximized) {
    WindowContext* ctx = (WindowContext*)glfwGetWindowUserPointer(window);
    ctx->iterate_callback<CBEnum::WindowMaximize>(maximized);
}
void __glfw_callback_windowcontentscale(GLFWwindow* window,
                                        float xscale,
                                        float yscale) {
    WindowContext* ctx = (WindowContext*)glfwGetWindowUserPointer(window);
    ctx->iterate_callback<CBEnum::WindowContentScale>(xscale, yscale);
}
void __glfw_callback_mousebutton(GLFWwindow* window,
                                 int button,
                                 int action,
                                 int mods) {
    WindowContext* ctx = (WindowContext*)glfwGetWindowUserPointer(window);
    ctx->iterate_callback<CBEnum::MouseButton>(button, action, mods);
}
void __glfw_callback_cursorpos(GLFWwindow* window, double xpos, double ypos) {
    WindowContext* ctx = (WindowContext*)glfwGetWindowUserPointer(window);
    ctx->iterate_callback<CBEnum::CursorPos>(xpos, ypos);
}
void __glfw_callback_cursorenter(GLFWwindow* window, int entered) {
    WindowContext* ctx = (WindowContext*)glfwGetWindowUserPointer(window);
    ctx->iterate_callback<CBEnum::CursorEnter>(entered);
}
void __glfw_callback_scroll(GLFWwindow* window,
                            double xoffset,
                            double yoffset) {
    WindowContext* ctx = (WindowContext*)glfwGetWindowUserPointer(window);
    ctx->iterate_callback<CBEnum::Scroll>(xoffset, yoffset);
}
void __glfw_callback_keybord(GLFWwindow* window,
                             int key,
                             int scancode,
                             int action,
                             int mods) {
    WindowContext* ctx = (WindowContext*)glfwGetWindowUserPointer(window);
    ctx->iterate_callback<CBEnum::Keyboard>(key, scancode, action, mods);
}
void __glfw_callback_charinput(GLFWwindow* window, unsigned int codepoint) {
    WindowContext* ctx = (WindowContext*)glfwGetWindowUserPointer(window);
    ctx->iterate_callback<CBEnum::CharInput>(codepoint);
}
void __glfw_callback_charmods(GLFWwindow* window,
                              unsigned int codepoint,
                              int mods) {
    WindowContext* ctx = (WindowContext*)glfwGetWindowUserPointer(window);
    ctx->iterate_callback<CBEnum::CharMods>(codepoint, mods);
}
void __glfw_callback_drop(GLFWwindow* window,
                          int path_count,
                          const char* paths[]) {
    WindowContext* ctx = (WindowContext*)glfwGetWindowUserPointer(window);
    ctx->iterate_callback<CBEnum::DropFile>(path_count, paths);
}
void __glfw_error_callback(int error_code, const char* description) {
    print_error("GLFW", "Error code:", error_code, "; Desc:", description);
}
}  // namespace _internal_windowcb
std::error_code WindowContext::initialize() noexcept {
    glfwSetErrorCallback(_internal_windowcb::__glfw_error_callback);
    if (!glfwInit()) {
        return make_error_code(WindowErrorEnum::GLFWInitFailed);
    } else {
        return make_error_code(WindowErrorEnum::Success);
    }
}
void WindowContext::terminate() {
    glfwTerminate();
}
std::error_code WindowContext::create_window(const WindowInit_t& init) noexcept {
    using State = WindowInitState;
    using Error = WindowErrorEnum;
    if (init.init_state & State::UsePrimaryMonitor) {
        pMonitor = glfwGetPrimaryMonitor();
    } else {
        int count;
        GLFWmonitor** pMonitors = glfwGetMonitors(&count);
        if (!pMonitors) {
            return make_error_code(Error::NoMonitor);
        }
        if (!init.monitor_choose) {
            return make_error_code(Error::EmptyMonitorChooseFunc);
        }
        for (int i = 0; i < count; i++) {
            if (init.monitor_choose(pMonitors[i])) {
                pMonitor = pMonitors[i];
                break;
            }
        }
    }
    if (!pMonitor) {
        return make_error_code(Error::NoMonitor);
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
        return make_error_code(Error::InitInfoLost);
    }
    if (!pWindow) {
        pMonitor = nullptr;
        title.clear();
        return make_error_code(Error::WindowCreateFailed);
    }
    if (init.init_pos_x != (~0u) && init.init_pos_y != (~0u))
        glfwSetWindowPos(pWindow, init.init_pos_x, init.init_pos_y);
    glfwSetWindowSizeLimits(pWindow, init.min_size_x, init.min_size_y,
                            init.max_size_x, init.max_size_y);
    glfwSetWindowUserPointer(pWindow, this);
    return make_error_code(Error::Success);
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
VkResult WindowContext::_createSwapChain_Internal() {
    auto& context = cur_context();
    auto& createInfo = swapchainCreateInfo;
    if (VkResult result = vkCreateSwapchainKHR(context.device, &createInfo,
                                               nullptr, &swapchain)) {
        print_error("createSwapchain",
                    "Failed to create a swapchain! Error "
                    "code:",
                    int32_t(result));
        return result;
    }
    uint32_t swapchainImageCount;
    if (VkResult result = vkGetSwapchainImagesKHR(
            context.device, swapchain, &swapchainImageCount, nullptr)) {
        print_error("createSwapchain",
                    "Failed to get the count of swapchain images! Error code:",
                    int32_t(result));
        return result;
    }
    swapchainImages.resize(swapchainImageCount);
    if (VkResult result = vkGetSwapchainImagesKHR(context.device, swapchain,
                                                  &swapchainImageCount,
                                                  swapchainImages.data())) {
        print_error(
            "createSwapchain",
            "Failed to get swapchain images! Error code:", int32_t(result));
        return result;
    }

    swapchainImageViews.resize(swapchainImageCount);
    VkImageViewCreateInfo imageViewCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = createInfo.imageFormat,
        //.components = {},//四个成员皆为VK_COMPONENT_SWIZZLE_IDENTITY
        .subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}};
    for (size_t i = 0; i < swapchainImageCount; i++) {
        imageViewCreateInfo.image = swapchainImages[i];
        if (VkResult result =
                vkCreateImageView(context.device, &imageViewCreateInfo, nullptr,
                                  &swapchainImageViews[i])) {
            print_error("createSwapchain",
                        "Failed to create a swapchain image view! Error code:",
                        int32_t(result));
            return result;
        }
    }
    return VK_SUCCESS;
}
VkResult WindowContext::createSwapchain(
    const WindowContextSwapchainInit_t* pInit) {
    auto& context = cur_context();
    VkSurfaceCapabilitiesKHR surface_capabilities;
    if (VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
            context.phyDevice, surface, &surface_capabilities)) {
        print_error(
            "createSwapchain",
            "Failed to get physical device surface capabilities! Error code:",
            int32_t(result));
        return result;
    }
    auto& createInfo = swapchainCreateInfo;
    createInfo.minImageCount = surface_capabilities.minImageCount +
                               (surface_capabilities.maxImageCount >
                                surface_capabilities.minImageCount);
    uint32_t width, height;
    glfwGetWindowSize(pWindow, (int*)&width, (int*)&height);
    createInfo.imageExtent =
        surface_capabilities.currentExtent.width == (~0u)
            ? VkExtent2D{std::clamp(width,
                                    surface_capabilities.minImageExtent.width,
                                    surface_capabilities.maxImageExtent.width),
                         std::clamp(height,
                                    surface_capabilities.minImageExtent.height,
                                    surface_capabilities.maxImageExtent.height)}
            : surface_capabilities.currentExtent;
    createInfo.imageArrayLayers = 1;
    createInfo.preTransform = surface_capabilities.currentTransform;
    if (surface_capabilities.supportedCompositeAlpha &
        VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR)
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;
    else
        for (size_t i = 0; i < 4; i++)
            if (surface_capabilities.supportedCompositeAlpha & 1 << i) {
                createInfo.compositeAlpha = VkCompositeAlphaFlagBitsKHR(
                    surface_capabilities.supportedCompositeAlpha & 1 << i);
                break;
            }
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    if (surface_capabilities.supportedUsageFlags &
        VK_IMAGE_USAGE_TRANSFER_SRC_BIT)
        createInfo.imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    if (surface_capabilities.supportedUsageFlags &
        VK_IMAGE_USAGE_TRANSFER_DST_BIT)
        createInfo.imageUsage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    else
        print_warning("WindowContext",
                      "VK_IMAGE_USAGE_TRANSFER_DST_BIT isn't supported!");
    // 指定图像格式
    static std::vector<VkSurfaceFormatKHR> availableFormats;
    if (availableFormats.empty())
        if (VkResult result = _getSurfaceFormats(availableFormats))
            return result;
    if (!createInfo.imageFormat)
        if (_setSurfaceFormat(
                {VK_FORMAT_R8G8B8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},
                availableFormats) &&
            _setSurfaceFormat(
                {VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},
                availableFormats)) {
            // 如果找不到上述图像格式和色彩空间的组合，那只能有什么用什么，采用availableSurfaceFormats中的第一组
            createInfo.imageFormat = availableFormats[0].format;
            createInfo.imageColorSpace = availableFormats[0].colorSpace;
            print_warning(
                "createSwapchain",
                "Failed to select a four-component UNORM surface format!\n");
        }
    // 指定呈现模式
    uint32_t surfacePresentModeCount;
    if (VkResult result = vkGetPhysicalDeviceSurfacePresentModesKHR(
            context.phyDevice, surface, &surfacePresentModeCount, nullptr)) {
        print_error(
            "createSwapchain",
            "Failed to get the count of surface present modes! Error code:",
            int32_t(result));
        return result;
    }
    if (!surfacePresentModeCount) {
        print_error("createSwapchain",
                    "Failed to find any surface present mode!");
        abort();
    }
    std::vector<VkPresentModeKHR> surfacePresentModes(surfacePresentModeCount);
    if (VkResult result = vkGetPhysicalDeviceSurfacePresentModesKHR(
            context.phyDevice, surface, &surfacePresentModeCount,
            surfacePresentModes.data())) {
        print_error("createSwapchain",
                    "Failed to get surface present "
                    "modes! Error code:",
                    int32_t(result));
        return result;
    }
    createInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
    if (!pInit->isFrameRateLimited)
        for (size_t i = 0; i < surfacePresentModeCount; i++)
            if (surfacePresentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
                createInfo.presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
                break;
            }
    const char* mode_str;
    switch (createInfo.presentMode) {
        case VK_PRESENT_MODE_FIFO_KHR:
            mode_str = "VK_PRESENT_MODE_FIFO_KHR";
            break;
        case VK_PRESENT_MODE_FIFO_RELAXED_KHR:
            mode_str = "VK_PRESENT_MODE_FIFO_RELAXED_KHR";
            break;
        case VK_PRESENT_MODE_IMMEDIATE_KHR:
            mode_str = "VK_PRESENT_MODE_IMMEDIATE_KHR";
            break;
        case VK_PRESENT_MODE_MAILBOX_KHR:
            mode_str = "VK_PRESENT_MODE_MAILBOX_KHR";
            break;
        case VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR:
            mode_str = "VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR";
            break;
        case VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR:
            mode_str = "VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR";
            break;
        default:
            mode_str = "Unknown!";
            break;
    }
    print_log("Present Mode", mode_str);
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.flags = pInit->flags;
    createInfo.surface = surface;
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;
    createInfo.pNext = nullptr;
    if (VkResult result = _createSwapChain_Internal())
        return result;
    iterate_callback<WindowCallbackEnum::SwapchainCreate>();
    return VK_SUCCESS;
}
VkResult WindowContext::recreateSwapchain() {
    auto& info = cur_context();
    auto& createInfo = swapchainCreateInfo;
    VkSurfaceCapabilitiesKHR surface_capabilities = {};
    VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        info.phyDevice, surface, &surface_capabilities);
    if (result != VK_SUCCESS) {
        print_error(
            "WindowContext",
            "Failed to get physical device surface capabilities! Error code:",
            int32_t(result));
        return result;
    }
    if (surface_capabilities.currentExtent.width == 0 ||
        surface_capabilities.currentExtent.height == 0)
        return VK_SUBOPTIMAL_KHR;
    createInfo.imageExtent = surface_capabilities.currentExtent;
    createInfo.oldSwapchain = swapchain;
    result = vkQueueWaitIdle(info.queue_graphics);
    // 仅在等待图形队列成功，且图形与呈现所用队列不同时等待呈现队列
    if (!result && info.queue_graphics != info.queue_presentation)
        result = vkQueueWaitIdle(info.queue_presentation);
    if (result) {
        print_error("WindowContext",
                    "Failed to wait for the queue to be idle! Error code:",
                    int32_t(result));
        return result;
    }
    iterate_callback<WindowCallbackEnum::SwapchainDestroy>();
    for (auto& i : swapchainImageViews)
        if (i)
            vkDestroyImageView(info.device, i, nullptr);
    swapchainImageViews.resize(0);
    result = _createSwapChain_Internal();
    if (result != VK_SUCCESS) {
        print_error("WindowContext",
                    "Create swapchain failed! Code:", int32_t(result));
        return result;
    }
    iterate_callback<WindowCallbackEnum::SwapchainCreate>();
    print_log("WindowContext", "Swapchain recreated!");
    return VK_SUCCESS;
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
