#include <bl_vkcontext.hpp>
#include <cstring>

namespace BL {
const char* ContextErrorCategory::name() const noexcept {
    return "Vulkan Context error";
}
std::string ContextErrorCategory::message(int ev) const {
    using Enum = ContextErrorEnum;
    switch (static_cast<Enum>(ev)) {
        case Enum::Success:
            return "no error";
        default:
            return "unknown error";
    }
}
static ContextErrorCategory category;
[[nodiscard]]
std::error_code make_error_code(ContextErrorEnum e) {
    return {static_cast<int>(e), category};
}
// Context Class
VkResult Context::get_api_version(uint32_t* apiVersion) {
    if (vkGetInstanceProcAddr(VK_NULL_HANDLE, "vkEnumerateInstanceVersion"))
        return vkEnumerateInstanceVersion(apiVersion);
    else
        *apiVersion = VK_API_VERSION_1_0;
    return VK_SUCCESS;
}
std::vector<const char*> Context::_get_instance_extension(
    const ContextInstanceInit* pInit) {
    std::vector<const char*> extensions;
    uint32_t extensionCount = 0;
    const char** extensionNames;
    extensionNames = glfwGetRequiredInstanceExtensions(&extensionCount);
    if (!extensionNames) {
        print_error("Context", "Get Vulkan extensions failed!");
    }
    for (size_t i = 0; i < extensionCount; i++)
        extensions.push_back(extensionNames[i]);
#ifdef BL_DEBUG
    if (pInit->isDebuging) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
#else
    if (pInit->isDebuging) {
        print_warning("Context", "debuging open but BL_DEBUG not defined!");
    }
#endif  // BL_DEBUG
    for (size_t i = 0; i < pInit->extensionCount; i++) {
        for (size_t j = 0; j < extensions.size(); j++)
            if (std::strcmp(pInit->ppExtensionNames[i], extensions[j]) == 0)
                goto IGNORE_EXT;
        extensions.push_back(pInit->ppExtensionNames[i]);
    IGNORE_EXT:
        continue;
    }
    return extensions;
}
std::vector<const char*> Context::_get_instance_layer(
    const ContextInstanceInit* pInit) {
    std::vector<const char*> layers;
#ifdef BL_DEBUG
    if (pInit->isDebuging) {
        layers.push_back("VK_LAYER_KHRONOS_validation");
    }
#else
    if (pInit->isDebuging) {
        print_warning("Context", "debuging open but BL_DEBUG not defined!");
    }
#endif  // BL_DEBUG
    for (size_t i = 0; i < pInit->layerCount; i++) {
        bool flag = true;
        for (size_t j = 0; j < layers.size(); j++)
            if (std::strcmp(pInit->ppLayerNames[i], layers[j]) == 0)
                goto IGNORE_LAYER;
        layers.push_back(pInit->ppLayerNames[i]);
    IGNORE_LAYER:
        continue;
    }
    return layers;
}
#ifdef BL_DEBUG
VkResult Context::_create_debug_messenger() {
    static PFN_vkDebugUtilsMessengerCallbackEXT DebugUtilsMessengerCallback =
        [](VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
           VkDebugUtilsMessageTypeFlagsEXT messageTypes,
           const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
           void* pUserData) -> VkBool32 {
        std::cout << pCallbackData->pMessage << '\n';
        return VK_FALSE;
    };
    VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
        .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                       VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                       VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        .pfnUserCallback = DebugUtilsMessengerCallback};
    PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessenger =
        reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
            vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
    if (vkCreateDebugUtilsMessenger) {
        VkResult result = vkCreateDebugUtilsMessenger(
            instance, &debugUtilsMessengerCreateInfo, nullptr, &debugger);
        if (result)
            print_error("Context",
                        "Failed to create debug messenger! Error code:",
                        int32_t(result));
        return result;
    }
    print_error("Context",
                "Failed to get the function pointer of "
                "vkCreateDebugUtilsMessengerEXT!");
    return VK_RESULT_MAX_ENUM;
}
#endif  // BL_DEBUG
std::error_code Context::create_instance(const ContextInstanceInit* pInit) {
    uint32_t curApiVersion = 0u;
    get_api_version(&curApiVersion);
    if (curApiVersion < pInit->minApiVersion) {
        print_error("Context", "Vulkan api version too low!");
        return make_error_code(ContextErrorEnum::VulkanVersionTooLow);
    }
    vulkanApiVersion = std::max(curApiVersion, pInit->minApiVersion);
    VkApplicationInfo appInfo = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = pInit->pAppName,
        .applicationVersion = pInit->appVersion,
        .pEngineName = "blvk",
        .engineVersion = VK_MAKE_API_VERSION(0, 0, 1, 0),
        .apiVersion = vulkanApiVersion,
    };

    auto extensions = _get_instance_extension(pInit);
    auto layers = _get_instance_layer(pInit);

    VkInstanceCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = pInit->pNextInstance,
        .flags = 0,
        .pApplicationInfo = &appInfo,
        .enabledLayerCount = uint32_t(layers.size()),
        .ppEnabledLayerNames = layers.data(),
        .enabledExtensionCount = uint32_t(extensions.size()),
        .ppEnabledExtensionNames = extensions.data()};

    if (VkResult result = vkCreateInstance(&createInfo, nullptr, &instance)) {
        print_error("Context", "Vulkan instance create failed!",
                    int32_t(result));
        return make_error_code(result);
    }
