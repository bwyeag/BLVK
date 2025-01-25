#include <bl_vkcontext.hpp>
#include <bl_window.hpp>

BL::Context ctx;
int main() {
    {
        if (BL::ContextInstanceInit ctxinit{
                .pAppName = "BLVK test",
                .appVersion = VK_MAKE_API_VERSION(0, 0, 1, 1),
                .minApiVersion = VK_API_VERSION_1_3,
                .isDebuging = true,
            };
            ctx.create_instance(&ctxinit)) {
            exit(-1);
        }
        BL::make_current_context(ctx);
    }
}