#include "bl_vkcontext.hpp"

namespace BL {
thread_local static ContextThreadData local_data;
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
}  // namespace BL
