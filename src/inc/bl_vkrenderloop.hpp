#ifndef _BL_VK_RENDERLOOP_HPP_FILE_
#define _BL_VK_RENDERLOOP_HPP_FILE_
#include <bl_vkcontext.hpp>
#include <bl_vktypes.hpp>
#include <bl_window.hpp>

namespace BL {
struct RenderLoopErrorEnum_t {
    enum Type { Success = 0 };
};
using RenderLoopErrorEnum = RenderLoopErrorEnum_t::Type;
class RenderLoopErrorCategory : public std::error_category {
   public:
    RenderLoopErrorCategory() {}
    [[nodiscard]]
    const char* name() const noexcept override;
    [[nodiscard]]
    std::string message(int ev) const override;
};
[[nodiscard]]
std::error_code make_error_code(RenderLoopErrorEnum e);
}  // namespace BL
namespace std {
template <>
struct is_error_code_enum<BL::RenderLoopErrorEnum> : public true_type {};
}  // namespace std
namespace BL {
struct RenderLoopInit {
    WindowContext* windowContext;
    uint32_t renderPassCount{1};
    bool force_ownership_transfer{false};
};
struct RenderLoop {
    WindowContext* windowContext;
    std::array<Fence, MAX_FLIGHT_COUNT> fences; // 在渲染完成时置位，初始为置位状态
    std::array<Semaphore, MAX_FLIGHT_COUNT> semsOwnershipIsTransfered; // 在渲染和呈现之间执行可能需要的队列所有权转移
    std::vector<CommandBuffer> cmdBuffers; // 每帧（maxImageCount帧）每个环节（maxRenderPassCount轮）的命令缓冲
    std::array<CommandBuffer, MAX_FLIGHT_COUNT> cmdBuffer_presentation; // 呈现操作的命令缓冲
    std::vector<Semaphore> semaphores; // 每帧每个环节（maxRenderPassCount+1轮）的信号量
    /*
    每个环节使用的信号量：(Sn为索引n的信号量)
    等待S0->环节0--S1->环节1--S2->...->环节(maxRenderPassCount-1)--S(maxRenderPassCount)->呈现->置位S0
    */

    CommandPool cmdPool_graphics;
    CommandPool cmdPool_compute;
    CommandPool cmdPool_presentation;
    uint32_t image_index; // 当前交换链图像索引
    uint32_t maxRenderPassCount; // 最大渲染环节数
    uint32_t maxImageCount; // 交换链图像数
    uint32_t currentRenderPass; // 当前渲染环节，从0开始
    uint32_t curFrame; // 当前使用的 inflight 索引
    bool ownership_transfer;

    explicit RenderLoop(const RenderLoopInit* pInit, std::error_code& ec) {
        ec = create(pInit);
    }
    std::error_code create(const RenderLoopInit* pInit);
    void destroy();
    VkCommandBuffer begin_render();
    VkCommandBuffer next_render_pass();
    void end_render();
    ~RenderLoop() {}
   private:
    VkResult _present_image(VkPresentInfoKHR& presentInfo);
    VkResult _present_image_semaphore(VkSemaphore semaphore_renderingIsOver = VK_NULL_HANDLE)
    VkResult _acquire_next_image(uint32_t* index,
                                VkSemaphore semsImageAvaliable = VK_NULL_HANDLE,
                                VkFence fence = VK_NULL_HANDLE);
};
}  // namespace BL
#endif  //!_BL_VK_RENDERLOOP_HPP_FILE_