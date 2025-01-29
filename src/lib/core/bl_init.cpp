#include <core/bl_init.hpp>

#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>

namespace BL {
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
                    string_VkResult(result));
        return result;
    }
    if (extensionCount) {
        availableExtensions.resize(extensionCount);
        if (VkResult result = vkEnumerateInstanceExtensionProperties(
                layerName, &extensionCount, availableExtensions.data())) {
            print_error("Context",
                        "Failed to enumerate instance extension "
                        "properties! Code:",
                        string_VkResult(result));
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
                    string_VkResult(result));
        return result;
    }
    if (layerCount) {
        availableLayers.resize(layerCount);
        if (VkResult result = vkEnumerateInstanceLayerProperties(
                &layerCount, availableLayers.data())) {
            print_error("Context",
                        "Failed to enumerate instance layer "
                        "properties! Code:",
                        string_VkResult(result));
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
            sstm << "Type:" << string_VkObjectType(label.objectType) << '\t';
            sstm << "Handle:" << std::hex << label.objectHandle << std::dec
                 << '\t';
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
            print_error("Context", "Failed to create debug messenger! Code:",
                        string_VkResult(result));
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
                    string_VkResult(result));
        return CtxResult::CHECK_EXT_FAILED;
    }
    if (VkResult result = check_instance_layer(info.layerNames)) {
        print_error("Context", "check_instance_layer() failed! Code:",
                    string_VkResult(result));
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
                    string_VkResult(result));
                break;
            default:
                print_error("Context", "Vulkan instance create failed! Code:",
                            string_VkResult(result));
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
                        "create debug failed! Code:", string_VkResult(result));
            return CtxResult::DEBUG_CREATE_FAILED;
        }
    return CtxResult::SUCCESS;
}
CtxResult Context::prepare_glfw() {
    if (!glfwInit()) {
        print_error("Context", "Failed to initialize GLFW!");
        return CtxResult::INIT_GLFW_FAILED;
    }
    glfwSetErrorCallback([](int error_code, const char* description) {
        print_error("GLFW", "Error Code:", error_code, "; Desc:", description);
    });
    return CtxResult::SUCCESS;
}
CtxResult Context::create_window(const WindowCreateInfo& info,
                                 WindowContext*& ret) {
    auto& window = windowData.emplace_back();
    if (CtxResult result = window.prepare_window(info, *this);
        result != CtxResult::SUCCESS) {
        window.cleanup(*this);
        return result;
    }
    return CtxResult::SUCCESS;
}
CtxResult WindowContext::prepare_window(const WindowCreateInfo& info,
                                        Context& ctx) {
    using State = WindowCreateState;
    if (info.init_state & State::UsePrimaryMonitor) {
        pMonitor = glfwGetPrimaryMonitor();
        if (!pMonitor)
            return CtxResult::NO_MONITOR;
    } else {
        int monitorCount;
        GLFWmonitor** pMonitors = glfwGetMonitors(&monitorCount);
        if (!pMonitors) {
            print_error("Context", "No monitors!");
            return CtxResult::NO_MONITOR;
        }
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

    return CtxResult::SUCCESS;
}
VkResult Context::acquire_physical_devices(
    std::vector<VkPhysicalDevice>& availablePhysicalDevices) {
    uint32_t deviceCount;
    if (VkResult result =
            vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr)) {
        print_error("Context",
                    "Failed to get the count of physical devices! Code:",
                    string_VkResult(result));
        return result;
    }
    if (!deviceCount) {
        print_error("Context",
                    "Failed to find any physical device supports vulkan!");
        abort();
    }
    availablePhysicalDevices.resize(deviceCount);
    VkResult result = vkEnumeratePhysicalDevices(
        instance, &deviceCount, availablePhysicalDevices.data());
    if (result)
        print_error("Context", "Failed to enumerate physical devices! Code:",
                    string_VkResult(result));
    return result;
}
VkResult Context::acquire_queue_family_indices(
    VkPhysicalDevice physicalDevice,
    uint32_t (&queueFamilyIndices)[3],
    bool enableGraphicsQueue,
    bool enableComputeQueue) {
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
                                string_VkResult(result));
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
        VkResult result = acquire_queue_family_indices(
            availablePhysicalDevices[deviceIndex], indices, enableGraphicsQueue,
            enableComputeQueue);
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
void Context::acquire_physical_divice_properties() {
    //   设备属性:
    if (vulkanApiVersion >= VK_API_VERSION_1_1) {
        phyDeviceProperties = {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2};
        phyDeviceVulkan11Properties = {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_PROPERTIES};
        phyDeviceVulkan12Properties = {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_PROPERTIES};
        phyDeviceVulkan13Properties = {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_PROPERTIES};
        if (vulkanApiVersion >= VK_API_VERSION_1_2) {
            phyDeviceProperties.pNext = &phyDeviceVulkan11Properties;
            phyDeviceVulkan11Properties.pNext = &phyDeviceVulkan12Properties;
            if (vulkanApiVersion >= VK_API_VERSION_1_3) {
                phyDeviceVulkan12Properties.pNext =
                    &phyDeviceVulkan13Properties;
            }
        }
        vkGetPhysicalDeviceProperties2(phyDevice, &phyDeviceProperties);
        phyDeviceMemoryProperties = {
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2};
        vkGetPhysicalDeviceMemoryProperties2(phyDevice,
                                             &phyDeviceMemoryProperties);
    } else {
        vkGetPhysicalDeviceProperties(phyDevice,
                                      &phyDeviceProperties.properties);
        vkGetPhysicalDeviceMemoryProperties(
            phyDevice, &phyDeviceMemoryProperties.memoryProperties);
    }
}
void Context::acquire_physical_divice_features() {
    //   设备特性:
    if (vulkanApiVersion >= VK_API_VERSION_1_1) {
        phyDeviceFeatures = {.sType =
                                 VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2};
        phyDeviceVulkan11Features = {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES};
        phyDeviceVulkan12Features = {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES};
        phyDeviceVulkan13Features = {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES};
        if (vulkanApiVersion >= VK_API_VERSION_1_2) {
            phyDeviceFeatures.pNext = &phyDeviceVulkan11Features;
            phyDeviceVulkan11Features.pNext = &phyDeviceVulkan12Features;
            if (vulkanApiVersion >= VK_API_VERSION_1_3)
                phyDeviceVulkan12Features.pNext = &phyDeviceVulkan13Features;
        }
        vkGetPhysicalDeviceFeatures2(phyDevice, &phyDeviceFeatures);
    } else
        vkGetPhysicalDeviceFeatures(phyDevice, &phyDeviceFeatures.features);
}
CtxResult Context::prepare_physical_device() {
    std::vector<VkPhysicalDevice> availablePhysicalDevices;
    if (acquire_physical_devices(availablePhysicalDevices))
        return CtxResult::ACQUIRE_PHYSICAL_DEVICES_FAILED;
    for (uint32_t i = 0; i < availablePhysicalDevices.size(); ++i)
        if (!determine_physical_device(availablePhysicalDevices, i, true, true))
            goto FIND_SUCCESS;
    print_error("Context", "Can not find any phyDevice useable!");
    return CtxResult::NO_FIT_PHYDEVICE;
FIND_SUCCESS:
    acquire_physical_divice_properties();
    acquire_physical_divice_features();
    return CtxResult::SUCCESS;
}
VkResult Context::acquire_device_extensions(
    std::vector<VkExtensionProperties>& extensionNames,
    const char* layerName) {
    uint32_t extCount;
    if (VkResult result = vkEnumerateDeviceExtensionProperties(
            phyDevice, layerName, &extCount, nullptr)) {
        print_error("Context",
                    "vkEnumerateDeviceExtensionProperties() failed! Code:",
                    string_VkResult(result));
        return VK_RESULT_MAX_ENUM;
    }
    extensionNames.resize(extCount);
    if (VkResult result = vkEnumerateDeviceExtensionProperties(
            phyDevice, layerName, &extCount, extensionNames.data())) {
        print_error("Context",
                    "vkEnumerateDeviceExtensionProperties() failed! Code:",
                    string_VkResult(result));
        return VK_RESULT_MAX_ENUM;
    }
    std::vector<const char*> extensions(availableExtensions.size());
    std::transform(availableExtensions.begin(), availableExtensions.end(),
                   extensions.begin(), [](VkExtensionProperties& ext) {
                       return ext.extensionName;
                   });
    std::sort(
        extensions.begin(), extensions.end(),
        [](const char* a, const char* b) { return std::strcmp(a, b) < 0; });
    return VK_SUCCESS;
}
static constexpr uint32_t vma_flags_count = 8;
static constexpr std::pair<const char*, VmaAllocatorCreateFlagBits> vma_flags[]{
    {VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME,
     VMA_ALLOCATOR_CREATE_KHR_DEDICATED_ALLOCATION_BIT},
    {VK_KHR_BIND_MEMORY_2_EXTENSION_NAME,
     VMA_ALLOCATOR_CREATE_KHR_BIND_MEMORY2_BIT},
    {VK_KHR_MAINTENANCE_4_EXTENSION_NAME,
     VMA_ALLOCATOR_CREATE_KHR_MAINTENANCE4_BIT},
    {VK_KHR_MAINTENANCE_5_EXTENSION_NAME,
     VMA_ALLOCATOR_CREATE_KHR_MAINTENANCE5_BIT},
    {VK_EXT_MEMORY_BUDGET_EXTENSION_NAME,
     VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT},
    {VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
     VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT},
    {VK_EXT_MEMORY_PRIORITY_EXTENSION_NAME,
     VMA_ALLOCATOR_CREATE_EXT_MEMORY_PRIORITY_BIT},
    {VK_AMD_DEVICE_COHERENT_MEMORY_EXTENSION_NAME,
     VMA_ALLOCATOR_CREATE_AMD_DEVICE_COHERENT_MEMORY_BIT}
    // {VK_KHR_EXTERNAL_MEMORY_WIN32_EXTENSION_NAME,
    //  VMA_ALLOCATOR_CREATE_KHR_EXTERNAL_MEMORY_WIN32_BIT}
};
VmaAllocatorCreateFlagBits Context::check_VMA_extensions(
    std::vector<const char*>& extensionNames) {
    auto ret = static_cast<VmaAllocatorCreateFlagBits>(0);
    for (uint32_t i = 0; i < vma_flags_count; ++i)
        if (!std::binary_search(extensions.begin(), extensions.end(),
                                vma_flags[i].first,
                                [](const char* a, const char* b) {
                                    return std::strcmp(a, b) < 0;
                                })) {
            extensionNames.push_back(vma_flags[i].first);
            ret = static_cast<VmaAllocatorCreateFlagBits>(ret |
                                                          vma_flags[i].second);
        }
    return ret;
}
VkResult Context::check_device_extension(std::span<const char*> extensionNames,
                                         const char* layerName) {
    for (auto& i : extensionNames) {
        if (!std::binary_search(extensions.begin(), extensions.end(), i,
                                [](const char* a, const char* b) {
                                    return std::strcmp(a, b) < 0;
                                }))
            i = nullptr;
    }
}
VkResult Context::prepare_VMA(DeviceCreateInfo& info) {
    VmaAllocatorCreateInfo allocatorCreateInfo = {
        .flags = info.vmaFlags,
        .physicalDevice = phyDevice,
        .device = device,
        .instance = instance,
        .vulkanApiVersion = vulkanApiVersion};
    return vmaCreateAllocator(&allocatorCreateInfo, &allocator);
}
CtxResult Context::prepare_device(DeviceCreateInfo& info) {
    // 1.构建队列创建表
    float queuePriority = 1.0f;
    VkDeviceQueueCreateInfo queue_create_infos[3] = {
        {.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
         .queueCount = 1,
         .pQueuePriorities = &queuePriority},
        {.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
         .queueCount = 1,
         .pQueuePriorities = &queuePriority},
        {.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
         .queueCount = 1,
         .pQueuePriorities = &queuePriority}};
    uint32_t queue_create_info_count = 0;
    uint32_t& queue_index_graphics = queueFamilyIndex_graphics;
    uint32_t& queue_index_compute = queueFamilyIndex_compute;
    uint32_t& queue_index_present = queueFamilyIndex_presentation;
    if (queue_index_graphics != VK_QUEUE_FAMILY_IGNORED)
        queue_create_infos[queue_create_info_count++].queueFamilyIndex =
            queue_index_graphics;
    if (queue_index_present != VK_QUEUE_FAMILY_IGNORED &&
        queue_index_present != queue_index_graphics)
        queue_create_infos[queue_create_info_count++].queueFamilyIndex =
            queue_index_present;
    if (queue_index_compute != VK_QUEUE_FAMILY_IGNORED &&
        queue_index_compute != queue_index_graphics &&
        queue_index_compute != queue_index_present)
        queue_create_infos[queue_create_info_count++].queueFamilyIndex =
            queue_index_compute;
    //   设备扩展:
    if (acquire_device_extensions(availableExtensions))
        return CtxResult::ACQUIRE_DEVICE_EXTENSIONS_FAILED;
    info.extensionNames.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    info.vmaFlags = static_cast<VmaAllocatorCreateFlagBits>(
        info.vmaFlags | check_VMA_extensions(info.extensionNames));
    if (check_device_extension(info.extensionNames)) {
        print_error("Context", "Init device extension failed!");
    }
    availableExtensions.clear();
    extensions.clear();
    // 3.创建逻辑设备
    VkDeviceCreateInfo deviceCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .flags = info.diviceFlags,
        .queueCreateInfoCount = queue_create_info_count,
        .pQueueCreateInfos = queue_create_infos,
        .enabledExtensionCount = uint32_t(info.extensionNames.size()),
        .ppEnabledExtensionNames = info.extensionNames.data()};
    vkStructureHead* last;
    if (vulkanApiVersion >= VK_API_VERSION_1_1) {
        auto* ptr = (vkStructureHead*)(info.pNextDivice);
        last = ptr;
        while (ptr)
            last = ptr, ptr = (vkStructureHead*)(ptr->pNext);
        if (last) {
            last->pNext = &phyDeviceFeatures;
            deviceCreateInfo.pNext = info.pNextDivice;
        } else {
            deviceCreateInfo.pNext = &phyDeviceFeatures;
        }
    } else {
        deviceCreateInfo.pEnabledFeatures = &phyDeviceFeatures.features;
    }
    if (VkResult result =
            vkCreateDevice(phyDevice, &deviceCreateInfo, nullptr, &device)) {
        print_error("Context",
                    "Failed to create a vulkan logical device! "
                    "Code: ",
                    string_VkResult(result));
        last->pNext = nullptr;
        return CtxResult::CREATE_DEVICE_FAILED;
    }
    last->pNext = nullptr;
    // 4.获取队列
    if (queue_index_graphics != VK_QUEUE_FAMILY_IGNORED)
        vkGetDeviceQueue(device, queue_index_graphics, 0, &queue_graphics);
    if (queue_index_present != VK_QUEUE_FAMILY_IGNORED)
        vkGetDeviceQueue(device, queue_index_present, 0, &queue_presentation);
    if (queue_index_compute != VK_QUEUE_FAMILY_IGNORED)
        vkGetDeviceQueue(device, queue_index_compute, 0, &queue_compute);
    if (prepare_VMA(info))
        return CtxResult::VMA_CREATE_FAILED;
    print_log("Context",
              "Renderer:", phyDeviceProperties.properties.deviceName);
    return CtxResult::SUCCESS;
}
void Context::update() {
    // 更新时间
    auto newtime = glfwGetTime();
    delta_time = newtime - current_time;
    current_time = newtime;
}
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
VkResult WindowContext::prepare_surface(Context& ctx) {
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    if (VkResult result =
            glfwCreateWindowSurface(ctx.instance, pWindow, nullptr, &surface)) {
        print_error("Context",
                    "Failed to create a window "
                    "surface! Code:",
                    string_VkResult(result));
        return VK_RESULT_MAX_ENUM;
    }
    this->surface = surface;
    return VK_SUCCESS;
}
CtxResult WindowContext::prepare_swapchain(const SwapchainCreateInfo info,
                                           Context& ctx) {
    if (create_swapchain(info, ctx))
        return CtxResult::SWAPCHAIN_CREATE_FAILED;
    return CtxResult::SUCCESS;
}

