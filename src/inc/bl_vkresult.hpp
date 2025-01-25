#ifndef _BL_VKRESULT_HPP_FILE_
#define _BL_VKRESULT_HPP_FILE_
#include <vulkan/vulkan.h>
#include <system_error>
namespace BL {
using VulkanErrorEnum = VkResult;
class VulkanErrorCategory : public std::error_category {
   public:
    VulkanErrorCategory() {}
    [[nodiscard]]
    const char* name() const noexcept override {
        return "Vulkan error";
    }
    [[nodiscard]]
    std::string message(int ev) const override;
};
[[nodiscard]]
std::error_code make_error_code(VulkanErrorEnum e);
}  // namespace BL
namespace std {
template <>
struct is_error_code_enum<BL::VulkanErrorEnum> : public true_type {};
}  // namespace std
namespace BL
{
    
} // namespace BL

#endif  //!_BL_VKRESULT_HPP_FILE_