#ifdef BL_DEBUG
    if (pInit->isDebuging && _create_debug_messenger() != VK_SUCCESS) {
        print_error("Context", "create debug failed!");
        return make_error_code(ContextErrorEnum::DebugMessengerCreateFailed);
    }
#else
    if (pInit->isDebuging) {
        print_warning("Context", "debuging open but BL_DEBUG not defined!");
    }
#endif  // BL_DEBUG
    print_log("Context",
              "Vulkan API Version:", VK_API_VERSION_MAJOR(vulkanApiVersion),
              VK_API_VERSION_MINOR(vulkanApiVersion),
              VK_API_VERSION_PATCH(vulkanApiVersion));
    return make_error_code(ContextErrorEnum::Success);
}
VkResult Context::_get_physical_devices(
    std::vector<VkPhysicalDevice>& avaliablePhyDevices) {
    uint32_t deviceCount;
    if (VkResult result =
            vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr)) {
        print_error("Context",
                    "Failed to get the count of physical devices! "
                    "Error code: ",
                    int32_t(result));
        return result;
    }
    if (!deviceCount) {
        print_error("Context",
                    "Failed to find any physical device supports "
                    "vulkan!");
        return VK_RESULT_MAX_ENUM;
    }
    avaliablePhyDevices.resize(deviceCount);
    VkResult result = vkEnumeratePhysicalDevices(instance, &deviceCount,
                                                 avaliablePhyDevices.data());
    if (result)
        print_error("Context",
                    "Failed to enumerate physical devices! Error code: ",
                    int32_t(result));
    return result;
}
VkResult Context::_get_queue_family_indices(VkPhysicalDevice physicalDevice,
                                            const ContextDeviceInit* pInit) {
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount,
                                             nullptr);
    if (queueFamilyCount == 0)
        return VK_RESULT_MAX_ENUM;
    std::vector<VkQueueFamilyProperties> queueFamilyPropertieses(
        queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount,
                                             queueFamilyPropertieses.data());
    for (uint32_t i = 0; i < queueFamilyCount; i++) {
        VkBool32 supportGraphics = queueFamilyPropertieses[i].queueFlags &
                                   VK_QUEUE_GRAPHICS_BIT,
                 supportPresentation = VK_TRUE,
                 supportCompute = queueFamilyPropertieses[i].queueFlags &
                                  VK_QUEUE_COMPUTE_BIT;
        if (pInit->surfaceCount > 0)
            for (uint32_t j = 0; j < pInit->surfaceCount; j++) {
                VkBool32 support;
                if (VkResult result = vkGetPhysicalDeviceSurfaceSupportKHR(
                        physicalDevice, i, pInit->surface[j], &support)) {
                    print_error("Context",
                                "Failed to check physical device "
                                "surface support! Error code: ",
                                int32_t(result));
                    return result;
                }
                supportPresentation = supportPresentation & support;
            }
        if (supportGraphics && supportCompute &&
            (!pInit->surface || supportPresentation)) {
            queueFamilyIndex_graphics = queueFamilyIndex_compute = i;
            if (pInit->surface)
                queueFamilyIndex_presentation = i;
            return VK_SUCCESS;
        }
    }
    return VK_RESULT_MAX_ENUM;
}
VkResult Context::_select_physical_device(const ContextDeviceInit* pInit) {
    std::vector<VkPhysicalDevice> avaliablePhyDevices;
    if (VkResult result = _get_physical_devices(avaliablePhyDevices)) {
        return result;
    }
    bool flag = false;
    if (pInit->isDeviceSuitable)
        for (size_t i = 0; i < avaliablePhyDevices.size(); i++)
            if (pInit->isDeviceSuitable(avaliablePhyDevices[i]))
                phyDevice = avaliablePhyDevices[i], flag = true;
    if (!flag)
        phyDevice = avaliablePhyDevices[0];
    if (VkResult result = _get_queue_family_indices(phyDevice, pInit)) {
        return result;
    }
    return VK_SUCCESS;
}
const uint32_t vma_extcheck_count = 4;
static const char* vma_extensionNames[vma_extcheck_count] = {
    VK_KHR_MAINTENANCE_4_EXTENSION_NAME, VK_KHR_MAINTENANCE_5_EXTENSION_NAME,
    VK_EXT_MEMORY_BUDGET_EXTENSION_NAME,
    VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME};