VkResult WindowContext::create_swapchain_Internal(Context& ctx) {
    auto& createInfo = swapchainCreateInfo;
    // 直接创建交换链
    if (VkResult result = vkCreateSwapchainKHR(ctx.device, &createInfo, nullptr,
                                               &swapchain)) {
        print_error("WindowContext",
                    "Failed to create a swapchain! "
                    "Code:",
                    string_VkResult(result));
        return result;
    }
    // 获取交换链图像
    uint32_t swapchainImageCount;
    if (VkResult result = vkGetSwapchainImagesKHR(
            ctx.device, swapchain, &swapchainImageCount, nullptr)) {
        print_error("WindowContext",
                    "Failed to get the count of swapchain images! Code:",
                    string_VkResult(result));
        return result;
    }
    swapchainImages.resize(swapchainImageCount);
    if (VkResult result =
            vkGetSwapchainImagesKHR(ctx.device, swapchain, &swapchainImageCount,
                                    swapchainImages.data())) {
        print_error("WindowContext", "Failed to get swapchain images! Code:",
                    string_VkResult(result));
        return result;
    }
    // 直接创建交换链，并且获取交换链图像和视图
    swapchainImageViews.resize(swapchainImageCount);
    VkImageViewCreateInfo imageViewCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = createInfo.imageFormat,
        //.components = {}, // 四个成员皆为VK_COMPONENT_SWIZZLE_IDENTITY
        .subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}};
    for (size_t i = 0; i < swapchainImageCount; i++) {
        imageViewCreateInfo.image = swapchainImages[i];
        if (VkResult result =
                vkCreateImageView(ctx.device, &imageViewCreateInfo, nullptr,
                                  &swapchainImageViews[i])) {
            print_error("WindowContext",
                        "Failed to create a swapchain image view! Code:",
                        string_VkResult(result));
            return result;
        }
    }
    return VK_SUCCESS;
}
VkResult WindowContext::acquire_surface_formats(Context& ctx) {
    uint32_t surfaceFormatCount;
    if (VkResult result = vkGetPhysicalDeviceSurfaceFormatsKHR(
            ctx.phyDevice, surface, &surfaceFormatCount, nullptr)) {
        print_error("WindowContext",
                    "Failed to get the count of surface "
                    "formats! Code:",
                    string_VkResult(result));
        return result;
    }
    if (!surfaceFormatCount)
        print_error("WindowContext",
                    "Failed to find any supported surface "
                    "format!"),
            abort();
    availableFormats.resize(surfaceFormatCount);
    VkResult result = vkGetPhysicalDeviceSurfaceFormatsKHR(
        ctx.phyDevice, surface, &surfaceFormatCount, availableFormats.data());
    if (result)
        print_error("WindowContext",
                    "Failed to get surface formats! "
                    "Code:",
                    string_VkResult(result));
    return VK_SUCCESS;
}
VkResult WindowContext::acquire_present_modes(
    std::vector<VkPresentModeKHR>& presentModes,
    Context& ctx) {
    uint32_t surfacePresentModeCount;
    if (VkResult result = vkGetPhysicalDeviceSurfacePresentModesKHR(
            ctx.phyDevice, surface, &surfacePresentModeCount, nullptr)) {
        print_error("WindowContext",
                    "Failed to get the count of surface present modes! Code:",
                    string_VkResult(result));
        return result;
    }
    if (!surfacePresentModeCount) {
        print_error("WindowContext",
                    "Failed to find any surface present mode!");
        abort();
    }
    presentModes.resize(surfacePresentModeCount);
    if (VkResult result = vkGetPhysicalDeviceSurfacePresentModesKHR(
            ctx.phyDevice, surface, &surfacePresentModeCount,
            presentModes.data())) {
        print_error("WindowContext",
                    "Failed to get surface present "
                    "modes! Code:",
                    string_VkResult(result));
        return result;
    }
    return VK_SUCCESS;
}
VkResult WindowContext::set_surface_format(VkSurfaceFormatKHR surfaceFormat,
                                           Context& ctx) {
    bool formatIsAvailable = false;
    if (!surfaceFormat.format) {
        // 如果格式未指定，只匹配色彩空间，图像格式有啥就用啥
        for (auto& i : availableFormats)
            if (i.colorSpace == surfaceFormat.colorSpace) {
                swapchainCreateInfo.imageFormat = i.format;
                swapchainCreateInfo.imageColorSpace = i.colorSpace;
                formatIsAvailable = true;
                break;
            }
    } else
        // 否则匹配格式和色彩空间
        for (auto& i : availableFormats)
            if (i.format == surfaceFormat.format &&
                i.colorSpace == surfaceFormat.colorSpace) {
                swapchainCreateInfo.imageFormat = i.format;
                swapchainCreateInfo.imageColorSpace = i.colorSpace;
                formatIsAvailable = true;
                break;
            }
    // 如果没有符合的格式，恰好有个语义相符的错误代码
    if (!formatIsAvailable)
        return VK_ERROR_FORMAT_NOT_SUPPORTED;
    // 如果交换链已存在，调用RecreateSwapchain()重建交换链
    if (swapchain)
        return recreate_swapchain(ctx);
    return VK_SUCCESS;
}
VkResult WindowContext::create_swapchain(const SwapchainCreateInfo& info,
                                         Context& ctx) {
    VkSurfaceCapabilitiesKHR surface_capabilities;
    // 获取surface支持能力
    if (VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
            ctx.phyDevice, surface, &surface_capabilities)) {
        print_error("WindowContext",
                    "Failed to get physical device surface capabilities! Code:",
                    string_VkResult(result));
        return result;
    }
    auto& createInfo = swapchainCreateInfo;
    // 如果容许的最大数量与最小数量不等，那么使用最小数量+1
    createInfo.minImageCount = surface_capabilities.minImageCount +
                               (surface_capabilities.maxImageCount >
                                surface_capabilities.minImageCount);
    // 决定窗口大小
    uint32_t width, height;
    glfwGetWindowSize(pWindow, (int*)&width, (int*)&height);
    // surface_capabilities.currentExtent.width为 ~0u 表示大小未确定
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
    // 指定处理交换链图像透明通道方式
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
    // 指定图像的用途
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    createInfo.imageUsage |= surface_capabilities.supportedUsageFlags &
                             VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    if (surface_capabilities.supportedUsageFlags &
        VK_IMAGE_USAGE_TRANSFER_DST_BIT)
        createInfo.imageUsage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    else
        print_warning("WindowContext",
                      "VK_IMAGE_USAGE_TRANSFER_DST_BIT isn't supported!");
    // 指定图像格式
    if (availableFormats.empty())
        if (VkResult result = acquire_surface_formats(ctx))
            return result;
    if (!createInfo.imageFormat)
        if (set_surface_format(
                {VK_FORMAT_R8G8B8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},
                ctx) &&
            set_surface_format(
                {VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},
                ctx)) {
            // 如果找不到上述图像格式和色彩空间的组合，那只能有什么用什么，采用availableSurfaceFormats中的第一组
            createInfo.imageFormat = availableFormats[0].format;
            createInfo.imageColorSpace = availableFormats[0].colorSpace;
            print_warning(
                "WindowContext",
                "Failed to select a four-component UNORM surface format!");
        }
    // 指定呈现模式
    std::vector<VkPresentModeKHR> surfacePresentModes;
    if (VkResult result = acquire_present_modes(surfacePresentModes, ctx)) {
        return result;
    }
    createInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
    if (!info.isFrameRateLimited)
        for (size_t i = 0; i < surfacePresentModes.size(); i++)
            if (surfacePresentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
                createInfo.presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
                break;
            }
    const char* mode_str = string_VkPresentModeKHR(createInfo.presentMode);
    print_log("Present Mode", mode_str);
    // ----------------
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.flags = info.flags;
    createInfo.surface = surface;
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;
    createInfo.pNext = nullptr;
    // ----------------
    if (VkResult result = create_swapchain_Internal(ctx))
        return result;
    iterate<WindowCallback::vk_swapchain_construct>();
    return VK_SUCCESS;
}
VkResult WindowContext::recreate_swapchain(Context& ctx) {
    auto& createInfo = swapchainCreateInfo;
    VkSurfaceCapabilitiesKHR surface_capabilities = {};
    VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        ctx.phyDevice, surface, &surface_capabilities);
    if (result != VK_SUCCESS) {
        print_error("WindowContext",
                    "Failed to get physical device surface capabilities! Code:",
                    string_VkResult(result));
        return result;
    }
    if (surface_capabilities.currentExtent.width == 0 ||
        surface_capabilities.currentExtent.height == 0)
        return VK_SUBOPTIMAL_KHR;
    createInfo.imageExtent = surface_capabilities.currentExtent;
    createInfo.oldSwapchain = swapchain;
    result = vkQueueWaitIdle(ctx.queue_graphics);
    // 仅在等待图形队列成功，且图形与呈现所用队列不同时等待呈现队列
    if (!result && ctx.queue_graphics != ctx.queue_presentation)
        result = vkQueueWaitIdle(ctx.queue_presentation);
    if (result) {
        print_error("WindowContext",
                    "Failed to wait for the queue to be idle! Code:",
                    string_VkResult(result));
        return result;
    }
    iterate<WindowCallback::vk_swapchain_destroy>();
    for (auto& i : swapchainImageViews)
        if (i)
            vkDestroyImageView(ctx.device, i, nullptr);
    swapchainImageViews.resize(0);
    result = create_swapchain_Internal(ctx);
    if (result != VK_SUCCESS) {
        print_error("WindowContext",
                    "Create swapchain failed! Code:", string_VkResult(result));
        return result;
    }
    iterate<WindowCallback::vk_swapchain_construct>();
    print_log("WindowContext", "Swapchain recreated!");
    return VK_SUCCESS;
}
CtxResult Context::prepare_context(
    ContextCreateInfo& info,
    std::span<std::pair<WindowCreateInfo, SwapchainCreateInfo>>& list,
    std::span<WindowContext*> ret) {
    CtxResult result;
    if (result = prepare_glfw(); result != CtxResult::SUCCESS)
        return result;
    if (result = prepare_instance(info.instance_info);
        result != CtxResult::SUCCESS)
        return result;
    for (size_t i = 0; i < list.size(); ++i) {
        if (result = create_window(list[i].first, ret[i]);
            result != CtxResult::SUCCESS)
            return result;
        if (ret[i]->prepare_surface(*this))
            return CtxResult::SURFACE_ACQUIRE_FAILED;
    }
    if (result = prepare_physical_device(); result != CtxResult::SUCCESS)
        return result;
    if (result = prepare_device(info.device_info); result != CtxResult::SUCCESS)
        return result;
    for (size_t i = 0; i < list.size(); ++i)
        if (result = ret[i]->prepare_swapchain(list[i].second, *this);
            result != CtxResult::SUCCESS)
            return result;
    return result;
}
}  // namespace BL