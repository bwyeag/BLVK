/******************************************************************************
MIT License

Copyright (c) 2025 bwyeag

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
******************************************************************************/
// 本地include
#include <bl_contexts.hpp>
#include <bl_util.hpp>
// 标准库include
#include <algorithm>
#include <cstring>
#include <mutex>
#include <vector>
#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>
namespace blt {
//*****************************************************************************
// WindowContextBase_*** 类
//*****************************************************************************

std::once_flag WindowContextBase_glfw::s_InitOnce_glfw{};
VkResult WindowContextBase_glfw::make_surface_impl(VkInstance instance,
                                              VkSurfaceKHR &surface) {
  return glfwCreateWindowSurface(instance, m_pWindow, nullptr, &surface);
}
CtxResult WindowContextBase_glfw::init_library() {
  static bool init_successful = false;
  std::call_once(s_InitOnce_glfw, [] {
    if (!glfwInit() || !glfwVulkanSupported()) {
      print_error(s_TypeName, "Failed to initialize GLFW!");
      return;
    }
    glfwSetErrorCallback([](int error_code, const char *description) {
      print_error("GLFW", "Error Code:", error_code, "; Desc:", description);
    });
    init_successful = true;
  });
  return init_successful ? CtxResult::Success : CtxResult::GLFWInitFailed;
}
void WindowContextBase_glfw::cleanup_library() noexcept { glfwTerminate(); }
CtxResult
WindowContextBase_glfw::create_window_impl(const WindowCreateInfo_glfw &info) {
  using State = WindowCreateState;
  if (CtxResult result = initialize(); result != CtxResult::Success)
    return result;
  // 1. 选取所用的监视器
  int monitor_count;
  GLFWmonitor **pMonitors = glfwGetMonitors(&monitor_count);
  if (!pMonitors)
    print_error(s_TypeName, "No monitors!"), abort();
  auto use_primary_monitor = [this]() {
    m_pMonitor = glfwGetPrimaryMonitor();
    if (!m_pMonitor)
      print_error(s_TypeName, "No primary monitor!"), abort();
  };
  if (info.m_InitState & State::use_primary_monitor) {
    use_primary_monitor();
  } else if (info.m_MonitorPred) {
    for (int i = 0; i < monitor_count; ++i)
      if (info.m_MonitorPred(pMonitors[i])) {
        m_pMonitor = pMonitors[i];
        break;
      }
    print_warning(s_TypeName, "No monitor selected! Use primary monitor.");
    use_primary_monitor();
  } else {
    print_error(s_TypeName, "No Monitor select function!");
    return CtxResult::WrongArgument;
  }
  // 2. 创建窗口
  m_Title = info.m_InitTitle;
  glfwDefaultWindowHints();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE,
                 static_cast<bool>(info.m_InitState & State::resizable));
  glfwWindowHint(GLFW_DECORATED,
                 static_cast<bool>(info.m_InitState & State::decorated));
  glfwWindowHint(GLFW_AUTO_ICONIFY,
                 static_cast<bool>(info.m_InitState & State::auto_iconify));
  glfwWindowHint(GLFW_FLOATING,
                 static_cast<bool>(info.m_InitState & State::window_floating));
  glfwWindowHint(
      GLFW_CENTER_CURSOR,
      static_cast<bool>(info.m_InitState & State::init_mouse_centered));
  glfwWindowHint(GLFW_VISIBLE,
                 !static_cast<bool>(info.m_InitState & State::init_unvisiable));
  State size_state = State(info.m_InitState & State::size_mask);
  const GLFWvidmode *pMode = glfwGetVideoMode(m_pMonitor);
  if (!pMode) {
    print_error("WindowContext", "Get video mode failed!");
    return CtxResult::FuncGetVideoModeFailed;
  }
  switch (size_state) {
  case State::full_screen:
    m_pWindow = glfwCreateWindow(pMode->width, pMode->height, m_Title.c_str(),
                                 m_pMonitor, nullptr);
    break;
  case State::maximized:
    glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
    m_pWindow = glfwCreateWindow(pMode->width, pMode->height, m_Title.c_str(),
                                 nullptr, nullptr);
    break;
  case State::specified:
    m_pWindow = glfwCreateWindow(info.m_InitSizeWidth, info.m_InitSizeHeight,
                                 m_Title.c_str(), nullptr, nullptr);
    break;
  default:
    m_pMonitor = nullptr;
    m_Title.clear();
    return CtxResult::WrongArgument;
  }
  if (!m_pWindow) {
    m_pMonitor = nullptr, m_pWindow = nullptr;
    m_Title.clear();
    return CtxResult::WindowCreateFailed;
  }
  if (info.m_InitPosWidth != (~0u) && info.m_InitPosHeight != (~0u))
    glfwSetWindowPos(m_pWindow, info.m_InitPosWidth, info.m_InitPosHeight);
  glfwSetWindowSizeLimits(m_pWindow, info.m_MinSizeWidth, info.m_MinSizeHeight,
                          info.m_MaxSizeWidth, info.m_MaxSizeHeight);
  glfwSetWindowUserPointer(m_pWindow, this);
  print_log(s_TypeName,
            std::format(
                "Window created!\n Title:{}\n Position:{},{}\n Extent:{},{}\n  "
                "with Size limits:{},{}~{},{}",
                m_Title, info.m_InitPosWidth, info.m_InitPosHeight, info.m_InitSizeWidth,
                info.m_InitSizeHeight, info.m_MinSizeWidth, info.m_MinSizeHeight,
                info.m_MaxSizeWidth, info.m_MaxSizeHeight));
  return CtxResult::Success;
}
void WindowContextBase_glfw::cleanup_window_impl() noexcept {
  if (m_pWindow)
    glfwDestroyWindow(m_pWindow), m_pWindow = nullptr;
  m_pMonitor = nullptr, m_Title.clear();
}
//*****************************************************************************
// Context 类
//*****************************************************************************