static const VmaAllocatorCreateFlagBits vma_ext_flags[vma_extcheck_count] = {
    VMA_ALLOCATOR_CREATE_KHR_MAINTENANCE4_BIT,
    VMA_ALLOCATOR_CREATE_KHR_MAINTENANCE5_BIT,
    VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT,
    VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT};
static VmaAllocatorCreateFlagBits vma_extensions;
std::vector<const char*> Context::_select_device_extensions(
    const std::vector<VkExtensionProperties>& supported_ext) {
    std::vector<const char*> extensions{"VK_KHR_swapchain"};
    extensions.reserve(extensions.size() + supported_ext.size());
    for (const VkExtensionProperties& it : supported_ext) {
        for (uint32_t i = 0; i < vma_extcheck_count; i++) {
            if (std::strcmp(vma_extensionNames[i], it.extensionName) == 0) {
                extensions.push_back(it.extensionName);
                vma_extensions = VmaAllocatorCreateFlagBits(vma_extensions |
                                                            vma_ext_flags[i]);
            }
        }
    }
    return extensions;
}
std::vector<VkExtensionProperties> Context::_get_device_extensions(
    const ContextDeviceInit* pInit) {
    std::vector<VkExtensionProperties> ext;
    uint32_t ext_count = 0;
    VkResult res = vkEnumerateDeviceExtensionProperties(phyDevice, nullptr,
                                                        &ext_count, nullptr);
    if (res != VK_SUCCESS) {
        print_error("getDeviceExtensions", "enumerate device ext failed!");
    }
    ext.resize(ext_count);
    vkEnumerateDeviceExtensionProperties(phyDevice, nullptr, &ext_count,
                                         ext.data());
    if (pInit->debug_print_deviceExtension) {
        print_log("Info",
                  "Supported Device Extensions list: count:", ext_count);
        for (const VkExtensionProperties& it : ext) {
            print_log("Info", '\t', it.extensionName);
        }
    }
    return ext;
}
VkResult Context::_init_VMA(const ContextDeviceInit* pInit) {
    VmaAllocatorCreateInfo create_info = {
        .flags = pInit->vmaFlags | vma_extensions,
        .physicalDevice = phyDevice,
        .device = device,
        .instance = instance,
        .vulkanApiVersion = vulkanApiVersion};
    VkResult res = vmaCreateAllocator(&create_info, &allocator);
    if (res != VK_SUCCESS) {
        print_error("Context",
                    "VMA allocator create failed! Error Code:", int32_t(res));
        return res;
    }
    print_log("Info", "VMA create successfull.");
    return VK_SUCCESS;
}
void Context::_get_physical_divice_properties() {
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
void Context::_get_physical_divice_features() {
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
std::error_code Context::create_device(const ContextDeviceInit* pInit) {
    float queuePriority = 1.0f;
    if (VkResult result = _select_physical_device(pInit))
        return make_error_code(result);
    // 1.构建队列创建表
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
    // 2.检查设备特性和扩展
    _get_physical_divice_features();
    //   设备扩展:
    std::vector<VkExtensionProperties> supported_ext =
        _get_device_extensions(pInit);
    std::vector<const char*> deviceExtensions =
        _select_device_extensions(supported_ext);
    // 3.创建逻辑设备
    VkDeviceCreateInfo deviceCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .flags = pInit->diviceFlags,
        .queueCreateInfoCount = queue_create_info_count,
        .pQueueCreateInfos = queue_create_infos,
        .enabledExtensionCount = uint32_t(deviceExtensions.size()),
        .ppEnabledExtensionNames = deviceExtensions.data()};
    vkStructureHead* last;
    if (vulkanApiVersion >= VK_API_VERSION_1_1) {
        auto* ptr = (vkStructureHead*)(pInit->pNextDivice);
        last = ptr;
        while (ptr)
            last = ptr, ptr = (vkStructureHead*)(ptr->pNext);
        if (last) {
            last->pNext = &phyDeviceFeatures;
            deviceCreateInfo.pNext = pInit->pNextDivice;
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
                    "Error code: ",
                    int32_t(result));
        last->pNext = nullptr;
        return make_error_code(result);
    }
    last->pNext = nullptr;
    // 4.获取队列
    if (queue_index_graphics != VK_QUEUE_FAMILY_IGNORED)
        vkGetDeviceQueue(device, queue_index_graphics, 0, &queue_graphics);
    if (queue_index_present != VK_QUEUE_FAMILY_IGNORED)
        vkGetDeviceQueue(device, queue_index_present, 0, &queue_presentation);
    if (queue_index_compute != VK_QUEUE_FAMILY_IGNORED)
        vkGetDeviceQueue(device, queue_index_compute, 0, &queue_compute);
    if (VkResult result = _init_VMA(pInit)) {
        return make_error_code(ContextErrorEnum::VMACreateFailed);
    }
    print_log("Context",
              "Renderer:", phyDeviceProperties.properties.deviceName);
    return make_error_code(ContextErrorEnum::Success);
}
void Context::destroy_device() {
    if (allocator) {
        vmaDestroyAllocator(allocator);
        allocator = VK_NULL_HANDLE;
    }
    if (device) {
        vkDeviceWaitIdle(device);
        vkDestroyDevice(device, nullptr);
        device = VK_NULL_HANDLE;
    }
    phyDevice = VK_NULL_HANDLE;
}
void Context::destroy_instance() {
#ifdef BL_DEBUG
    if (debugger) {
        PFN_vkDestroyDebugUtilsMessengerEXT DestroyDebugUtilsMessenger =
            reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
                vkGetInstanceProcAddr(instance,
                                      "vkDestroyDebugUtilsMessengerEXT"));
        if (DestroyDebugUtilsMessenger)
            DestroyDebugUtilsMessenger(instance, debugger, nullptr);
        debugger = VK_NULL_HANDLE;
    }
#endif  // BL_DEBUG
    if (instance) {
        vkDestroyInstance(instance, nullptr);
        instance = VK_NULL_HANDLE;
    }
}
}  // namespace BL
