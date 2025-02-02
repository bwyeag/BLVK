#ifndef _BL_RENDERLOOP_HPP_FILE_
#define _BL_RENDERLOOP_HPP_FILE_
#include <bl_vktypes.hpp>
#include <core/bl_constant.hpp>
#include <core/bl_init.hpp>
namespace BL {
enum class RenderLoopResult { SUCCESS = 0, INITIALIZE_FAILED = -1 };
struct RenderPassWithFramebuffers {
    RenderPass renderPass;
    std::vector<Framebuffer> framebuffers;
};
struct RenderLoopInfo {
    WindowContext* windowContext;
    uint32_t renderPassCount{1};
    bool force_ownership_transfer{false};
};
struct RenderLoop {
    WindowContext* windowContext;
    std::array<Fence, MAX_FLIGHT_COUNT>
        fences;  // 在渲染完成时置位，初始为置位状态
    std::array<Semaphore, MAX_FLIGHT_COUNT>
        semsOwnershipIsTransfered;  // 在渲染和呈现之间执行可能需要的队列所有权转移
    std::vector<CommandBuffer>
        cmdBuffers;  // 每帧（maxImageCount帧）每个环节（maxRenderPassCount轮）的命令缓冲
    std::array<CommandBuffer, MAX_FLIGHT_COUNT>
        cmdBuffer_presentation;  // 呈现操作的命令缓冲
    std::vector<Semaphore>
        semaphores;  // 每帧每个环节（maxRenderPassCount+1轮）的信号量
    /*
    每个环节使用的信号量：(Sn为索引n的信号量, Fk为索引k的栅栏)
    等待Fi->等待S0(由vkAcquireNextImageKHR()置位)->环节0--S1->环节1--S2->...->环节(maxRenderPassCount-1)--S(maxRenderPassCount)->呈现->置位Fi
    */

    CommandPool cmdPool_graphics;
    CommandPool cmdPool_compute;
    CommandPool cmdPool_presentation;
    uint32_t image_index;         // 当前交换链图像索引
    uint32_t maxRenderPassCount;  // 最大渲染环节数
    uint32_t maxImageCount;       // 交换链图像数
    uint32_t curRenderPass;       // 当前渲染环节，从0开始
    uint32_t curFrame;            // 当前使用的 inflight 索引
    // uint32_t curQueue;            // 当前使用的队列族, 自动做队列族所有权交换
    bool ownership_transfer;

    RenderLoopResult prepare(const RenderLoopInfo pInit);
    void cleanup() noexcept;
    ~RenderLoop() {}

    VkCommandBuffer begin_render();
    VkCommandBuffer next_render_pass();
    void end_render();
    void present();

   protected:
    VkResult present_image(VkPresentInfoKHR& presentInfo);
    VkResult present_image_semaphore(
        VkSemaphore semaphore_renderingIsOver = VK_NULL_HANDLE);
    VkResult acquire_next_image(uint32_t* index,
                                VkSemaphore semsImageAvaliable = VK_NULL_HANDLE,
                                VkFence fence = VK_NULL_HANDLE);
    void cmd_transfer_image_ownership(VkCommandBuffer commandBuffer);
    VkResult submit_cmdbuffer_presentation(
        VkCommandBuffer commandBuffer,
        VkSemaphore semaphore_renderingIsOver = VK_NULL_HANDLE,
        VkSemaphore semaphore_ownershipIsTransfered = VK_NULL_HANDLE,
        VkFence fence = VK_NULL_HANDLE);
};
VkCommandBuffer reset_and_begin_cmdbuffer(
    CommandBuffer& cmdBuf,
    VkCommandBufferResetFlags resetflags,
    VkCommandBufferUsageFlags cmdbufusage);
}  // namespace BL
#endif  //!_BL_RENDERLOOP_HPP_FILE_