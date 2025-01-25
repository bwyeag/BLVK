#ifndef _BOUNDLESS_VKCONTEXT_HPP_FILE_
#define _BOUNDLESS_VKCONTEXT_HPP_FILE_
#include <functional>
#include <system_error>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "vk_mem_alloc.h"
namespace BL {
struct ContextErrorEnum_t {
    enum Type { Success = 0 };
};
using ContextErrorEnum = ContextErrorEnum_t::Type;
class ContextErrorCategory : public std::error_category {
   public:
    ContextErrorCategory() {}
    [[nodiscard]]
    const char* name() const noexcept override {
        return "Vulkan Context error";
    }
    [[nodiscard]]
    std::string message(int ev) const override;
};
[[nodiscard]]
std::error_code make_error_code(ContextErrorEnum e);
}  // namespace BL
namespace std {
template <>
struct is_error_code_enum<BL::ContextErrorEnum> : public true_type {};
}  // namespace std
namespace BL {
constexpr uint32_t MAX_FLIGHT_COUNT = 3;
struct ContextInstanceInit {
    const char* pAppName = nullptr;
    uint32_t appVersion = 0u;
    uint32_t minApiVersion;
    bool isDebuging = false;
    const char** ppLayerNames = nullptr;
    uint32_t layerCount = 0u;
    const char** ppExtensionNames = nullptr;
    uint32_t extensionCount = 0u;
};
struct ContextDeviceInit {
    std::function<bool(VkPhysicalDevice)> isDeviceSuitable;
    uint32_t surfaceCount;
    VkSurfaceKHR* surface;
    VmaAllocatorCreateFlags vmaFlags;
    bool debug_print_deviceExtension;
};
struct Context {
    uint32_t vulkanApiVersion;
    VkInstance instance = VK_NULL_HANDLE;
    VkPhysicalDevice phyDevice = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;
    uint32_t queueFamilyIndex_graphics = VK_QUEUE_FAMILY_IGNORED;
    uint32_t queueFamilyIndex_compute = VK_QUEUE_FAMILY_IGNORED;
    uint32_t queueFamilyIndex_presentation = VK_QUEUE_FAMILY_IGNORED;
    VkQueue queue_graphics = VK_NULL_HANDLE;
    VkQueue queue_compute = VK_NULL_HANDLE;
    VkQueue queue_presentation = VK_NULL_HANDLE;

    VkPhysicalDeviceProperties2 phyDeviceProperties;
    VkPhysicalDeviceVulkan11Properties phyDeviceVulkan11Properties;
    VkPhysicalDeviceVulkan12Properties phyDeviceVulkan12Properties;
    VkPhysicalDeviceVulkan13Properties phyDeviceVulkan13Properties;

    VkPhysicalDeviceMemoryProperties2 phyDeviceMemoryProperties;

    VkPhysicalDeviceFeatures2 phyDeviceFeatures;
    VkPhysicalDeviceVulkan11Features phyDeviceVulkan11Features;
    VkPhysicalDeviceVulkan12Features phyDeviceVulkan12Features;
    VkPhysicalDeviceVulkan13Features phyDeviceVulkan13Features;
#ifdef BL_DEBUG
    VkDebugUtilsMessengerEXT debugger;
#endif  // BL_DEBUG
    VmaAllocator allocator;

    Context() = default;
    Context(Context&& other) = delete;
    ~Context() {}
};
// 线程本地数据，方便获取上下文
struct ContextThreadData {
    Context* current_vkcontext = nullptr;
};
thread_local extern ContextThreadData local_data{};
inline Context& cur_context() {
    return *local_data.current_vkcontext;
}
inline void set_context(Context& ctx) {
    local_data.current_vkcontext = &ctx;
}
}  // namespace BL

#endif  //!_BOUNDLESS_VKCONTEXT_HPP_FILE_