//*****************************************************************************
// create_instance() 部分

CtxResult ContextBase::create_instance(const InstanceCreateInfo &info) {
  uint32_t current_version = 0u;
  if (acquire_vkapi_version(current_version)) {
    print_error(s_TypeName, "acquire_vkapi_version failed!");
    return CtxResult::AcquireApiVersionFailed;
  }
  if (current_version < info.m_MinApiVersion) {
    print_error(s_TypeName, "Vulkan API version too low!");
    return CtxResult::VulkanAPIVersionTooLow;
  }
  m_VulkanApiVersion = std::max(current_version, info.m_MinApiVersion);
  VkApplicationInfo app_info = {
      .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
      .pApplicationName = info.m_pAppName,
      .applicationVersion = info.m_AppVersion,
      .pEngineName = "BLVK",
      .engineVersion = BL_VERSION,
      .apiVersion = m_VulkanApiVersion,
  };

  auto extension_names = info.m_ExtensionNames;
  auto layer_names = info.m_LayerNames;
#ifdef DEBUG
  if (info.m_isDebuging)
    insert_debug_ext_layers(layer_names, extension_names);
#endif // DEBUG
  {
    WindowContextBase_glfw::initialize();
    uint32_t extension_count = 0;
    const char **ppExtensionNames;
    ppExtensionNames = glfwGetRequiredInstanceExtensions(&extension_count);
    if (!ppExtensionNames && !glfwVulkanSupported()) {
      print_error(s_TypeName, "Vulkan is not available on this "
                              "machine!");
      return CtxResult::AcquireGlfwExtFailed;
    }
    extension_names.append_range(
        std::span<const char *>(ppExtensionNames, extension_count));
    // for (size_t i = 0; i < extensionCount; i++)
    //  info.m_ExtensionNames.push_back(extensionNames[i]);
  }

  if (VkResult result = check_instance_extension(extension_names)) {
    print_error(s_TypeName, "check_instance_extension() failed! Code:",
                string_VkResult(result));
    return CtxResult::CheckExtFailed;
  }
  if (VkResult result = check_instance_layer(layer_names)) {
    print_error(s_TypeName, "check_instance_layer() failed! Code:",
                string_VkResult(result));
    return CtxResult::CheckLayerFailed;
  }

  VkInstanceCreateInfo createInfo = {
      .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
      .pNext = info.m_pNextInstance,
      .flags = info.m_InstanceFlags,
      .pApplicationInfo = &app_info,
      .enabledLayerCount = uint32_t(layer_names.size()),
      .ppEnabledLayerNames = layer_names.data(),
      .enabledExtensionCount = uint32_t(extension_names.size()),
      .ppEnabledExtensionNames = extension_names.data()};

  auto print = [](const char *dec, std::vector<const char *> names) {
    for (auto name : names) {
      print_log(s_TypeName, dec, name ? name : "Null");
    };
  };
  if (VkResult result = vkCreateInstance(&createInfo, nullptr, &m_Instance)) {
    switch (result) {
    case VK_ERROR_LAYER_NOT_PRESENT:
    case VK_ERROR_EXTENSION_NOT_PRESENT:
      print_warning(s_TypeName,
                    "Vulkan instance some ext/layer not useable! Code:",
                    string_VkResult(result));
      print("Layer", layer_names);
      print("Ext", extension_names);
      break;
    default:
      print_error(s_TypeName, "Vulkan instance create failed! Code:",
                  string_VkResult(result));
      return CtxResult::InstanceCreateFailed;
    }
  }
  print_log(s_TypeName,
            "Vulkan API Version:", VK_API_VERSION_MAJOR(m_VulkanApiVersion),
            VK_API_VERSION_MINOR(m_VulkanApiVersion),
            VK_API_VERSION_PATCH(m_VulkanApiVersion));
#ifdef DEBUG
  if (info.m_isDebuging)
    if (VkResult result = create_debugger()) {
      print_error(s_TypeName,
                  "create debug failed! Code:", string_VkResult(result));
      return CtxResult::DebugCreateFailed;
    }
#endif // DEBUG
  return CtxResult::Success;
}
VkResult ContextBase::acquire_vkapi_version(uint32_t &version) {
  if (vkGetInstanceProcAddr(VK_NULL_HANDLE, "vkEnumerateInstanceVersion"))
    return vkEnumerateInstanceVersion(&version);
  else
    version = VK_API_VERSION_1_0;
  return VK_SUCCESS;
}
VkResult
ContextBase::check_instance_extension(std::span<const char *> extensionNames,
                                      const char *layerName) {
  uint32_t extension_count;
  std::vector<VkExtensionProperties> available_extensions;
  if (VkResult result = vkEnumerateInstanceExtensionProperties(
          layerName, &extension_count, nullptr)) {
    print_error(s_TypeName,
                "Failed to get the count of instance "
                "extension! Code:",
                string_VkResult(result));
    return result;
  }
  if (extension_count) {
    available_extensions.resize(extension_count);
    if (VkResult result = vkEnumerateInstanceExtensionProperties(
            layerName, &extension_count, available_extensions.data())) {
      print_error(s_TypeName,
                  "Failed to enumerate instance extension "
                  "properties! Code:",
                  string_VkResult(result));
      return result;
    }
    for (auto &i : extensionNames)
      if (auto it = std::find_if(available_extensions.begin(),
                                 available_extensions.end(),
                                 [i](VkExtensionProperties &j) {
                                   return !strcmp(i, j.extensionName);
                                 });
          it == available_extensions.end())
        i = nullptr;
  } else
    std::fill(extensionNames.begin(), extensionNames.end(), nullptr);
  return VK_SUCCESS;
}
VkResult ContextBase::check_instance_layer(std::span<const char *> layerNames) {
  uint32_t layer_count;
  std::vector<VkLayerProperties> available_layers;
  if (VkResult result =
          vkEnumerateInstanceLayerProperties(&layer_count, nullptr)) {
    print_error(s_TypeName, "Failed to get the count of instance layers! Code:",
                string_VkResult(result));
    return result;
  }
  if (layer_count) {
    available_layers.resize(layer_count);
    if (VkResult result = vkEnumerateInstanceLayerProperties(
            &layer_count, available_layers.data())) {
      print_error(s_TypeName,
                  "Failed to enumerate instance layer properties! Code:",
                  string_VkResult(result));
      return result;
    }
    for (auto &i : layerNames)
      if (auto it = std::find_if(
              available_layers.begin(), available_layers.end(),
              [i](VkLayerProperties &j) { return !strcmp(i, j.layerName); });
          it == available_layers.end())
        i = nullptr;
  } else
    std::fill(layerNames.begin(), layerNames.end(), nullptr);
  return VK_SUCCESS;
}
#ifdef DEBUG
std::string ContextBase::combine_debug_message(
    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData) {
  auto &sstm = acquire_local_data().m_LocalSstream;
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
      auto &label = pCallbackData->pObjects[i];
      sstm << "Type:" << string_VkObjectType(label.objectType) << '\t';
      sstm << "Handle:" << std::hex << label.objectHandle << std::dec << '\t';
      sstm << "Name:" << ((label.pObjectName) ? label.pObjectName : "Null")
           << '\n';
    }
  }
  if (pCallbackData->pMessage) {
    sstm << "Debug Message:\n" << pCallbackData->pMessage;
  }
  auto message = sstm.str();
  sstm.str("");
  return message;
}
VkResult ContextBase::create_debugger() {
  static PFN_vkDebugUtilsMessengerCallbackEXT DebugUtilsMessengerCallback =
      [](VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
         VkDebugUtilsMessageTypeFlagsEXT messageTypes,
         const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
         void *pUserData) -> VkBool32 {
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
      .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                         VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                         VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
      .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                     VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
      .pfnUserCallback = DebugUtilsMessengerCallback};
  // VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT
  PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessenger =
      reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
          vkGetInstanceProcAddr(m_Instance, "vkCreateDebugUtilsMessengerEXT"));
  if (vkCreateDebugUtilsMessenger) {
    VkResult result = vkCreateDebugUtilsMessenger(
        m_Instance, &debugUtilsMessengerCreateInfo, nullptr, &m_Debugger);
    if (result)
      print_error(s_TypeName, "Failed to create debug messenger! Code:",
                  string_VkResult(result));
    return result;
  }
  print_error(s_TypeName, "Failed to get the function pointer of "
                          "vkCreateDebugUtilsMessengerEXT!");
  return VK_RESULT_MAX_ENUM;
}
void ContextBase::insert_debug_ext_layers(
    std::vector<const char *> &layerNames,
    std::vector<const char *> &extensionNames) {
  layerNames.push_back("VK_LAYER_KHRONOS_validation");
  extensionNames.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
}
#endif // DEBUG
//*****************************************************************************
// create_device() 部分

