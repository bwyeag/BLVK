#include <core/bl_init.hpp>

namespace BLVK {
void __glfw_callback_windowpos(GLFWwindow* window, int xpos, int ypos) {
    auto* ptr = (WindowContext*)glfwGetWindowUserPointer(window);
    ptr->iterate<WindowCallback::glfw_windowpos_func>(ptr, xpos, ypos);
}
void __glfw_callback_windowsize(GLFWwindow* window, int width, int height) {
    auto* ptr = (WindowContext*)glfwGetWindowUserPointer(window);
    ptr->iterate<WindowCallback::glfw_windowsize_func>(ptr, width, height);
}
void __glfw_callback_windowclose(GLFWwindow* window) {
    auto* ptr = (WindowContext*)glfwGetWindowUserPointer(window);
    ptr->iterate<WindowCallback::glfw_windowclose_func>(ptr);
}
void __glfw_callback_windowrefresh(GLFWwindow* window) {
    auto* ptr = (WindowContext*)glfwGetWindowUserPointer(window);
    ptr->iterate<WindowCallback::glfw_windowrefresh_func>(ptr);
}
void __glfw_callback_windowfocus(GLFWwindow* window, int focused) {
    auto* ptr = (WindowContext*)glfwGetWindowUserPointer(window);
    ptr->iterate<WindowCallback::glfw_windowfocus_func>(ptr, focused);
}
void __glfw_callback_windowiconify(GLFWwindow* window, int iconified) {
    auto* ptr = (WindowContext*)glfwGetWindowUserPointer(window);
    ptr->iterate<WindowCallback::glfw_windowiconify_func>(ptr, iconified);
}
void __glfw_callback_windowmaximize(GLFWwindow* window, int maximized) {
    auto* ptr = (WindowContext*)glfwGetWindowUserPointer(window);
    ptr->iterate<WindowCallback::glfw_windowmaximize_func>(ptr, maximized);
}
void __glfw_callback_windowcontentscale(GLFWwindow* window,
                                        float xscale,
                                        float yscale) {
    auto* ptr = (WindowContext*)glfwGetWindowUserPointer(window);
    ptr->iterate<WindowCallback::glfw_windowcontentscale_func>(ptr, xscale,
                                                               yscale);
}
void __glfw_callback_mousebutton(GLFWwindow* window,
                                 int button,
                                 int action,
                                 int mods) {
    auto* ptr = (WindowContext*)glfwGetWindowUserPointer(window);
    ptr->iterate<WindowCallback::glfw_mousebutton_func>(ptr, button, action,
                                                        mods);
}
void __glfw_callback_cursorpos(GLFWwindow* window, double xpos, double ypos) {
    auto* ptr = (WindowContext*)glfwGetWindowUserPointer(window);
    ptr->iterate<WindowCallback::glfw_cursorpos_func>(ptr, xpos, ypos);
}
void __glfw_callback_cursorenter(GLFWwindow* window, int entered) {
    auto* ptr = (WindowContext*)glfwGetWindowUserPointer(window);
    ptr->iterate<WindowCallback::glfw_cursorenter_func>(ptr, entered);
}
void __glfw_callback_scroll(GLFWwindow* window,
                            double xoffset,
                            double yoffset) {
    auto* ptr = (WindowContext*)glfwGetWindowUserPointer(window);
    ptr->iterate<WindowCallback::glfw_scroll_func>(ptr, xoffset, yoffset);
}
void __glfw_callback_keybord(GLFWwindow* window,
                             int key,
                             int scancode,
                             int action,
                             int mods) {
    auto* ptr = (WindowContext*)glfwGetWindowUserPointer(window);
    ptr->iterate<WindowCallback::glfw_key_func>(ptr, key, scancode, action,
                                                mods);
}
void __glfw_callback_charinput(GLFWwindow* window, unsigned int codepoint) {
    auto* ptr = (WindowContext*)glfwGetWindowUserPointer(window);
    ptr->iterate<WindowCallback::glfw_char_func>(ptr, codepoint);
}
void __glfw_callback_charmods(GLFWwindow* window,
                              unsigned int codepoint,
                              int mods) {
    auto* ptr = (WindowContext*)glfwGetWindowUserPointer(window);
    ptr->iterate<WindowCallback::glfw_charmods_func>(ptr, codepoint, mods);
}
void __glfw_callback_drop(GLFWwindow* window,
                          int path_count,
                          const char* paths[]) {
    auto* ptr = (WindowContext*)glfwGetWindowUserPointer(window);
    ptr->iterate<WindowCallback::glfw_drop_func>(ptr, path_count, paths);
}
VkResult Context::acquire_vkapi_version(uint32_t& version) {
    if (vkGetInstanceProcAddr(VK_NULL_HANDLE, "vkEnumerateInstanceVersion"))
        return vkEnumerateInstanceVersion(&version);
    else
        version = VK_API_VERSION_1_0;
    return VK_SUCCESS;
}
VkResult Context::check_instance_extension(
    std::span<const char*> extensionNames,
    const char* layerName) {
    uint32_t extensionCount;
    std::vector<VkExtensionProperties> availableExtensions;
    if (VkResult result = vkEnumerateInstanceExtensionProperties(
            layerName, &extensionCount, nullptr)) {
        print_error("Context",
                    "Failed to get the count of instance "
                    "extension! Code:",
                    int32_t(result));
        return result;
    }
    if (extensionCount) {
        availableExtensions.resize(extensionCount);
        if (VkResult result = vkEnumerateInstanceExtensionProperties(
                layerName, &extensionCount, availableExtensions.data())) {
            print_error("Context",
                        "Failed to enumerate instance extension "
                        "properties! Code:",
                        int32_t(result));
            return result;
        }
        for (auto& i : extensionNames) {
            for (auto& j : availableExtensions)
                if (!strcmp(i, j.extensionName))
                    goto CHECK_FOUND;
            i = nullptr;
        CHECK_FOUND:
            continue;
        }
    } else
        for (auto& i : extensionNames)
            i = nullptr;
    return VK_SUCCESS;
}
VkResult Context::check_instance_layer(std::span<const char*> layerNames) {
    uint32_t layerCount;
    std::vector<VkLayerProperties> availableLayers;
    if (VkResult result =
            vkEnumerateInstanceLayerProperties(&layerCount, nullptr)) {
        print_error("Context",
                    "Failed to get the count of instance "
                    "layers! Code:",
                    int32_t(result));
        return result;
    }
    if (layerCount) {
        availableLayers.resize(layerCount);
        if (VkResult result = vkEnumerateInstanceLayerProperties(
                &layerCount, availableLayers.data())) {
            print_error("Context",
                        "Failed to enumerate instance layer "
                        "properties! Code:",
                        int32_t(result));
            return result;
        }
        for (auto& i : layerNames) {
            for (auto& j : availableLayers)
                if (!strcmp(i, j.layerName))
                    goto CHECK_FOUND;
            i = nullptr;
        CHECK_FOUND:
            continue;
        }
    } else
        for (auto& i : layerNames)
            i = nullptr;
    return VK_SUCCESS;
}
std::string Context::combine_debug_message(
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData) {
    auto& sstm = acquire_local_data().local_sstm;
    if (pCallbackData->queueLabelCount) {
        sstm << "Queue Labels:";
        for (uint32_t i = 0; i < pCallbackData->queueLabelCount; ++i) {
            sstm << ((pCallbackData->pQueueLabels[i].pLabelName)
                         ? pCallbackData->pQueueLabels[i].pLabelName
                         : "Null")
                 << '\t';
        }
        sstm << '\n';
    }
    if (pCallbackData->cmdBufLabelCount) {
        sstm << "CmdBuf Labels:";
        for (uint32_t i = 0; i < pCallbackData->queueLabelCount; ++i) {
            sstm << ((pCallbackData->pCmdBufLabels[i].pLabelName)
                         ? pCallbackData->pCmdBufLabels[i].pLabelName
                         : "Null")
                 << '\t';
        }
        sstm << '\n';
    }
    if (pCallbackData->objectCount) {
        sstm << "Object Labels:\n";
        for (uint32_t i = 0; i < pCallbackData->queueLabelCount; ++i) {
            auto& label = pCallbackData->pObjects[i];
            sstm << "Type:" << label.objectType << '\t';
            sstm << "Handle:" << label.objectHandle << '\t';
            sstm << "Name:"
                 << ((label.pObjectName) ? label.pObjectName : "Null") << '\n';
        }
    }
    if (pCallbackData->pMessage) {
        sstm << "Debug Message:\n" << pCallbackData->pMessage;
    }
    auto message = sstm.str();
    sstm.str("");
    return message;
}
VkResult Context::prepare_debugger() {
    static PFN_vkDebugUtilsMessengerCallbackEXT DebugUtilsMessengerCallback =
        [](VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
           VkDebugUtilsMessageTypeFlagsEXT messageTypes,
           const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
           void* pUserData) -> VkBool32 {
        switch (messageTypes) {
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
                print_log("Vulkan", "Severity:", "VERBOSE", "Message:\n",
                          combine_debug_message(pCallbackData));
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
                print_log("Vulkan", "Severity:", "INFO", "Message:\n",
                          combine_debug_message(pCallbackData));
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
                print_warning("Vulkan", "Severity:", "WARNING", "Message:\n",
                              combine_debug_message(pCallbackData));
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
                print_error("Vulkan", "Severity:", "ERROR", "Message:\n",
                            combine_debug_message(pCallbackData));
                break;
        }
        return VK_FALSE;
    };
    VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
        .messageType =
            VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT,
        .pfnUserCallback = DebugUtilsMessengerCallback};
    PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessenger =
        reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
            vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
    if (vkCreateDebugUtilsMessenger) {
        VkResult result = vkCreateDebugUtilsMessenger(
            instance, &debugUtilsMessengerCreateInfo, nullptr, &debugger);
        if (result)
            print_error("Context", "Failed to create debug messenger! code:",
                        int32_t(result));
        return result;
    }
    print_error("Context",
                "Failed to get the function pointer of "
                "vkCreateDebugUtilsMessengerEXT!");
    return VK_SUCCESS;
}
CtxResult Context::prepare_instance(InstanceCreateInfo& info) {
    uint32_t current_version = 0u;
    if (acquire_vkapi_version(current_version)) {
        print_error("Context", "acquire_vkapi_version failed!");
        return CtxResult::ACQUIRE_API_VERSION_FAILED;
    }
    if (current_version < info.min_api_version) {
        print_error("Context", "Vulkan API version too low!");
        return CtxResult::API_VERSION_TOO_LOW;
    }
    vulkanApiVersion = std::max(current_version, info.min_api_version);
    VkApplicationInfo app_info = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = info.pAppName,
        .applicationVersion = info.appVersion,
        .pEngineName = "BLVK",
        .engineVersion = VK_MAKE_API_VERSION(0, 0, 1, 0),
        .apiVersion = vulkanApiVersion,
    };

    if (VkResult result = check_instance_extension(info.extensionNames)) {
        print_error("Context", "check_instance_extension() failed! Code:",
                    int32_t(result));
        return CtxResult::CHECK_EXT_FAILED;
    }
    if (VkResult result = check_instance_layer(info.layerNames)) {
        print_error("Context",
                    "check_instance_layer() failed! Code:", int32_t(result));
        return CtxResult::CHECK_LAYER_FAILED;
    }

    VkInstanceCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = info.pNextInstance,
        .flags = info.instanceFlag,
        .pApplicationInfo = &app_info,
        .enabledLayerCount = uint32_t(info.layerNames.size()),
        .ppEnabledLayerNames = info.layerNames.data(),
        .enabledExtensionCount = uint32_t(info.extensionNames.size()),
        .ppEnabledExtensionNames = info.extensionNames.data()};

    if (VkResult result = vkCreateInstance(&createInfo, nullptr, &instance)) {
        switch (result) {
            case VK_ERROR_LAYER_NOT_PRESENT:
            case VK_ERROR_EXTENSION_NOT_PRESENT:
                print_warning(
                    "Context",
                    "Vulkan instance some ext/layer not useable! Code:",
                    int32_t(result));
                break;
            default:
                print_error("Context", "Vulkan instance create failed! Code:",
                            int32_t(result));
                return CtxResult::INSTANCE_CREATE_FAILED;
        }
    }
    print_log("Context",
              "Vulkan API Version:", VK_API_VERSION_MAJOR(vulkanApiVersion),
              VK_API_VERSION_MINOR(vulkanApiVersion),
              VK_API_VERSION_PATCH(vulkanApiVersion));
    if (info.isDebuging)
        if (VkResult result = prepare_debugger()) {
            print_error("Context",
                        "create debug failed! Code:", int32_t(result));
            return CtxResult::DEBUG_CREATE_FAILED;
        }
    return CtxResult::SUCCESS;
}
CtxResult Context::prepare_window() {
    if (!glfwInit()) {
        print_error("Context", "Failed to initialize GLFW!");
        return CtxResult::INIT_GLFW_FAILED;
    }
    pMonitors = glfwGetMonitors(&monitorCount);
    if (!pMonitors) {
        print_error("Context", "No monitors!");
        return CtxResult::NO_MONITOR;
    }
    return CtxResult::SUCCESS;
}
CtxResult Context::create_window(WindowCreateInfo& info, WindowContext*& ret) {
    glfwSetErrorCallback([](int error_code, const char* description) {
        print_error("GLFW", "Error code:", error_code, "; Desc:", description);
    });
    auto& window = windowData.emplace_back();
    if (CtxResult result = window.prepare_window(*this, info);
        result != CtxResult::SUCCESS) {
        window.cleanup(*this);
        return result;
    }
    return CtxResult::SUCCESS;
}
CtxResult WindowContext::prepare_window(Context& ctx, WindowCreateInfo& info) {
    using State = WindowCreateState;
    if (info.init_state & State::UsePrimaryMonitor) {
        pMonitor = glfwGetPrimaryMonitor();
    } else {
        if (!info.monitor_choose) {
            return CtxResult::NO_MONITOR_CHOOSE_FUNCT;
        }
        for (int i = 0; i < ctx.monitorCount; i++) {
            if (info.monitor_choose(ctx.pMonitors[i])) {
                pMonitor = ctx.pMonitors[i];
                goto FINISH_CHOOSE;
            }
        }
        return CtxResult::NO_FIT_MONITOR;
    }
FINISH_CHOOSE:
    title = info.init_title;
    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE,
                   static_cast<bool>(info.init_state & State::Resizable));
    glfwWindowHint(GLFW_DECORATED,
                   static_cast<bool>(info.init_state & State::Decorated));
    glfwWindowHint(GLFW_AUTO_ICONIFY,
                   static_cast<bool>(info.init_state & State::AutoIconify));
    glfwWindowHint(GLFW_FLOATING,
                   static_cast<bool>(info.init_state & State::WindowFloating));
    glfwWindowHint(
        GLFW_CENTER_CURSOR,
        static_cast<bool>(info.init_state & State::InitMouseCentered));
    glfwWindowHint(GLFW_VISIBLE,
                   !static_cast<bool>(info.init_state & State::InitUnvisiable));
    State size_state = State(info.init_state & State::SizeMask);
    const GLFWvidmode* pMode = glfwGetVideoMode(pMonitor);
    if (!pMode) {
        print_error("WindowContext", "Get Video Mode Failed!");
        return CtxResult::GET_VIDEO_MODE_FAILED;
    }
    if (size_state == State::FullScreen) {
        pWindow = glfwCreateWindow(pMode->width, pMode->height, title.c_str(),
                                   pMonitor, nullptr);
    } else if (size_state == State::Maximized) {
        glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
        pWindow = glfwCreateWindow(pMode->width, pMode->height, title.c_str(),
                                   nullptr, nullptr);
    } else if (size_state == State::Specified) {
        pWindow = glfwCreateWindow(info.init_size_x, info.init_size_y,
                                   title.c_str(), nullptr, nullptr);
    } else {
        pMonitor = nullptr;
        title.clear();
        return CtxResult::WRONG_ARGUMENT;
    }
    if (!pWindow) {
        pMonitor = nullptr;
        title.clear();
        return CtxResult::WINDOW_CREATE_FAILED;
    }
    if (info.init_pos_x != (~0u) && info.init_pos_y != (~0u))
        glfwSetWindowPos(pWindow, info.init_pos_x, info.init_pos_y);
    glfwSetWindowSizeLimits(pWindow, info.min_size_x, info.min_size_y,
                            info.max_size_x, info.max_size_y);
    glfwSetWindowUserPointer(pWindow, this);

    VkSurfaceKHR surface = VK_NULL_HANDLE;
    if (VkResult result = glfwCreateWindowSurface(cur_context().instance,
                                                  pWindow, nullptr, &surface)) {
        print_error("Context",
                    "Failed to create a window "
                    "surface! Code:",
                    int32_t(result));
        return CtxResult::WINDOW_SURFACE_CREATE_FAILED;
    }
    this->surface = surface;
    return CtxResult::SUCCESS;
}
VkResult Context::acquire_physical_devices(
    std::vector<VkPhysicalDevice>& availablePhysicalDevices) {
    uint32_t deviceCount;
    if (VkResult result =
            vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr)) {
        print_error("Context",
                    "Failed to get the count of physical devices! Code:",
                    int32_t(result));
        return result;
    }
    if (!deviceCount) {
        print_error("Context",
                    "Failed to find any physical device supports vulkan!\n");
        abort();
    }
    availablePhysicalDevices.resize(deviceCount);
    VkResult result = vkEnumeratePhysicalDevices(
        instance, &deviceCount, availablePhysicalDevices.data());
    if (result)
        print_error("Context", "Failed to enumerate physical devices! Code:",
                    int32_t(result));
    return result;
}
VkResult Context::get_queue_family_indices(VkPhysicalDevice physicalDevice,
                                           bool enableGraphicsQueue,
                                           bool enableComputeQueue,
                                           uint32_t (&queueFamilyIndices)[3]) {
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount,
                                             nullptr);
    if (!queueFamilyCount)
        return VK_RESULT_MAX_ENUM;
    std::vector<VkQueueFamilyProperties> queueFamilyPropertieses(
        queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount,
                                             queueFamilyPropertieses.data());
    auto& [ig, ip, ic] = queueFamilyIndices;
    ig = ip = ic = VK_QUEUE_FAMILY_IGNORED;
    for (uint32_t i = 0; i < queueFamilyCount; i++) {
        // 这三个VkBool32变量指示是否可获取（指应该被获取且能获取）相应队列族索引
        VkBool32
            // 只在enableGraphicsQueue为true时获取支持图形操作的队列族的索引
            supportGraphics =
                enableGraphicsQueue &&
                queueFamilyPropertieses[i].queueFlags & VK_QUEUE_GRAPHICS_BIT,
            supportPresentation = false,
            // 只在enableComputeQueue为true时获取支持计算的队列族的索引
            supportCompute =
                enableComputeQueue &&
                queueFamilyPropertieses[i].queueFlags & VK_QUEUE_COMPUTE_BIT;
        // 只在创建了window surface时获取支持呈现的队列族的索引
        if (windowData.size() > 0) {
            for (auto& window : windowData)
                if (VkResult result = vkGetPhysicalDeviceSurfaceSupportKHR(
                        physicalDevice, i, window.surface,
                        &supportPresentation)) {
                    print_error("Context",
                                "Failed to determine if the "
                                "queue "
                                "family supports presentation! Code:",
                                int32_t(result));
                    return result;
                }
        }
        // 若某队列族同时支持图形操作和计算
        if (supportGraphics && supportCompute) {
            // 若需要呈现，最好是三个队列族索引全部相同
            if (supportPresentation) {
                ig = ip = ic = i;
                break;
            }
            // 除非ig和ic都已取得且相同，否则将它们的值覆写为i，以确保两个队列族索引相同
            if (ig != ic || ig == VK_QUEUE_FAMILY_IGNORED)
                ig = ic = i;
            // 如果不需要呈现，那么已经可以break了
            if (windowData.size() == 0)
                break;
        }
        // 若任何一个队列族索引可以被取得但尚未被取得，将其值覆写为i
        if (supportGraphics && ig == VK_QUEUE_FAMILY_IGNORED)
            ig = i;
        if (supportPresentation && ip == VK_QUEUE_FAMILY_IGNORED)
            ip = i;
        if (supportCompute && ic == VK_QUEUE_FAMILY_IGNORED)
            ic = i;
    }
    if (ig == VK_QUEUE_FAMILY_IGNORED && enableGraphicsQueue ||
        ip == VK_QUEUE_FAMILY_IGNORED && windowData.size() > 0 ||
        ic == VK_QUEUE_FAMILY_IGNORED && enableComputeQueue)
        return VK_RESULT_MAX_ENUM;
    queueFamilyIndex_graphics = ig;
    queueFamilyIndex_presentation = ip;
    queueFamilyIndex_compute = ic;
    return VK_SUCCESS;
}
VkResult Context::determine_physical_device(
    std::vector<VkPhysicalDevice>& availablePhysicalDevices,
    uint32_t deviceIndex = 0,
    bool enableGraphicsQueue,
    bool enableComputeQueue) {
    // 定义一个特殊值用于标记一个队列族索引已被找过但未找到
    static constexpr uint32_t notFound =
        INT32_MAX;  //== VK_QUEUE_FAMILY_IGNORED & INT32_MAX
    // 定义队列族索引组合的结构体
    struct QueueFamilyIndex_combination {
        uint32_t graphics = VK_QUEUE_FAMILY_IGNORED;
        uint32_t presentation = VK_QUEUE_FAMILY_IGNORED;
        uint32_t compute = VK_QUEUE_FAMILY_IGNORED;
    };
    // QueueFamilyIndex_combination用于为各个物理设备保存一份队列族索引组合
    static std::vector<QueueFamilyIndex_combination>
        queueFamilyIndexCombinations(availablePhysicalDevices.size());
    auto& [ig, ip, ic] = queueFamilyIndexCombinations[deviceIndex];

    // 如果有任何队列族索引已被找过但未找到，返回VK_RESULT_MAX_ENUM
    if (ig == notFound && enableGraphicsQueue ||
        ip == notFound && windowData.size() > 0 ||
        ic == notFound && enableComputeQueue)
        return VK_RESULT_MAX_ENUM;

    // 如果有任何队列族索引应被获取但还未被找过
    if (ig == VK_QUEUE_FAMILY_IGNORED && enableGraphicsQueue ||
        ip == VK_QUEUE_FAMILY_IGNORED && windowData.size() > 0 ||
        ic == VK_QUEUE_FAMILY_IGNORED && enableComputeQueue) {
        uint32_t indices[3];
        VkResult result = get_queue_family_indices(
            availablePhysicalDevices[deviceIndex], enableGraphicsQueue,
            enableComputeQueue, indices);
        // 若GetQueueFamilyIndices(...)返回VK_SUCCESS或VK_RESULT_MAX_ENUM（vkGetPhysicalDeviceSurfaceSupportKHR(...)执行成功但没找齐所需队列族），
        // 说明对所需队列族索引已有结论，保存结果到queueFamilyIndexCombinations[deviceIndex]中相应变量
        // 应被获取的索引若仍为VK_QUEUE_FAMILY_IGNORED，说明未找到相应队列族，VK_QUEUE_FAMILY_IGNORED（~0u）与INT32_MAX做位与得到的数值等于notFound
        if (result == VK_SUCCESS || result == VK_RESULT_MAX_ENUM) {
            if (enableGraphicsQueue)
                ig = indices[0] & INT32_MAX;
            if (windowData.size() > 0)
                ip = indices[1] & INT32_MAX;
            if (enableComputeQueue)
                ic = indices[2] & INT32_MAX;
        }
        // 如果GetQueueFamilyIndices(...)执行失败，return
        if (result)
            return result;
    }
    // 若以上两个if分支皆不执行，则说明所需的队列族索引皆已被获取，从queueFamilyIndexCombinations[deviceIndex]中取得索引
    else {
        queueFamilyIndex_graphics =
            enableGraphicsQueue ? ig : VK_QUEUE_FAMILY_IGNORED;
        queueFamilyIndex_presentation =
            windowData.size() > 0 ? ip : VK_QUEUE_FAMILY_IGNORED;
        queueFamilyIndex_compute =
            enableComputeQueue ? ic : VK_QUEUE_FAMILY_IGNORED;
    }
    phyDevice = availablePhysicalDevices[deviceIndex];
    return VK_SUCCESS;
}
CtxResult Context::prepare_physical_device() {
    std::vector<VkPhysicalDevice> availablePhysicalDevices;
    if (acquire_physical_devices(availablePhysicalDevices))
        return CtxResult::ACQUIRE_PHYSICAL_DEVICES_FAILED;
    for (uint32_t i = 0;i<availablePhysicalDevices.size();++i)
        if (!determine_physical_device(availablePhysicalDevices,i,true,true))
            goto FIND_SUCCESS;
    print_error("Context","Can not find any phyDevice useable!");
    return ;
FIND_SUCCESS:
}
CtxResult Context::prepare_device() {}
void Context::update() {}
void WindowContext::cleanup(Context& ctx) {
    if (pWindow)
        glfwDestroyWindow(pWindow);
    pWindow = nullptr;
    pMonitor = nullptr;
    title.clear();
}
void Context::cleanup() {
    for (auto& window : windowData)
        window.cleanup(*this);
    glfwTerminate();
}
CtxResult WindowContext::prepare_swapchain(Context& ctx) {}
}  // namespace BLVK