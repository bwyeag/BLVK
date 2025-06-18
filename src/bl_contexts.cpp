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
#include "vulkan/vulkan_core.h"
#include <bl_contexts.hpp>
#include <corecrt.h>
#define VMA_IMPLEMENTATION
#include <algorithm>
#include <vma/vk_mem_alloc.h>
#define BL_VERSION VK_MAKE_API_VERSION(0, 0, 1, 1)
namespace BL {
namespace _detail {
// void __glfw_callback_windowpos(GLFWwindow* window, int xpos, int ypos) {
//     auto* ptr = (WindowContext*)glfwGetWindowUserPointer(window);
//     ptr->callback.callback_windowpos.iterate(ptr, xpos, ypos);
// }
// void __glfw_callback_windowsize(GLFWwindow* window, int width, int height) {
//     auto* ptr = (WindowContext*)glfwGetWindowUserPointer(window);
//     ptr->callback.callback_windowsize.iterate(ptr, width, height);
// }
// void __glfw_callback_windowclose(GLFWwindow* window) {
//     auto* ptr = (WindowContext*)glfwGetWindowUserPointer(window);
//     ptr->callback.callback_windowclose.iterate(ptr);
// }
// void __glfw_callback_windowrefresh(GLFWwindow* window) {
//     auto* ptr = (WindowContext*)glfwGetWindowUserPointer(window);
//     ptr->callback.callback_windowrefresh.iterate(ptr);
// }
// void __glfw_callback_windowfocus(GLFWwindow* window, int focused) {
//     auto* ptr = (WindowContext*)glfwGetWindowUserPointer(window);
//     ptr->callback.callback_windowfocus.iterate(ptr, focused);
// }
// void __glfw_callback_windowiconify(GLFWwindow* window, int iconified) {
//     auto* ptr = (WindowContext*)glfwGetWindowUserPointer(window);
//     ptr->callback.callback_windowiconify.iterate(ptr, iconified);
// }
// void __glfw_callback_windowmaximize(GLFWwindow* window, int maximized) {
//     auto* ptr = (WindowContext*)glfwGetWindowUserPointer(window);
//     ptr->callback.callback_windowmaximize.iterate(ptr, maximized);
// }
// void __glfw_callback_windowcontentscale(GLFWwindow* window,
//                                         float xscale,
//                                         float yscale) {
//     auto* ptr = (WindowContext*)glfwGetWindowUserPointer(window);
//     ptr->callback.callback_windowcontentscale.iterate(ptr, xscale, yscale);
// }
// void __glfw_callback_mousebutton(GLFWwindow* window,
//                                  int button,
//                                  int action,
//                                  int mods) {
//     auto* ptr = (WindowContext*)glfwGetWindowUserPointer(window);
//     ptr->callback.callback_mousebutton.iterate(ptr, button, action, mods);
// }
// void __glfw_callback_cursorpos(GLFWwindow* window, double xpos, double ypos)
// {
//     auto* ptr = (WindowContext*)glfwGetWindowUserPointer(window);
//     ptr->callback.callback_cursorpos.iterate(ptr, xpos, ypos);
// }
// void __glfw_callback_cursorenter(GLFWwindow* window, int entered) {
//     auto* ptr = (WindowContext*)glfwGetWindowUserPointer(window);
//     ptr->callback.callback_cursorenter.iterate(ptr, entered);
// }
// void __glfw_callback_scroll(GLFWwindow* window,
//                             double xoffset,
//                             double yoffset) {
//     auto* ptr = (WindowContext*)glfwGetWindowUserPointer(window);
//     ptr->callback.callback_scroll.iterate(ptr, xoffset, yoffset);
// }
// void __glfw_callback_keybord(GLFWwindow* window,
//                              int key,
//                              int scancode,
//                              int action,
//                              int mods) {
//     auto* ptr = (WindowContext*)glfwGetWindowUserPointer(window);
//     ptr->callback.callback_key.iterate(ptr, key, scancode, action, mods);
// }
// void __glfw_callback_charinput(GLFWwindow* window, unsigned int codepoint) {
//     auto* ptr = (WindowContext*)glfwGetWindowUserPointer(window);
//     ptr->callback.callback_char.iterate(ptr, codepoint);
// }
// void __glfw_callback_charmods(GLFWwindow* window,
//                               unsigned int codepoint,
//                               int mods) {
//     auto* ptr = (WindowContext*)glfwGetWindowUserPointer(window);
//     ptr->callback.callback_charmods.iterate(ptr, codepoint, mods);
// }
// void __glfw_callback_drop(GLFWwindow* window,
//                           int path_count,
//                           const char* paths[]) {
//     auto* ptr = (WindowContext*)glfwGetWindowUserPointer(window);
//     ptr->callback.callback_drop.iterate(ptr, path_count, paths);
// }
} // namespace _detail
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
    print_error("Context",
                "Failed to get the count of instance "
                "extension! Code:",
                string_VkResult(result));
    return result;
  }
  if (extension_count) {
    available_extensions.resize(extension_count);
    if (VkResult result = vkEnumerateInstanceExtensionProperties(
            layerName, &extension_count, available_extensions.data())) {
      print_error("Context",
                  "Failed to enumerate instance extension "
                  "properties! Code:",
                  string_VkResult(result));
      return result;
    }
    for (auto &i : extensionNames) {
      for (auto &j : available_extensions)
        if (!strcmp(i, j.extensionName))
          goto CHECK_FOUND;
      i = nullptr;
    CHECK_FOUND:
      continue;
    }
  } else
    std::fill(extensionNames.begin(), extensionNames.end(), nullptr);
  return VK_SUCCESS;
}
VkResult ContextBase::check_instance_layer(std::span<const char *> layerNames) {
  uint32_t layer_count;
  std::vector<VkLayerProperties> available_layers;
  if (VkResult result =
          vkEnumerateInstanceLayerProperties(&layer_count, nullptr)) {
    print_error("Context",
                "Failed to get the count of instance "
                "layers! Code:",
                string_VkResult(result));
    return result;
  }
  if (layer_count) {
    available_layers.resize(layer_count);
    if (VkResult result = vkEnumerateInstanceLayerProperties(
            &layer_count, available_layers.data())) {
      print_error("Context",
                  "Failed to enumerate instance layer "
                  "properties! Code:",
                  string_VkResult(result));
      return result;
    }
    for (auto &i : layerNames) {
      for (auto &j : available_layers)
        if (!strcmp(i, j.layerName))
          goto CHECK_FOUND;
      i = nullptr;
    CHECK_FOUND:
      continue;
    }
  } else
    for (auto &i : layerNames)
      i = nullptr;
  return VK_SUCCESS;
}
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
VkResult ContextBase::prepare_debugger() {
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
                     VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
                     VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT,
      .pfnUserCallback = DebugUtilsMessengerCallback};
  PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessenger =
      reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
          vkGetInstanceProcAddr(m_Instance, "vkCreateDebugUtilsMessengerEXT"));
  if (vkCreateDebugUtilsMessenger) {
    VkResult result = vkCreateDebugUtilsMessenger(
        m_Instance, &debugUtilsMessengerCreateInfo, nullptr, &m_Debugger);
    if (result)
      print_error("Context", "Failed to create debug messenger! Code:",
                  string_VkResult(result));
    return result;
  }
  print_error("Context", "Failed to get the function pointer of "
                         "vkCreateDebugUtilsMessengerEXT!");
  return VK_RESULT_MAX_ENUM;
}
CtxResult ContextBase::prepare_instance(InstanceCreateInfo &info) {
  uint32_t current_version = 0u;
  if (acquire_vkapi_version(current_version)) {
    print_error("Context", "acquire_vkapi_version failed!");
    return CtxResult::ACQUIRE_API_VERSION_FAILED;
  }
  if (current_version < info.m_MinApiVersion) {
    print_error("Context", "Vulkan API version too low!");
    return CtxResult::API_VERSION_TOO_LOW;
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

  if (info.m_isDebuging) {
    info.m_ExtensionNames.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    info.m_LayerNames.push_back("VK_LAYER_KHRONOS_validation");
  }

  {
    uint32_t extension_count = 0;
    const char **ppExtensionNames;
    ppExtensionNames = glfwGetRequiredInstanceExtensions(&extension_count);
    if (!ppExtensionNames) {
      print_error("Context", "Vulkan is not available on this "
                             "machine!");
      return CtxResult::ACQUIRE_GLFW_EXT_FAILED;
    }
    info.m_ExtensionNames.append_range(
        std::span<const char *>(ppExtensionNames, extension_count));
    // for (size_t i = 0; i < extensionCount; i++)
    //  info.m_ExtensionNames.push_back(extensionNames[i]);
  };

  if (VkResult result = check_instance_extension(info.m_ExtensionNames)) {
    print_error("Context", "check_instance_extension() failed! Code:",
                string_VkResult(result));
    return CtxResult::CHECK_EXT_FAILED;
  }
  if (VkResult result = check_instance_layer(info.m_LayerNames)) {
    print_error("Context", "check_instance_layer() failed! Code:",
                string_VkResult(result));
    return CtxResult::CHECK_LAYER_FAILED;
  }

  VkInstanceCreateInfo createInfo = {
      .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
      .pNext = info.m_pNextInstance,
      .flags = info.m_InstanceFlags,
      .pApplicationInfo = &app_info,
      .enabledLayerCount = uint32_t(info.m_LayerNames.size()),
      .ppEnabledLayerNames = info.m_LayerNames.data(),
      .enabledExtensionCount = uint32_t(info.m_ExtensionNames.size()),
      .ppEnabledExtensionNames = info.m_ExtensionNames.data()};

  if (VkResult result = vkCreateInstance(&createInfo, nullptr, &m_Instance)) {
    switch (result) {
    case VK_ERROR_LAYER_NOT_PRESENT:
    case VK_ERROR_EXTENSION_NOT_PRESENT:
      print_warning("Context",
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
            "Vulkan API Version:", VK_API_VERSION_MAJOR(m_VulkanApiVersion),
            VK_API_VERSION_MINOR(m_VulkanApiVersion),
            VK_API_VERSION_PATCH(m_VulkanApiVersion));
  if (info.m_isDebuging)
    if (VkResult result = prepare_debugger()) {
      print_error("Context",
                  "create debug failed! Code:", string_VkResult(result));
      return CtxResult::DEBUG_CREATE_FAILED;
    }
  return CtxResult::SUCCESS;
}
CtxResult ContextBase::prepare_glfw() {
  if (!glfwInit()) {
    print_error("Context", "Failed to initialize GLFW!");
    return CtxResult::INIT_GLFW_FAILED;
  }
  glfwSetErrorCallback([](int error_code, const char *description) {
    print_error("GLFW", "Error Code:", error_code, "; Desc:", description);
  });
  return CtxResult::SUCCESS;
}
CtxResult WindowContextBase::prepare_window(const WindowCreateInfo &info) {
  using State = WindowCreateState;
  if (info.m_InitState & State::use_primary_monitor) {
    m_pMonitor = glfwGetPrimaryMonitor();
    if (!m_pMonitor)
      return CtxResult::NO_MONITOR;
  } else {
    int monitor_count;
    GLFWmonitor **pMonitors = glfwGetMonitors(&monitor_count);
    if (!pMonitors) {
      print_error("Context", "No monitors!");
      return CtxResult::NO_MONITOR;
    }
    if (!info.m_MonitorChooseFunc) {
      return CtxResult::NO_MONITOR_CHOOSE_FUNCT;
    }
    for (int i = 0; i < monitor_count; i++) {
      if (info.m_MonitorChooseFunc(pMonitors[i])) {
        m_pMonitor = pMonitors[i];
        goto FINISH_CHOOSE;
      }
    }
    return CtxResult::NO_FIT_MONITOR;
  }
FINISH_CHOOSE:
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
    print_error("WindowContext", "Get Video Mode Failed!");
    return CtxResult::GET_VIDEO_MODE_FAILED;
  }
  if (size_state == State::full_screen) {
    m_pWindow = glfwCreateWindow(pMode->width, pMode->height, m_Title.c_str(),
                                 m_pMonitor, nullptr);
  } else if (size_state == State::maximized) {
    glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
    m_pWindow = glfwCreateWindow(pMode->width, pMode->height, m_Title.c_str(),
                                 nullptr, nullptr);
  } else if (size_state == State::specified) {
    m_pWindow = glfwCreateWindow(info.m_InitSizeX, info.m_InitSizeY,
                                 m_Title.c_str(), nullptr, nullptr);
  } else {
    m_pMonitor = nullptr;
    m_Title.clear();
    return CtxResult::WRONG_ARGUMENT;
  }
  if (!m_pWindow) {
    m_pMonitor = nullptr;
    m_Title.clear();
    return CtxResult::WINDOW_CREATE_FAILED;
  }
  if (info.m_InitPosX != (~0u) && info.m_InitPosY != (~0u))
    glfwSetWindowPos(m_pWindow, info.m_InitPosX, info.m_InitPosY);
  glfwSetWindowSizeLimits(m_pWindow, info.m_MinSizeX, info.m_MinSizeY,
                          info.m_MaxSizeX, info.m_MaxSizeY);
  glfwSetWindowUserPointer(m_pWindow, this);

  return CtxResult::SUCCESS;
}
VkResult ContextBase::acquire_physical_devices(
    std::vector<VkPhysicalDevice> &availablePhysicalDevices) {
  uint32_t device_count;
  if (VkResult result =
          vkEnumeratePhysicalDevices(m_Instance, &device_count, nullptr)) {
    print_error("Context", "Failed to get the count of physical devices! Code:",
                string_VkResult(result));
    return result;
  }
  if (!device_count) {
    print_error("Context",
                "Failed to find any physical device supports vulkan!");
    abort();
  }
  availablePhysicalDevices.resize(device_count);
  VkResult result = vkEnumeratePhysicalDevices(m_Instance, &device_count,
                                               availablePhysicalDevices.data());
  if (result)
    print_error("Context", "Failed to enumerate physical devices! Code:",
                string_VkResult(result));
  return result;
}
VkResult ContextBase::acquire_queue_family_indices(
    VkPhysicalDevice physicalDevice, uint32_t (&queueFamilyIndices)[3],
    std::span<WindowContext> windowData, bool enableGraphicsQueue,
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
    if (!windowData.empty()) {
      for (auto &window : windowData)
        if (VkResult result = vkGetPhysicalDeviceSurfaceSupportKHR(
                physicalDevice, i, window.m_Surface, &support_presentation)) {
          print_error("Context",
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
      // 如果不需要呈现，那么已经可以break了
      if (windowData.size() == 0)
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
      (ip == VK_QUEUE_FAMILY_IGNORED && windowData.size() > 0) ||
      (ic == VK_QUEUE_FAMILY_IGNORED && enableComputeQueue))
    return VK_RESULT_MAX_ENUM;
  m_QueueFamilyIndex_graphics = ig;
  m_QueueFamilyIndex_presentation = ip;
  m_QueueFamilyIndex_compute = ic;
  return VK_SUCCESS;
}
VkResult ContextBase::determine_physical_device(
    std::vector<VkPhysicalDevice> &availablePhysicalDevices,
    uint32_t deviceIndex, std::span<WindowContext> windowData,
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
      (ip == not_found && !windowData.empty()) ||
      (ic == not_found && enableComputeQueue))
    return VK_RESULT_MAX_ENUM;

  // 如果有任何队列族索引应被获取但还未被找过
  if ((ig == VK_QUEUE_FAMILY_IGNORED && enableGraphicsQueue) ||
      (ip == VK_QUEUE_FAMILY_IGNORED && !windowData.empty()) ||
      (ic == VK_QUEUE_FAMILY_IGNORED && enableComputeQueue)) {
    uint32_t indices[3];
    VkResult result = acquire_queue_family_indices(
        availablePhysicalDevices[deviceIndex], indices, windowData,
        enableGraphicsQueue, enableComputeQueue);
    // 若GetQueueFamilyIndices(...)返回VK_SUCCESS或VK_RESULT_MAX_ENUM（vkGetPhysicalDeviceSurfaceSupportKHR(...)执行成功但没找齐所需队列族），
    // 说明对所需队列族索引已有结论，保存结果到queueFamilyIndexCombinations[deviceIndex]中相应变量
    // 应被获取的索引若仍为VK_QUEUE_FAMILY_IGNORED，说明未找到相应队列族，VK_QUEUE_FAMILY_IGNORED（~0u）与INT32_MAX做位与得到的数值等于notFound
    if (result == VK_SUCCESS || result == VK_RESULT_MAX_ENUM) {
      if (enableGraphicsQueue)
        ig = indices[0] & INT32_MAX;
      if (!windowData.empty())
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
        (!windowData.empty()) ? ip : VK_QUEUE_FAMILY_IGNORED;
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
        if (m_VulkanApiVersion >=VK_API_VERSION_1_4)
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
ContextBase::prepare_physical_device(std::span<WindowContext> windowData) {
  std::vector<VkPhysicalDevice> available_physical_devices;
  if (acquire_physical_devices(available_physical_devices))
    return CtxResult::ACQUIRE_PHYSICAL_DEVICES_FAILED;
  for (uint32_t i = 0; i < available_physical_devices.size(); ++i)
    if (!determine_physical_device(available_physical_devices, i, windowData,
                                   true, true))
      goto FIND_SUCCESS;
  print_error("Context", "Can not find any phyDevice useable!");
  return CtxResult::NO_FIT_PHYDEVICE;
FIND_SUCCESS:
  acquire_physical_divice_properties();
  acquire_physical_divice_features();
  return CtxResult::SUCCESS;
}
VkResult ContextBase::acquire_device_extensions(
    std::vector<VkExtensionProperties> &extensionNames, const char *layerName) {
  uint32_t extCount;
  if (VkResult result = vkEnumerateDeviceExtensionProperties(
          m_PhysicalDevice, layerName, &extCount, nullptr)) {
    print_error("Context",
                "vkEnumerateDeviceExtensionProperties() failed! Code:",
                string_VkResult(result));
    return VK_RESULT_MAX_ENUM;
  }
  extensionNames.resize(extCount);
  if (VkResult result = vkEnumerateDeviceExtensionProperties(
          m_PhysicalDevice, layerName, &extCount, extensionNames.data())) {
    print_error("Context",
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
ContextBase::check_VMA_extensions(std::vector<const char *> &extensionNames) {
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
VkResult ContextBase::prepare_VMA(DeviceCreateInfo &info) {
  VmaAllocatorCreateInfo allocatorCreateInfo = {
      .flags = info.m_VmaFlags,
      .physicalDevice = m_PhysicalDevice,
      .device = m_Device,
      .instance = m_Instance,
      .vulkanApiVersion = m_VulkanApiVersion};
  return vmaCreateAllocator(&allocatorCreateInfo, &m_Allocator);
}
CtxResult ContextBase::prepare_device(DeviceCreateInfo &info) {
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
  //   设备扩展(设备没有层级，已经弃用):
  if (acquire_device_extensions(m_AvailableExtensions))
    return CtxResult::ACQUIRE_DEVICE_EXTENSIONS_FAILED;
  info.m_ExtensionNames.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
  info.m_VmaFlags = static_cast<VmaAllocatorCreateFlagBits>(
      info.m_VmaFlags | check_VMA_extensions(info.m_ExtensionNames));
  check_device_extension(info.m_ExtensionNames);
  m_AvailableExtensions.clear();
  m_Extensions.clear();
  [&info]() {
    auto &ref = info.m_ExtensionNames;
    int length = 0;
    for (int i = 0; i < ref.size(); ++i) {
      if (ref[i] == nullptr)
        ++length;
      else if (length > 0)
        ref[i - length] = ref[i];
    }
    ref.resize(ref.size() - length);
  }();
  // std::erase_if(info.m_ExtensionNames,
  //              [](const char *str) { return str == nullptr; });
  // 3.创建逻辑设备
  VkDeviceCreateInfo deviceCreateInfo = {
      .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
      .flags = info.m_DiviceFlags,
      .queueCreateInfoCount = queue_create_info_count,
      .pQueueCreateInfos = queue_create_infos,
      .enabledExtensionCount = uint32_t(info.m_ExtensionNames.size()),
      .ppEnabledExtensionNames = info.m_ExtensionNames.data()};
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
    print_error("Context",
                "Failed to create a vulkan logical device! "
                "Code: ",
                string_VkResult(result));
    if (last)
      last->pNext = nullptr;
    return CtxResult::CREATE_DEVICE_FAILED;
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
  if (prepare_VMA(info))
    return CtxResult::VMA_CREATE_FAILED;
  print_log("Context",
            "Renderer:", m_PhysicalDeviceProperties.properties.deviceName);
  return CtxResult::SUCCESS;
}
void ContextBase::update() {
  // 更新时间
  auto newtime = glfwGetTime();
  m_DeltaTime = newtime - m_CurrentTime;
  m_CurrentTime = newtime;
}
void WindowContextBase::cleanup() {
  if (m_pWindow)
    glfwDestroyWindow(m_pWindow);
  m_pWindow = nullptr;
  m_pMonitor = nullptr;
  m_Title.clear();
}
void WindowContext::cleanup(ContextBase &ctx) {
  if (m_Swapchain) {
    m_CallbackSwapchainDestroy.iterate(this);
    for (auto &i : m_SwapchainImageViews)
      if (i)
        vkDestroyImageView(ctx.m_Device, i, nullptr);
    vkDestroySwapchainKHR(ctx.m_Device, m_Swapchain, nullptr);
    m_SwapchainImages.clear();
    m_SwapchainImageViews.clear();
    m_Swapchain = VK_NULL_HANDLE;
    m_SwapchainCreateInfo = {};

    m_AvailableFormats.clear();
    m_CallbackSwapchainDestroy.clear();
    m_CallbackSwapchainConstruct.clear();
  }
  if (m_Surface) {
    vkDestroySurfaceKHR(ctx.m_Instance, m_Surface, nullptr);
    m_Surface = VK_NULL_HANDLE;
  }
  WindowContextBase::cleanup();
}
void ContextBase::cleanup() {
  if (!m_Instance)
    return;
  if (m_Device) {
    if (VkResult result = vkDeviceWaitIdle(m_Device))
      print_warning("Context", "cleanup device waitIdle failed! Code:",
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
  glfwTerminate();
}
void Context::cleanup() {
  for (auto &window : m_WindowData)
    window.cleanup(*this);
  m_WindowData.clear();
  ContextBase::cleanup();
}
VkResult WindowContext::prepare_surface(ContextBase &ctx) {
  VkSurfaceKHR surface = VK_NULL_HANDLE;
  if (VkResult result = glfwCreateWindowSurface(ctx.m_Instance, m_pWindow,
                                                nullptr, &surface)) {
    print_error("Context",
                "Failed to create a window "
                "surface! Code:",
                string_VkResult(result));
    return VK_RESULT_MAX_ENUM;
  }
  this->m_Surface = surface;
  return VK_SUCCESS;
}
CtxResult WindowContext::prepare_swapchain(const SwapchainCreateInfo info,
                                           ContextBase &ctx) {
  if (create_swapchain(info, ctx))
    return CtxResult::SWAPCHAIN_CREATE_FAILED;
  return CtxResult::SUCCESS;
}

VkResult WindowContext::create_swapchain_Internal(ContextBase &ctx) {
  auto &createInfo = m_SwapchainCreateInfo;
  // 直接创建交换链
  if (VkResult result = vkCreateSwapchainKHR(ctx.m_Device, &createInfo, nullptr,
                                             &m_Swapchain)) {
    print_error("WindowContext",
                "Failed to create a swapchain! "
                "Code:",
                string_VkResult(result));
    return result;
  }
  // 获取交换链图像
  uint32_t swapchainImageCount;
  if (VkResult result = vkGetSwapchainImagesKHR(
          ctx.m_Device, m_Swapchain, &swapchainImageCount, nullptr)) {
    print_error("WindowContext",
                "Failed to get the count of swapchain images! Code:",
                string_VkResult(result));
    return result;
  }
  m_SwapchainImages.resize(swapchainImageCount);
  if (VkResult result = vkGetSwapchainImagesKHR(ctx.m_Device, m_Swapchain,
                                                &swapchainImageCount,
                                                m_SwapchainImages.data())) {
    print_error("WindowContext", "Failed to get swapchain images! Code:",
                string_VkResult(result));
    return result;
  }
  // 直接创建交换链，并且获取交换链图像和视图
  m_SwapchainImageViews.resize(swapchainImageCount);
  VkImageViewCreateInfo imageViewCreateInfo = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
      .viewType = VK_IMAGE_VIEW_TYPE_2D,
      .format = createInfo.imageFormat,
      //.components = {}, // 四个成员皆为VK_COMPONENT_SWIZZLE_IDENTITY
      .subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}};
  for (size_t i = 0; i < swapchainImageCount; i++) {
    imageViewCreateInfo.image = m_SwapchainImages[i];
    if (VkResult result =
            vkCreateImageView(ctx.m_Device, &imageViewCreateInfo, nullptr,
                              &m_SwapchainImageViews[i])) {
      print_error("WindowContext",
                  "Failed to create a swapchain image view! Code:",
                  string_VkResult(result));
      return result;
    }
  }
  return VK_SUCCESS;
}
VkResult WindowContext::acquire_surface_formats(ContextBase &ctx) {
  uint32_t surfaceFormatCount;
  if (VkResult result = vkGetPhysicalDeviceSurfaceFormatsKHR(
          ctx.m_PhysicalDevice, m_Surface, &surfaceFormatCount, nullptr)) {
    print_error("WindowContext",
                "Failed to get the count of surface "
                "formats! Code:",
                string_VkResult(result));
    return result;
  }
  if (!surfaceFormatCount)
    print_error("WindowContext", "Failed to find any supported surface "
                                 "format!"),
        abort();
  m_AvailableFormats.resize(surfaceFormatCount);
  VkResult result = vkGetPhysicalDeviceSurfaceFormatsKHR(
      ctx.m_PhysicalDevice, m_Surface, &surfaceFormatCount,
      m_AvailableFormats.data());
  if (result)
    print_error("WindowContext",
                "Failed to get surface formats! "
                "Code:",
                string_VkResult(result));
  return VK_SUCCESS;
}
VkResult WindowContext::acquire_present_modes(
    std::vector<VkPresentModeKHR> &presentModes, ContextBase &ctx) {
  uint32_t surfacePresentModeCount;
  if (VkResult result = vkGetPhysicalDeviceSurfacePresentModesKHR(
          ctx.m_PhysicalDevice, m_Surface, &surfacePresentModeCount, nullptr)) {
    print_error("WindowContext",
                "Failed to get the count of surface present modes! Code:",
                string_VkResult(result));
    return result;
  }
  if (!surfacePresentModeCount) {
    print_error("WindowContext", "Failed to find any surface present mode!");
    abort();
  }
  presentModes.resize(surfacePresentModeCount);
  if (VkResult result = vkGetPhysicalDeviceSurfacePresentModesKHR(
          ctx.m_PhysicalDevice, m_Surface, &surfacePresentModeCount,
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
                                           ContextBase &ctx) {
  bool formatIsAvailable = false;
  if (!surfaceFormat.format) {
    // 如果格式未指定，只匹配色彩空间，图像格式有啥就用啥
    for (auto &i : m_AvailableFormats)
      if (i.colorSpace == surfaceFormat.colorSpace) {
        m_SwapchainCreateInfo.imageFormat = i.format;
        m_SwapchainCreateInfo.imageColorSpace = i.colorSpace;
        formatIsAvailable = true;
        break;
      }
  } else
    // 否则匹配格式和色彩空间
    for (auto &i : m_AvailableFormats)
      if (i.format == surfaceFormat.format &&
          i.colorSpace == surfaceFormat.colorSpace) {
        m_SwapchainCreateInfo.imageFormat = i.format;
        m_SwapchainCreateInfo.imageColorSpace = i.colorSpace;
        formatIsAvailable = true;
        break;
      }
  // 如果没有符合的格式, 返回错误
  if (!formatIsAvailable)
    return VK_ERROR_FORMAT_NOT_SUPPORTED;
  // 如果交换链已存在，调用RecreateSwapchain()重建交换链
  if (m_Swapchain)
    return recreate_swapchain(ctx);
  return VK_SUCCESS;
}
VkResult WindowContext::create_swapchain(const SwapchainCreateInfo &info,
                                         ContextBase &ctx) {
  VkSurfaceCapabilitiesKHR surface_capabilities;
  // 获取surface支持能力
  if (VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
          ctx.m_PhysicalDevice, m_Surface, &surface_capabilities)) {
    print_error("WindowContext",
                "Failed to get physical device surface capabilities! Code:",
                string_VkResult(result));
    return result;
  }
  auto &createInfo = m_SwapchainCreateInfo;
  // 如果容许的最大数量与最小数量不等，那么使用最小数量+1
  createInfo.minImageCount =
      surface_capabilities.minImageCount +
      (surface_capabilities.maxImageCount > surface_capabilities.minImageCount);
  // 决定窗口大小
  uint32_t width, height;
  glfwGetWindowSize(m_pWindow, (int *)&width, (int *)&height);
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
  if (m_AvailableFormats.empty())
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
      createInfo.imageFormat = m_AvailableFormats[0].format;
      createInfo.imageColorSpace = m_AvailableFormats[0].colorSpace;
      print_warning("WindowContext",
                    "Failed to select a four-component UNORM surface format!");
    }
  // 指定呈现模式
  std::vector<VkPresentModeKHR> surfacePresentModes;
  if (VkResult result = acquire_present_modes(surfacePresentModes, ctx)) {
    return result;
  }
  createInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
  if (!info.m_isFrameRateLimited)
    for (size_t i = 0; i < surfacePresentModes.size(); i++)
      if (surfacePresentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
        createInfo.presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
        break;
      }
  const char *mode_str = string_VkPresentModeKHR(createInfo.presentMode);
  print_log("Present Mode", mode_str);
  // ----------------
  createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  createInfo.flags = info.m_flags;
  createInfo.surface = m_Surface;
  createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  createInfo.clipped = VK_TRUE;
  createInfo.oldSwapchain = VK_NULL_HANDLE;
  createInfo.pNext = nullptr;
  // ----------------
  if (VkResult result = create_swapchain_Internal(ctx))
    return result;
  m_CallbackSwapchainConstruct.iterate(this);
  return VK_SUCCESS;
}
VkResult WindowContext::recreate_swapchain(ContextBase &ctx) {
  auto &createInfo = m_SwapchainCreateInfo;
  VkSurfaceCapabilitiesKHR surface_capabilities = {};
  VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
      ctx.m_PhysicalDevice, m_Surface, &surface_capabilities);
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
  createInfo.oldSwapchain = m_Swapchain;
  result = vkQueueWaitIdle(ctx.m_Queue_graphics);
  // 仅在等待图形队列成功，且图形与呈现所用队列不同时等待呈现队列
  if (!result && ctx.m_Queue_graphics != ctx.m_Queue_presentation)
    result = vkQueueWaitIdle(ctx.m_Queue_presentation);
  if (result) {
    print_error("WindowContext",
                "Failed to wait for the queue to be idle! Code:",
                string_VkResult(result));
    return result;
  }
  m_CallbackSwapchainDestroy.iterate(this);
  for (auto &i : m_SwapchainImageViews)
    if (i)
      vkDestroyImageView(ctx.m_Device, i, nullptr);
  m_SwapchainImageViews.resize(0);
  result = create_swapchain_Internal(ctx);
  if (result != VK_SUCCESS) {
    print_error("WindowContext",
                "Create swapchain failed! Code:", string_VkResult(result));
    return result;
  }
  m_CallbackSwapchainConstruct.iterate(this);
  print_log("WindowContext", "Swapchain recreated!");
  return VK_SUCCESS;
}
CtxResult Context::prepare_context(ContextCreateInfo &info,
                                   std::span<WindowContext *> ret) {
  CtxResult result;
  if (result = prepare_glfw(); result != CtxResult::SUCCESS)
    return result;
  if (result = prepare_instance(*info.m_InstanceInfo);
      result != CtxResult::SUCCESS)
    return result;
  auto &list = info.m_WindowInfo;
  for (size_t i = 0; i < list.size(); ++i) {
    if (result = create_window(list[i].first, ret[i]);
        result != CtxResult::SUCCESS)
      return result;
    if (ret[i]->prepare_surface(*this))
      return CtxResult::SURFACE_ACQUIRE_FAILED;
  }
  if (result = prepare_physical_device(m_WindowData);
      result != CtxResult::SUCCESS)
    return result;
  if (result = prepare_device(*info.m_DeviceInfo); result != CtxResult::SUCCESS)
    return result;
  for (size_t i = 0; i < list.size(); ++i)
    if (result = ret[i]->prepare_swapchain(list[i].second, *this);
        result != CtxResult::SUCCESS)
      return result;
  return result;
}
CtxResult Context::create_window(const WindowCreateInfo &info,
                                 WindowContext *&ret) {
  auto &window = m_WindowData.emplace_back();
  if (CtxResult result = window.prepare_window(info);
      result != CtxResult::SUCCESS) {
    ret = nullptr;
    return result;
  }
  ret = &window;
  return CtxResult::SUCCESS;
}
} // namespace BL