CtxResult ContextBase::create_device(const DeviceCreateInfo &info,
                                     std::span<VkSurfaceKHR> surfaces) {
  init_physical_device(surfaces);
  // 1.构建队列创建表
  float queue_priority = 1.0f;
  VkDeviceQueueCreateInfo queue_create_infos[3] = {
      {.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
       .queueCount = 1,
       .pQueuePriorities = &queue_priority},
      {.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
       .queueCount = 1,
       .pQueuePriorities = &queue_priority},
      {.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
       .queueCount = 1,
       .pQueuePriorities = &queue_priority}};
  uint32_t queue_create_info_count = 0;
  uint32_t &queue_index_graphics = m_QueueFamilyIndex_graphics;
  uint32_t &queue_index_compute = m_QueueFamilyIndex_compute;
  uint32_t &queue_index_present = m_QueueFamilyIndex_presentation;
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
  // 设备扩展(设备没有层级，已经弃用)
  // 获取设备扩展并排序，放入m_AvailableExtensions, m_Extensions中
  if (acquire_device_extensions(m_AvailableExtensions))
    return CtxResult::AcquireDeviceExtensionsFailed;

  auto extension_names = info.m_ExtensionNames;
  extension_names.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
  auto vma_flags = static_cast<VmaAllocatorCreateFlagBits>(
      info.m_VmaFlags | acquire_vma_extensions(extension_names));
  check_device_extension(extension_names);
  m_AvailableExtensions.clear();
  m_Extensions.clear();
  [&extension_names]() {
    int length = 0;
    for (int i = 0; i < extension_names.size(); ++i) {
      if (extension_names[i] == nullptr)
        ++length;
      else if (length > 0)
        extension_names[i - length] = extension_names[i];
    }
    extension_names.resize(extension_names.size() - length);
  }();
  // std::erase_if(extension_names,
  //              [](const char *str) { return str == nullptr; });
  // 3.创建逻辑设备
  VkDeviceCreateInfo deviceCreateInfo = {
      .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
      .flags = info.m_DiviceFlags,
      .queueCreateInfoCount = queue_create_info_count,
      .pQueueCreateInfos = queue_create_infos,
      .enabledExtensionCount = uint32_t(extension_names.size()),
      .ppEnabledExtensionNames = extension_names.data()};
  vkStructureHead *last = nullptr;
  if (m_VulkanApiVersion >= VK_API_VERSION_1_1) {
    auto *ptr = (vkStructureHead *)(info.m_pNextDivice);
    while (ptr)
      last = ptr, ptr = (vkStructureHead *)(ptr->pNext);
    if (last) {
      last->pNext = &m_PhysicalDeviceFeatures;
      deviceCreateInfo.pNext = info.m_pNextDivice;
    } else {
      deviceCreateInfo.pNext = &m_PhysicalDeviceFeatures;
    }
  } else {
    deviceCreateInfo.pEnabledFeatures = &m_PhysicalDeviceFeatures.features;
  }
  if (VkResult result = vkCreateDevice(m_PhysicalDevice, &deviceCreateInfo,
                                       nullptr, &m_Device)) {
    print_error(s_TypeName, "Failed to create a vulkan logical device! Code: ",
                string_VkResult(result));
    if (last)
      last->pNext = nullptr;
    return CtxResult::DeviceCreateFailed;
  }
  if (last)
    last->pNext = nullptr;
  // 4.获取队列
  if (queue_index_graphics != VK_QUEUE_FAMILY_IGNORED)
    vkGetDeviceQueue(m_Device, queue_index_graphics, 0, &m_Queue_graphics);
  if (queue_index_present != VK_QUEUE_FAMILY_IGNORED)
    vkGetDeviceQueue(m_Device, queue_index_present, 0, &m_Queue_presentation);
  if (queue_index_compute != VK_QUEUE_FAMILY_IGNORED)
    vkGetDeviceQueue(m_Device, queue_index_compute, 0, &m_Queue_compute);
  if (init_vma(vma_flags))
    return CtxResult::VmaCreateFailed;
  print_log(s_TypeName,
            "Renderer:", m_PhysicalDeviceProperties.properties.deviceName);
  return CtxResult::Success;
}
VkResult ContextBase::acquire_physical_devices(
    std::vector<VkPhysicalDevice> &availablePhysicalDevices) {
  uint32_t device_count;
  if (VkResult result =
          vkEnumeratePhysicalDevices(m_Instance, &device_count, nullptr)) {
    print_error(s_TypeName,
                "Failed to get the count of physical devices! Code:",
                string_VkResult(result));
    return result;
  }
  if (!device_count) {
    print_error(s_TypeName,
                "Failed to find any physical device supports vulkan!");
    abort();
  }
  availablePhysicalDevices.resize(device_count);
  VkResult result = vkEnumeratePhysicalDevices(m_Instance, &device_count,
                                               availablePhysicalDevices.data());
  if (result)
    print_error(s_TypeName, "Failed to enumerate physical devices! Code:",
                string_VkResult(result));
  return result;
}
VkResult ContextBase::acquire_queue_family_indices(
    VkPhysicalDevice physicalDevice, uint32_t (&queueFamilyIndices)[3],
    std::span<VkSurfaceKHR> surfacesData, bool enableGraphicsQueue,
    bool enableComputeQueue) {
  uint32_t queue_family_count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queue_family_count,
                                           nullptr);
  if (!queue_family_count)
    return VK_RESULT_MAX_ENUM;
  std::vector<VkQueueFamilyProperties> queue_family_properties(
      queue_family_count);
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queue_family_count,
                                           queue_family_properties.data());
  auto &[ig, ip, ic] = queueFamilyIndices;
  ig = ip = ic = VK_QUEUE_FAMILY_IGNORED;
  for (uint32_t i = 0; i < queue_family_count; i++) {
    // 这三个VkBool32变量指示是否可获取（指应该被获取且能获取）相应队列族索引
    VkBool32
        // 只在enableGraphicsQueue为true时获取支持图形操作的队列族的索引
        support_graphics =
            enableGraphicsQueue &&
            queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT,
        support_presentation = false,
        // 只在enableComputeQueue为true时获取支持计算的队列族的索引
        support_compute =
            enableComputeQueue &&
            queue_family_properties[i].queueFlags & VK_QUEUE_COMPUTE_BIT;
    // 只在创建了window surface时获取支持呈现的队列族的索引
    if (!surfacesData.empty()) {
      for (auto &surface : surfacesData)
        if (VkResult result = vkGetPhysicalDeviceSurfaceSupportKHR(
                physicalDevice, i, surface, &support_presentation)) {
          print_error(s_TypeName,
                      "Failed to determine if the queue "
                      "family supports presentation! Code:",
                      string_VkResult(result));
          return result;
        }
    }
    // 若某队列族同时支持图形操作和计算
    if (support_graphics && support_compute) {
      // 若需要呈现，最好是三个队列族索引全部相同
      if (support_presentation) {
        ig = ip = ic = i;
        break;
      }
      // 除非ig和ic都已取得且相同，否则将它们的值覆写为i，以确保两个队列族索引相同
      if (ig != ic || ig == VK_QUEUE_FAMILY_IGNORED)
        ig = ic = i;
      // 如果不需要呈现，则break
      if (surfacesData.empty())
        break;
    }
    // 若任何一个队列族索引可以被取得但尚未被取得，将其值覆写为i
    if (support_graphics && ig == VK_QUEUE_FAMILY_IGNORED)
      ig = i;
    if (support_presentation && ip == VK_QUEUE_FAMILY_IGNORED)
      ip = i;
    if (support_compute && ic == VK_QUEUE_FAMILY_IGNORED)
      ic = i;
  }
  if ((ig == VK_QUEUE_FAMILY_IGNORED && enableGraphicsQueue) ||
      (ip == VK_QUEUE_FAMILY_IGNORED && surfacesData.size() > 0) ||
      (ic == VK_QUEUE_FAMILY_IGNORED && enableComputeQueue))
    return VK_RESULT_MAX_ENUM;
  m_QueueFamilyIndex_graphics = ig;
  m_QueueFamilyIndex_presentation = ip;
  m_QueueFamilyIndex_compute = ic;
  return VK_SUCCESS;
}
VkResult ContextBase::determine_physical_device(
    std::vector<VkPhysicalDevice> &availablePhysicalDevices,
    uint32_t deviceIndex, std::span<VkSurfaceKHR> surfacesData,
    bool enableGraphicsQueue, bool enableComputeQueue) {
  // 定义一个特殊值用于标记一个队列族索引已被找过但未找到
  constexpr uint32_t not_found =
      INT32_MAX; //== VK_QUEUE_FAMILY_IGNORED & INT32_MAX
  // 定义队列族索引组合的结构体
  struct QueueFamilyIndex_combination {
    uint32_t graphics = VK_QUEUE_FAMILY_IGNORED;
    uint32_t presentation = VK_QUEUE_FAMILY_IGNORED;
    uint32_t compute = VK_QUEUE_FAMILY_IGNORED;
  };
  // QueueFamilyIndex_combination用于为各个物理设备保存一份队列族索引组合
  static std::vector<QueueFamilyIndex_combination>
      queue_family_index_combinations(availablePhysicalDevices.size());
  auto &[ig, ip, ic] = queue_family_index_combinations[deviceIndex];

  // 如果有任何队列族索引已被找过但未找到，返回VK_RESULT_MAX_ENUM
  if ((ig == not_found && enableGraphicsQueue) ||
      (ip == not_found && !surfacesData.empty()) ||
      (ic == not_found && enableComputeQueue))
    return VK_RESULT_MAX_ENUM;

  // 如果有任何队列族索引应被获取但还未被找过
  if ((ig == VK_QUEUE_FAMILY_IGNORED && enableGraphicsQueue) ||
      (ip == VK_QUEUE_FAMILY_IGNORED && !surfacesData.empty()) ||
      (ic == VK_QUEUE_FAMILY_IGNORED && enableComputeQueue)) {
    uint32_t indices[3];
    VkResult result = acquire_queue_family_indices(
        availablePhysicalDevices[deviceIndex], indices, surfacesData,
        enableGraphicsQueue, enableComputeQueue);
    // 若GetQueueFamilyIndices(...)返回VK_SUCCESS或VK_RESULT_MAX_ENUM（vkGetPhysicalDeviceSurfaceSupportKHR(...)执行成功但没找齐所需队列族），
    // 说明对所需队列族索引已有结论，保存结果到queueFamilyIndexCombinations[deviceIndex]中相应变量
    // 应被获取的索引若仍为VK_QUEUE_FAMILY_IGNORED，说明未找到相应队列族，VK_QUEUE_FAMILY_IGNORED（~0u）与INT32_MAX做位与得到的数值等于notFound
    if (result == VK_SUCCESS || result == VK_RESULT_MAX_ENUM) {
      if (enableGraphicsQueue)
        ig = indices[0] & INT32_MAX;
      if (!surfacesData.empty())
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
    m_QueueFamilyIndex_graphics =
        enableGraphicsQueue ? ig : VK_QUEUE_FAMILY_IGNORED;
    m_QueueFamilyIndex_presentation =
        (!surfacesData.empty()) ? ip : VK_QUEUE_FAMILY_IGNORED;
    m_QueueFamilyIndex_compute =
        enableComputeQueue ? ic : VK_QUEUE_FAMILY_IGNORED;
  }
  m_PhysicalDevice = availablePhysicalDevices[deviceIndex];
  return VK_SUCCESS;
}
void ContextBase::acquire_physical_divice_properties() {
  //   设备属性:
  if (m_VulkanApiVersion >= VK_API_VERSION_1_1) {
    m_PhysicalDeviceProperties = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2};
    m_PhysicalDeviceVulkan11Properties = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_PROPERTIES};
    m_PhysicalDeviceVulkan12Properties = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_PROPERTIES};
    m_PhysicalDeviceVulkan13Properties = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_PROPERTIES};
    m_PhysicalDeviceVulkan14Properties = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_PROPERTIES};
    if (m_VulkanApiVersion >= VK_API_VERSION_1_2) {
      m_PhysicalDeviceProperties.pNext = &m_PhysicalDeviceVulkan11Properties;
      m_PhysicalDeviceVulkan11Properties.pNext =
          &m_PhysicalDeviceVulkan12Properties;
      if (m_VulkanApiVersion >= VK_API_VERSION_1_3) {
        m_PhysicalDeviceVulkan12Properties.pNext =
            &m_PhysicalDeviceVulkan13Properties;
        if (m_VulkanApiVersion >= VK_API_VERSION_1_4)
          m_PhysicalDeviceVulkan13Properties.pNext =
              &m_PhysicalDeviceVulkan14Properties;
      }
    }
    vkGetPhysicalDeviceProperties2(m_PhysicalDevice,
                                   &m_PhysicalDeviceProperties);
    m_PhysicalDeviceMemoryProperties = {
        VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2};
    vkGetPhysicalDeviceMemoryProperties2(m_PhysicalDevice,
                                         &m_PhysicalDeviceMemoryProperties);
  } else {
    vkGetPhysicalDeviceProperties(m_PhysicalDevice,
                                  &m_PhysicalDeviceProperties.properties);
    vkGetPhysicalDeviceMemoryProperties(
        m_PhysicalDevice, &m_PhysicalDeviceMemoryProperties.memoryProperties);
  }
}
void ContextBase::acquire_physical_divice_features() {
  //   设备特性:
  if (m_VulkanApiVersion >= VK_API_VERSION_1_1) {
    m_PhysicalDeviceFeatures = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2};
    m_PhysicalDeviceVulkan11Features = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES};
    m_PhysicalDeviceVulkan12Features = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES};
    m_PhysicalDeviceVulkan13Features = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES};
    m_PhysicalDeviceVulkan14Features = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES};
    if (m_VulkanApiVersion >= VK_API_VERSION_1_2) {
      m_PhysicalDeviceFeatures.pNext = &m_PhysicalDeviceVulkan11Features;
      m_PhysicalDeviceVulkan11Features.pNext =
          &m_PhysicalDeviceVulkan12Features;
      if (m_VulkanApiVersion >= VK_API_VERSION_1_3) {
        m_PhysicalDeviceVulkan12Features.pNext =
            &m_PhysicalDeviceVulkan13Features;
        if (m_VulkanApiVersion >= VK_API_VERSION_1_4)
          m_PhysicalDeviceVulkan13Features.pNext =
              &m_PhysicalDeviceVulkan14Features;
      }
    }
    vkGetPhysicalDeviceFeatures2(m_PhysicalDevice, &m_PhysicalDeviceFeatures);
  } else
    vkGetPhysicalDeviceFeatures(m_PhysicalDevice,
                                &m_PhysicalDeviceFeatures.features);
}
CtxResult
ContextBase::init_physical_device(std::span<VkSurfaceKHR> surfacesData) {
  std::vector<VkPhysicalDevice> available_physical_devices;
  if (acquire_physical_devices(available_physical_devices))
    return CtxResult::AcquirePhysicalDevicesFailed;
  for (uint32_t i = 0; i < available_physical_devices.size(); ++i)
    if (determine_physical_device(available_physical_devices, i, surfacesData,
                                  true, true) == VK_SUCCESS)
      goto FIND_SUCCESS;
  print_error(s_TypeName, "Can not find any phyDevice fits all conditions!");
  return CtxResult::NoFitDevice;
FIND_SUCCESS:
  acquire_physical_divice_properties();
  acquire_physical_divice_features();
  return CtxResult::Success;
}
VkResult ContextBase::acquire_device_extensions(
    std::vector<VkExtensionProperties> &extensionNames, const char *layerName) {
  uint32_t extCount;
  if (VkResult result = vkEnumerateDeviceExtensionProperties(
          m_PhysicalDevice, layerName, &extCount, nullptr)) {
    print_error(s_TypeName,
                "vkEnumerateDeviceExtensionProperties() failed! Code:",
                string_VkResult(result));
    return VK_RESULT_MAX_ENUM;
  }
  extensionNames.resize(extCount);
  if (VkResult result = vkEnumerateDeviceExtensionProperties(
          m_PhysicalDevice, layerName, &extCount, extensionNames.data())) {
    print_error(s_TypeName,
                "vkEnumerateDeviceExtensionProperties() failed! Code:",
                string_VkResult(result));
    return VK_RESULT_MAX_ENUM;
  }
  m_Extensions.resize(m_AvailableExtensions.size());
  std::transform(m_AvailableExtensions.begin(), m_AvailableExtensions.end(),
                 m_Extensions.begin(),
                 [](VkExtensionProperties &ext) { return ext.extensionName; });
  std::sort(m_Extensions.begin(), m_Extensions.end(),
            [](const char *a, const char *b) { return std::strcmp(a, b) < 0; });
  return VK_SUCCESS;
}
//*****************************************************************************
// VMA 部分
static constexpr uint32_t vma_flags_count = 8;
static constexpr std::pair<const char *, VmaAllocatorCreateFlagBits>
    vma_flags[]{
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
VmaAllocatorCreateFlagBits
ContextBase::acquire_vma_extensions(std::vector<const char *> &extensionNames) {
  auto ret = static_cast<VmaAllocatorCreateFlagBits>(0);
  for (uint32_t i = 0; i < vma_flags_count; ++i)
    if (!std::binary_search(m_Extensions.begin(), m_Extensions.end(),
                            vma_flags[i].first,
                            [](const char *a, const char *b) {
                              return std::strcmp(a, b) < 0;
                            })) {
      extensionNames.push_back(vma_flags[i].first);
      ret = static_cast<VmaAllocatorCreateFlagBits>(ret | vma_flags[i].second);
    }
  return ret;
}

void ContextBase::check_device_extension(std::span<const char *> extensionNames,
                                         const char *layerName) {
  for (auto &i : extensionNames) {
    if (!std::binary_search(
            m_Extensions.begin(), m_Extensions.end(), i,
            [](const char *a, const char *b) { return std::strcmp(a, b) < 0; }))
      i = nullptr;
  }
}
VkResult ContextBase::init_vma(VmaAllocatorCreateFlagBits vmaFlags) {
  VmaAllocatorCreateInfo allocatorCreateInfo = {
      .flags = vmaFlags,
      .physicalDevice = m_PhysicalDevice,
      .device = m_Device,
      .instance = m_Instance,
      .vulkanApiVersion = m_VulkanApiVersion};
  return vmaCreateAllocator(&allocatorCreateInfo, &m_Allocator);
}
void ContextBase::cleanup() noexcept {
  if (!m_Instance)
    return;
  if (m_Device) {
    if (VkResult result = vkDeviceWaitIdle(m_Device))
      print_warning(s_TypeName, "cleanup device waitIdle failed! Code:",
                    string_VkResult(result));
    vkDestroyDevice(m_Device, nullptr);
    m_Device = VK_NULL_HANDLE;
  }
  if (m_Debugger) {
    PFN_vkDestroyDebugUtilsMessengerEXT DestroyDebugUtilsMessenger =
        reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
            vkGetInstanceProcAddr(m_Instance,
                                  "vkDestroyDebugUtilsMessengerEXT"));
    if (DestroyDebugUtilsMessenger)
      DestroyDebugUtilsMessenger(m_Instance, m_Debugger, nullptr);
    m_Debugger = VK_NULL_HANDLE;
  }
  vkDestroyInstance(m_Instance, nullptr);
  m_Instance = VK_NULL_HANDLE;
}
void ContextBase::update() {
  double time_now = glfwGetTime();
  m_DeltaTime = time_now - m_CurrentTime;
  m_CurrentTime = time_now;
  m_CallbackUpdate.iterate(this);
}
} // namespace blt
