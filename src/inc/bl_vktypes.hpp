#ifndef BOUNDLESS_TYPES_FILE
#define BOUNDLESS_TYPES_FILE
#include <vulkan/vulkan.h>
#include <bl_output.hpp>
#include <core/bl_constant.hpp>
#include <core/bl_init.hpp>
#include <cstdint>
#include <cstring>
#ifdef _MSC_VER_  // for MSVC
#define forceinline __forceinline
#elif defined __GNUC__  // for gcc on Linux/Apple OS X
#define forceinline inline __attribute__((always_inline))
#else
#define forceinline
#endif

namespace BL {
/*
 * Vulkan类型封装
 */
// todo: OcclusionQueries 修改（waitall()）
// todo: static 变量修改, 加入到.cpp中

constexpr VkFenceCreateInfo make_fence_createinfo(
    VkFenceCreateFlags flags = 0) {
    return {.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO, .flags = flags};
}
class Fence {
    VkFence handle = VK_NULL_HANDLE;

   public:
    forceinline Fence() = default;
    forceinline Fence(VkFenceCreateInfo& createInfo) { create(createInfo); }
    forceinline Fence(VkFenceCreateFlags flags) { create(flags); }
    forceinline Fence(Fence&& other) noexcept {
        handle = other.handle;
        other.handle = VK_NULL_HANDLE;
    }
    forceinline ~Fence() {
        if (handle)
            vkDestroyFence(cur_context().device, handle, nullptr);
        handle = VK_NULL_HANDLE;
    }
    forceinline operator VkFence() { return handle; }
    forceinline VkFence* getPointer() { return &handle; }
    forceinline VkResult wait(uint64_t time = UINT64_MAX) const {
        VkResult result =
            vkWaitForFences(cur_context().device, 1, &handle, false, time);
        if (result)
            print_error("Fence", "Failed to wait for the fence! Code:",
                        string_VkResult(result));
        return result;
    }
    forceinline VkResult reset() const {
        VkResult result = vkResetFences(cur_context().device, 1, &handle);
        if (result)
            print_error("Fence", "Failed to reset for the fence! Code:",
                        string_VkResult(result));
        return result;
    }
    forceinline VkResult wait_and_reset(uint64_t time = UINT64_MAX) const {
        VkResult result = wait(time);
        result || (result = reset());
        return result;
    }
    forceinline VkResult status() const {
        VkResult result = vkGetFenceStatus(cur_context().device, handle);
        if (result <
            0)  // vkGetFenceStatus(...)成功时有两种结果，所以不能仅仅判断result是否非0
            print_error("Fence", "Failed to get the status of the fence! Code:",
                        string_VkResult(result));
        return result;
    }
    forceinline VkResult create(VkFenceCreateInfo& createInfo) {
        VkResult result =
            vkCreateFence(cur_context().device, &createInfo, nullptr, &handle);
        if (result)
            print_error("Fence", "Failed to create a fence! Code:",
                        string_VkResult(result));
        return result;
    }
    forceinline VkResult create(VkFenceCreateFlags flags = 0) {
        VkFenceCreateInfo createInfo = {
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO, .flags = flags};
        return create(createInfo);
    }
};
forceinline void make_fences(Fence* begin,
                             Fence* end,
                             VkFenceCreateFlags flags = 0) {
    VkFenceCreateInfo ci = make_fence_createinfo(flags);
    for (; begin < end; ++begin) {
        begin->create(ci);
    }
}
constexpr forceinline VkSemaphoreCreateInfo make_semaphore_createinfo() {
    return {.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
}
class Semaphore {
    VkSemaphore handle = VK_NULL_HANDLE;

   public:
    forceinline Semaphore() = default;
    forceinline Semaphore(VkSemaphoreCreateInfo& createInfo) {
        create(createInfo);
    }
    forceinline Semaphore(Semaphore&& other) noexcept {
        handle = other.handle;
        other.handle = VK_NULL_HANDLE;
    }
    forceinline ~Semaphore() {
        if (handle)
            vkDestroySemaphore(cur_context().device, handle, nullptr);
        handle = VK_NULL_HANDLE;
    }
    forceinline operator VkSemaphore() { return handle; }
    forceinline VkSemaphore* getPointer() { return &handle; }
    forceinline VkResult create(VkSemaphoreCreateInfo& createInfo) {
        VkResult result = vkCreateSemaphore(cur_context().device, &createInfo,
                                            nullptr, &handle);
        if (result)
            print_error("Semaphore", "Failed to create a semaphore! Code:",
                        string_VkResult(result));
        return result;
    }
    forceinline VkResult create() {
        VkSemaphoreCreateInfo createInfo = {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
        return create(createInfo);
    }
};
forceinline void make_semaphores(Semaphore* begin, Semaphore* end) {
    VkSemaphoreCreateInfo ci = make_semaphore_createinfo();
    for (; begin < end; ++begin) {
        begin->create(ci);
    }
}
class CommandBuffer {
    friend class CommandPool;
    VkCommandBuffer handle = VK_NULL_HANDLE;

   public:
    forceinline CommandBuffer() = default;
    forceinline CommandBuffer(CommandBuffer&& other) noexcept {
        handle = other.handle;
        other.handle = VK_NULL_HANDLE;
    }
    forceinline operator VkCommandBuffer() { return handle; }
    forceinline VkCommandBuffer* getPointer() { return &handle; }
    forceinline VkResult
    begin(VkCommandBufferUsageFlags usageFlags,
          VkCommandBufferInheritanceInfo& inheritanceInfo) {
        VkCommandBufferBeginInfo beginInfo = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .flags = usageFlags,
            .pInheritanceInfo = &inheritanceInfo};
        return vkBeginCommandBuffer(handle, &beginInfo);
    }
    forceinline VkResult begin(VkCommandBufferUsageFlags usageFlags = 0) {
        VkCommandBufferBeginInfo beginInfo = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .flags = usageFlags,
        };
        return vkBeginCommandBuffer(handle, &beginInfo);
    }
    forceinline VkResult end() { return vkEndCommandBuffer(handle); }
    forceinline VkResult reset(VkCommandBufferResetFlags flags = 0) {
        return vkResetCommandBuffer(handle, flags);
    }
};
forceinline VkCommandPoolCreateInfo
make_commandpool_createinfo(uint32_t queueFamilyIndex,
                            VkCommandPoolCreateFlags flags = 0) {
    return {.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .flags = flags,
            .queueFamilyIndex = queueFamilyIndex};
}
class CommandPool {
    VkCommandPool handle = VK_NULL_HANDLE;

   public:
    forceinline CommandPool() = default;
    forceinline CommandPool(VkCommandPoolCreateInfo& createInfo) {
        create(createInfo);
    }
    forceinline CommandPool(uint32_t queueFamilyIndex,
                            VkCommandPoolCreateFlags flags = 0) {
        create(queueFamilyIndex, flags);
    }
    forceinline CommandPool(CommandPool&& other) noexcept {
        handle = other.handle;
        other.handle = VK_NULL_HANDLE;
    }
    forceinline ~CommandPool() {
        if (handle)
            vkDestroyCommandPool(cur_context().device, handle, nullptr);
        handle = VK_NULL_HANDLE;
    }
    forceinline operator VkCommandPool() { return handle; }
    forceinline VkCommandPool* getPointer() { return &handle; }
    VkResult allocate_buffer(
        CommandBuffer* pBuffer,
        VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY) const {
        VkCommandBufferAllocateInfo allocateInfo = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .commandPool = handle,
            .level = level,
            .commandBufferCount = 1};
        VkResult result = vkAllocateCommandBuffers(
            cur_context().device, &allocateInfo, (VkCommandBuffer*)pBuffer);
        if (result) {
            print_error("CommandPool", "Failed to allocate", 1,
                        "command buffer(s)! Code:", string_VkResult(result));
        }
        return result;
    }
    forceinline VkResult allocate_buffers(
        CommandBuffer* pBuffers,
        uint32_t count,
        VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY) const {
        VkCommandBufferAllocateInfo allocateInfo = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .commandPool = handle,
            .level = level,
            .commandBufferCount = count};
        VkResult result = vkAllocateCommandBuffers(
            cur_context().device, &allocateInfo, (VkCommandBuffer*)pBuffers);
        if (result) {
            print_error("CommandPool", "Failed to allocate", count,
                        "command buffer(s)! Code:", string_VkResult(result));
        }
        return result;
    }
    forceinline void free_buffer(CommandBuffer* pBuffer) const {
        vkFreeCommandBuffers(cur_context().device, handle, 1,
                             (VkCommandBuffer*)pBuffer);
        pBuffer->handle = VK_NULL_HANDLE;
    }
    forceinline void free_buffers(CommandBuffer* pBuffers,
                                  uint32_t count) const {
        vkFreeCommandBuffers(cur_context().device, handle, count,
                             (VkCommandBuffer*)pBuffers);
        std::memset((void*)pBuffers, 0, sizeof(VkCommandBuffer) * count);
    }
    forceinline VkResult create(VkCommandPoolCreateInfo& createInfo) {
        VkResult result = vkCreateCommandPool(cur_context().device, &createInfo,
                                              nullptr, &handle);
        if (result) {
            print_error("commandPool", "Failed to create a command pool! Code:",
                        string_VkResult(result));
        }
        return result;
    }
    forceinline VkResult create(uint32_t queueFamilyIndex,
                                VkCommandPoolCreateFlags flags = 0) {
        VkCommandPoolCreateInfo createInfo = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .flags = flags,
            .queueFamilyIndex = queueFamilyIndex};
        return create(createInfo);
    }
};
class RenderPass {
    VkRenderPass handle = VK_NULL_HANDLE;

   public:
    forceinline RenderPass() = default;
    forceinline RenderPass(VkRenderPassCreateInfo& createInfo) {
        create(createInfo);
    }
    forceinline RenderPass(RenderPass&& other) noexcept {
        handle = other.handle;
        other.handle = VK_NULL_HANDLE;
    }
    ~RenderPass() { destroy(); }
    forceinline operator VkRenderPass() { return handle; }
    forceinline VkRenderPass* getPointer() { return &handle; }
    forceinline void destroy() {
        if (handle)
            vkDestroyRenderPass(cur_context().device, handle, nullptr);
        handle = VK_NULL_HANDLE;
    }
    forceinline void cmd_begin(
        VkCommandBuffer cmdBuf,
        VkRenderPassBeginInfo& beginInfo,
        VkSubpassContents subpassContents = VK_SUBPASS_CONTENTS_INLINE) const {
        beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        beginInfo.renderPass = handle;
        vkCmdBeginRenderPass(cmdBuf, &beginInfo, subpassContents);
    }
    forceinline void cmd_begin(
        VkCommandBuffer cmdBuf,
        VkFramebuffer framebuffer,
        VkRect2D renderArea,
        const VkClearValue* clearValues = nullptr,
        uint32_t clearValuesCount = 0,
        VkSubpassContents subpassContents = VK_SUBPASS_CONTENTS_INLINE) const {
        VkRenderPassBeginInfo beginInfo = {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .renderPass = handle,
            .framebuffer = framebuffer,
            .renderArea = renderArea,
            .clearValueCount = clearValuesCount,
            .pClearValues = clearValues};
        vkCmdBeginRenderPass(cmdBuf, &beginInfo, subpassContents);
    }
    forceinline void cmd_next(
        VkCommandBuffer cmdBuf,
        VkSubpassContents subpassContents = VK_SUBPASS_CONTENTS_INLINE) const {
        vkCmdNextSubpass(cmdBuf, subpassContents);
    }
    forceinline void cmd_end(VkCommandBuffer cmdBuf) const {
        vkCmdEndRenderPass(cmdBuf);
    }
    forceinline VkResult create(VkRenderPassCreateInfo& createInfo) {
        VkResult result = vkCreateRenderPass(cur_context().device, &createInfo,
                                             nullptr, &handle);
        if (result) {
            print_error("renderPass", "Failed to create a render pass! Code:",
                        string_VkResult(result));
        }
        return result;
    }
};
class Framebuffer {
    VkFramebuffer handle = VK_NULL_HANDLE;

   public:
    forceinline Framebuffer() = default;
    forceinline Framebuffer(VkFramebufferCreateInfo& createInfo) {
        create(createInfo);
    }
    forceinline Framebuffer(Framebuffer&& other) noexcept {
        handle = other.handle;
        other.handle = VK_NULL_HANDLE;
    }
    forceinline ~Framebuffer() { destroy(); }
    forceinline operator VkFramebuffer() { return handle; }
    forceinline VkFramebuffer* getPointer() { return &handle; }
    forceinline VkResult create(VkFramebufferCreateInfo& createInfo) {
        VkResult result = vkCreateFramebuffer(cur_context().device, &createInfo,
                                              nullptr, &handle);
        if (result) {
            print_error("framebuffer", "Failed to create a framebuffer Code:",
                        string_VkResult(result));
        }
        return result;
    }
    forceinline void destroy() {
        if (handle)
            vkDestroyFramebuffer(cur_context().device, handle, nullptr);
        handle = VK_NULL_HANDLE;
    }
};
struct PipelineCreateInfosPack {
    VkGraphicsPipelineCreateInfo createInfo = {
        VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};
    std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
    // 顶点输入
    VkPipelineVertexInputStateCreateInfo vertexInputStateCi = {
        VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};
    std::vector<VkVertexInputBindingDescription> vertexInputBindings;
    std::vector<VkVertexInputAttributeDescription> vertexInputAttributes;
    // 顶点汇编
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCi = {
        VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};
    // 细分着色器状态
    VkPipelineTessellationStateCreateInfo tessellationStateCi = {
        VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO};
    // 视口
    VkPipelineViewportStateCreateInfo viewportStateCi = {
        VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO};
    std::vector<VkViewport> viewports;
    std::vector<VkRect2D> scissors;
    // 动态视口/剪裁不会用到上述的vector，因此手动指定动态视口和剪裁的个数
    uint32_t dynamicViewportCount = 1;
    uint32_t dynamicScissorCount = 1;
    // 光栅化
    VkPipelineRasterizationStateCreateInfo rasterizationStateCi = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};
    // 多重采样
    VkPipelineMultisampleStateCreateInfo multisampleStateCi = {
        VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO};
    // 深度 & 剪裁
    VkPipelineDepthStencilStateCreateInfo depthStencilStateCi = {
        VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO};
    // 颜色融混
    VkPipelineColorBlendStateCreateInfo colorBlendStateCi = {
        VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO};
    std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachmentStates;
    // 动态管线状态
    VkPipelineDynamicStateCreateInfo dynamicStateCi = {
        VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO};
    std::vector<VkDynamicState> dynamicStates;
    //-------------------------------------------------------------------------
    forceinline PipelineCreateInfosPack() {
        set_create_infos();
        createInfo.basePipelineIndex = -1;
    }
    forceinline PipelineCreateInfosPack(
        const PipelineCreateInfosPack& other) noexcept {
        createInfo.sType = other.createInfo.sType;
        createInfo.pNext = other.createInfo.pNext;
        createInfo.flags = other.createInfo.flags;
        createInfo.stageCount = other.createInfo.stageCount;
        createInfo.layout = other.createInfo.layout;
        createInfo.renderPass = other.createInfo.renderPass;
        createInfo.subpass = other.createInfo.subpass;
        createInfo.basePipelineHandle = other.createInfo.basePipelineHandle;
        createInfo.basePipelineIndex = other.createInfo.basePipelineIndex;

        vertexInputStateCi = other.vertexInputStateCi;
        inputAssemblyStateCi = other.inputAssemblyStateCi;
        tessellationStateCi = other.tessellationStateCi;
        viewportStateCi = other.viewportStateCi;
        rasterizationStateCi = other.rasterizationStateCi;
        multisampleStateCi = other.multisampleStateCi;
        depthStencilStateCi = other.depthStencilStateCi;
        colorBlendStateCi = other.colorBlendStateCi;
        dynamicStateCi = other.dynamicStateCi;
        set_create_infos();

        shaderStages = other.shaderStages;
        vertexInputBindings = other.vertexInputBindings;
        vertexInputAttributes = other.vertexInputAttributes;
        viewports = other.viewports;
        scissors = other.scissors;
        colorBlendAttachmentStates = other.colorBlendAttachmentStates;
        dynamicStates = other.dynamicStates;
        update_all_array_pointers();
    }
    forceinline operator VkGraphicsPipelineCreateInfo&() { return createInfo; }
    forceinline VkGraphicsPipelineCreateInfo* getPointer() {
        return &createInfo;
    }
    // 该函数用于将各个vector中数据的地址赋值给各个创建信息中相应成员，并相应改变各个count
    forceinline void update_all_arrays() {
        createInfo.stageCount = shaderStages.size();
        vertexInputStateCi.vertexBindingDescriptionCount =
            vertexInputBindings.size();
        vertexInputStateCi.vertexAttributeDescriptionCount =
            vertexInputAttributes.size();
        viewportStateCi.viewportCount = viewports.size()
                                            ? uint32_t(viewports.size())
                                            : dynamicViewportCount;
        viewportStateCi.scissorCount =
            scissors.size() ? uint32_t(scissors.size()) : dynamicScissorCount;
        colorBlendStateCi.attachmentCount = colorBlendAttachmentStates.size();
        dynamicStateCi.dynamicStateCount = dynamicStates.size();
        update_all_array_pointers();
    }

   private:
    // 将创建信息的地址赋值给basePipelineIndex中相应成员
    forceinline void set_create_infos() {
        createInfo.pVertexInputState = &vertexInputStateCi;
        createInfo.pInputAssemblyState = &inputAssemblyStateCi;
        createInfo.pTessellationState = &tessellationStateCi;
        createInfo.pViewportState = &viewportStateCi;
        createInfo.pRasterizationState = &rasterizationStateCi;
        createInfo.pMultisampleState = &multisampleStateCi;
        createInfo.pDepthStencilState = &depthStencilStateCi;
        createInfo.pColorBlendState = &colorBlendStateCi;
        createInfo.pDynamicState = &dynamicStateCi;
    }
    // 该将各个vector中数据的地址赋值给各个创建信息中相应成员，但不改变各个count
    forceinline void update_all_array_pointers() {
        createInfo.pStages = shaderStages.data();
        vertexInputStateCi.pVertexBindingDescriptions =
            vertexInputBindings.data();
        vertexInputStateCi.pVertexAttributeDescriptions =
            vertexInputAttributes.data();
        viewportStateCi.pViewports = viewports.data();
        viewportStateCi.pScissors = scissors.data();
        colorBlendStateCi.pAttachments = colorBlendAttachmentStates.data();
        dynamicStateCi.pDynamicStates = dynamicStates.data();
    }
};
class PipelineLayout {
    VkPipelineLayout handle = VK_NULL_HANDLE;

   public:
    forceinline PipelineLayout() = default;
    forceinline PipelineLayout(VkPipelineLayoutCreateInfo& createInfo) {
        create(createInfo);
    }
    forceinline PipelineLayout(PipelineLayout&& other) noexcept {
        handle = other.handle;
        other.handle = VK_NULL_HANDLE;
    }
    forceinline ~PipelineLayout() {
        if (handle)
            vkDestroyPipelineLayout(cur_context().device, handle, nullptr);
        handle = VK_NULL_HANDLE;
    }
    forceinline operator VkPipelineLayout() { return handle; }
    forceinline VkPipelineLayout* getPointer() { return &handle; }

    forceinline VkResult create(VkPipelineLayoutCreateInfo& createInfo) {
        VkResult result = vkCreatePipelineLayout(cur_context().device,
                                                 &createInfo, nullptr, &handle);
        if (result) {
            print_error("pipelineLayout",
                        "create pipelineLayout failed! Code: ",
                        string_VkResult(result));
        }
        return result;
    }
};
class Pipeline {
    VkPipeline handle = VK_NULL_HANDLE;

   public:
    forceinline Pipeline() = default;
    forceinline Pipeline(VkGraphicsPipelineCreateInfo& createInfo) {
        create(createInfo);
    }
    forceinline Pipeline(VkComputePipelineCreateInfo& createInfo) {
        create(createInfo);
    }
    forceinline Pipeline(Pipeline&& other) noexcept {
        handle = other.handle;
        other.handle = VK_NULL_HANDLE;
    }
    forceinline ~Pipeline() {
        vkDestroyPipeline(cur_context().device, handle, nullptr);
    }
    forceinline operator VkPipeline() { return handle; }
    forceinline VkPipeline* getPointer() { return &handle; }
    forceinline VkResult create(VkGraphicsPipelineCreateInfo& createInfo) {
        VkResult result =
            vkCreateGraphicsPipelines(cur_context().device, VK_NULL_HANDLE, 1,
                                      &createInfo, nullptr, &handle);
        if (result) {
            print_error("pipeline",
                        "Failed to create a graphics pipeline! Code:",
                        string_VkResult(result));
        }
        return result;
    }
    forceinline VkResult create(VkComputePipelineCreateInfo& createInfo) {
        VkResult result =
            vkCreateComputePipelines(cur_context().device, VK_NULL_HANDLE, 1,
                                     &createInfo, nullptr, &handle);
        if (result) {
            print_error("pipeline",
                        "Failed to create a compute pipeline! Code:",
                        string_VkResult(result));
        }

        return result;
    }
};
class Buffer {
   protected:
    VkBuffer handle = VK_NULL_HANDLE;
    VmaAllocation allocation = VK_NULL_HANDLE;

   public:
    forceinline Buffer() = default;
    forceinline Buffer(VkBufferCreateInfo& createInfo,
                       VmaAllocationCreateInfo& allocInfo) {
        allocate(createInfo, allocInfo);
    }
    forceinline Buffer(VkDeviceSize size,
                       VkBufferCreateFlags vk_flag,
                       VkBufferUsageFlags vk_usage,
                       VmaAllocationCreateFlags vma_flag,
                       VmaMemoryUsage vma_usage = VMA_MEMORY_USAGE_AUTO,
                       VkSharingMode sharing_mode = VK_SHARING_MODE_EXCLUSIVE) {
        allocate(size, vk_flag, vk_usage, vma_flag, vma_usage, sharing_mode);
    }
    forceinline Buffer(Buffer&& other) noexcept {
        handle = other.handle;
        other.handle = VK_NULL_HANDLE;
        allocation = other.allocation;
        other.allocation = VK_NULL_HANDLE;
    }
    forceinline operator VkBuffer() { return handle; }
    forceinline VkBuffer* getPointer() { return &handle; }
    forceinline operator VmaAllocation() { return allocation; }
    forceinline VmaAllocation getAllocation() { return allocation; }
    forceinline ~Buffer() {
        vmaDestroyBuffer(cur_context().allocator, handle, allocation);
        handle = VK_NULL_HANDLE;
        allocation = VK_NULL_HANDLE;
    }
    forceinline VkResult transfer_data(const void* pData,
                                       VkDeviceSize length,
                                       VkDeviceSize offset = 0) {
        VkResult result = vmaCopyMemoryToAllocation(
            cur_context().allocator, pData, allocation, offset, length);
        if (result) {
            print_error("Buffer", "transfer_data() failed! Code:",
                        string_VkResult(result));
        }
        return result;
    }
    forceinline VkResult retrieve_data(void* pData,
                                       VkDeviceSize length,
                                       VkDeviceSize offset = 0) {
        VkResult result = vmaCopyAllocationToMemory(
            cur_context().allocator, allocation, offset, pData, length);
        if (result) {
            print_error("Buffer", "retrieve_data() failed! Code:",
                        string_VkResult(result));
        }
        return result;
    }
    forceinline void* map_data() {
        void* data = nullptr;
        VkResult result =
            vmaMapMemory(cur_context().allocator, allocation, &data);
        if (result) {
            print_error("Buffer",
                        "map_data() failed! Code:", string_VkResult(result));
        }
        return data;
    }
    forceinline void unmap_data() {
        vmaUnmapMemory(cur_context().allocator, allocation);
    }
    forceinline VkResult flush_data(VkDeviceSize offset = 0,
                                    VkDeviceSize length = VK_WHOLE_SIZE) {
        VkResult result = vmaFlushAllocation(cur_context().allocator,
                                             allocation, offset, length);
        if (result) {
            print_error("Buffer",
                        "flush_data() failed! Code:", string_VkResult(result));
        }
        return result;
    }
    forceinline VkResult invalidate_data(VkDeviceSize offset = 0,
                                         VkDeviceSize length = VK_WHOLE_SIZE) {
        VkResult result = vmaInvalidateAllocation(cur_context().allocator,
                                                  allocation, offset, length);
        if (result) {
            print_error("Buffer", "invalidate_data() failed! Code:",
                        string_VkResult(result));
        }
        return result;
    }
    forceinline VkResult allocate(VkBufferCreateInfo& createInfo,
                                  VmaAllocationCreateInfo& allocInfo) {
        VkResult result =
            vmaCreateBuffer(cur_context().allocator, &createInfo, &allocInfo,
                            &handle, &allocation, nullptr);
        if (result) {
            print_error("Buffer", "VMA error when create Buffer. Code:",
                        string_VkResult(result));
        }
        return result;
    }
    forceinline VkResult
    allocate(VkDeviceSize size,
             VkBufferCreateFlags vk_flag,
             VkBufferUsageFlags vk_usage,
             VmaAllocationCreateFlags vma_flag,
             VmaMemoryUsage vma_usage,
             VkSharingMode sharing_mode = VK_SHARING_MODE_EXCLUSIVE) {
        VkBufferCreateInfo createInfo = {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .flags = vk_flag,
            .size = size,
            .usage = vk_usage,
            .sharingMode = sharing_mode};
        VmaAllocationCreateInfo allocInfo = {.flags = vma_flag,
                                             .usage = vma_usage};
        return allocate(createInfo, allocInfo);
    }
};
forceinline VkDeviceSize calculate_block_alignment(VkDeviceSize size) {
    static const VkDeviceSize uniformAlignment =
        cur_context()
            .phyDeviceProperties.properties.limits
            .minUniformBufferOffsetAlignment;
    return ((uniformAlignment + size - 1) & ~(uniformAlignment - 1));
}
class IndexBuffer : protected Buffer {
   public:
    forceinline IndexBuffer() = default;
    forceinline IndexBuffer(
        VkDeviceSize block_size,
        VkBufferCreateFlags flags = 0,
        VkBufferUsageFlags other_usage = 0,
        VkSharingMode sharing_mode = VK_SHARING_MODE_EXCLUSIVE) {
        create(block_size, flags, other_usage, sharing_mode);
    }
    forceinline IndexBuffer(IndexBuffer&& other) noexcept
        : Buffer(std::move(other)) {}
    forceinline operator VkBuffer() { return handle; }
    forceinline VkBuffer* getPointer() { return &handle; }
    forceinline ~IndexBuffer() {}
    forceinline VkResult
    create(VkDeviceSize block_size,
           VkBufferCreateFlags flags = 0,
           VkBufferUsageFlags other_usage = 0,
           VkSharingMode sharing_mode = VK_SHARING_MODE_EXCLUSIVE) {
        VkResult result = this->allocate(
            block_size, flags,
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT |
                VK_BUFFER_USAGE_TRANSFER_DST_BIT | other_usage,
            0, VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE, sharing_mode);
        return result;
    }
};
class VertexBuffer : protected Buffer {
   public:
    forceinline VertexBuffer() = default;
    forceinline VertexBuffer(
        VkDeviceSize block_size,
        VkBufferCreateFlags flags = 0,
        VkBufferUsageFlags other_usage = 0,
        VkSharingMode sharing_mode = VK_SHARING_MODE_EXCLUSIVE) {
        create(block_size, flags, other_usage, sharing_mode);
    }
    forceinline VertexBuffer(VertexBuffer&& other) noexcept
        : Buffer(std::move(other)) {}
    forceinline operator VkBuffer() { return handle; }
    forceinline VkBuffer* getPointer() { return &handle; }
    forceinline ~VertexBuffer() {}
    forceinline VkResult
    create(VkDeviceSize block_size,
           VkBufferCreateFlags flags = 0,
           VkBufferUsageFlags other_usage = 0,
           VkSharingMode sharing_mode = VK_SHARING_MODE_EXCLUSIVE) {
        VkResult result = this->allocate(
            block_size, flags,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                VK_BUFFER_USAGE_TRANSFER_DST_BIT | other_usage,
            0, VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE, sharing_mode);
        return result;
    }
};
class TransferBuffer : protected Buffer {
   protected:
    void* pBufferData;
    VkDeviceSize bufferSize;

   public:
    forceinline TransferBuffer() = default;
    forceinline TransferBuffer(
        VkDeviceSize block_size,
        VkBufferCreateFlags flags = 0,
        VkBufferUsageFlags other_usage = 0,
        VkSharingMode sharing_mode = VK_SHARING_MODE_EXCLUSIVE) {
        create(block_size, flags, other_usage, sharing_mode);
    }
    forceinline TransferBuffer(TransferBuffer&& other) noexcept
        : Buffer(std::move(other)) {}
    forceinline operator VkBuffer() { return handle; }
    forceinline VkBuffer* getPointer() { return &handle; }
    forceinline void* get_pdata() { return pBufferData; }
    forceinline ~TransferBuffer() {}
    forceinline VkResult
    resize(VkDeviceSize new_size,
           VkBufferCreateFlags flags = 0,
           VkBufferUsageFlags other_usage = 0,
           VkSharingMode sharing_mode = VK_SHARING_MODE_EXCLUSIVE) {
        if (bufferSize >= new_size)
            return VK_SUCCESS;
        else {
            vmaDestroyBuffer(cur_context().allocator, handle, allocation);
            return create(new_size, flags, other_usage, sharing_mode);
        }
    }
    forceinline VkResult flush() { return this->flush_data(); }
    forceinline VkResult flush(VkDeviceSize offset, VkDeviceSize length) {
        return this->flush_data(offset, length);
    }
    forceinline VkResult transfer_data(const void* pData) {
        memcpy(pBufferData, pData, bufferSize);
        return this->flush_data();
    }
    forceinline VkResult transfer_data(const void* pData,
                                       VkDeviceSize offset,
                                       VkDeviceSize length) {
        memcpy((uint8_t*)pBufferData + offset, pData, length);
        return this->flush_data(offset, length);
    }
    forceinline void cmd_insert_transfer(VkCommandBuffer cmdBuf,
                                         VkBuffer dstBuf,
                                         const VkBufferCopy* copyInfos,
                                         uint32_t count = 1) {
        vkCmdCopyBuffer(cmdBuf, handle, dstBuf, count, copyInfos);
    }
    forceinline VkResult transfer_to_buffer(VkQueue cmdPool,
                                            VkCommandBuffer cmdBuf,
                                            VkBuffer dstBuf,
                                            const VkBufferCopy* copyInfos,
                                            uint32_t count,
                                            VkFence fence) {
        VkCommandBufferBeginInfo beginInfo = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT};
        VkResult result = vkBeginCommandBuffer(cmdBuf, &beginInfo);
        if (result) {
            print_error("TransferBuffer",
                        "Failed to begin a command buffer! Code:",
                        string_VkResult(result));
            return result;
        }
        vkCmdCopyBuffer(cmdBuf, handle, dstBuf, count, copyInfos);
        result = vkEndCommandBuffer(cmdBuf);
        if (result) {
            print_error("TransferBuffer",
                        "Failed to end a command buffer! Code:",
                        string_VkResult(result));
            return result;
        }
        VkSubmitInfo submitInfo = {.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                                   .commandBufferCount = 1,
                                   .pCommandBuffers = &cmdBuf};
        result = vkQueueSubmit(cmdPool, 1, &submitInfo, fence);
        if (result) {
            print_error("TransferBuffer", "Failed to submit command! Code:",
                        string_VkResult(result));
            return result;
        }
        return VK_SUCCESS;
    }
    forceinline VkResult
    create(VkDeviceSize block_size,
           VkBufferCreateFlags flags = 0,
           VkBufferUsageFlags other_usage = 0,
           VkSharingMode sharing_mode = VK_SHARING_MODE_EXCLUSIVE) {
        VkResult result = this->allocate(
            block_size, flags, VK_BUFFER_USAGE_TRANSFER_SRC_BIT | other_usage,
            VMA_ALLOCATION_CREATE_MAPPED_BIT |
                VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT,
            VMA_MEMORY_USAGE_AUTO_PREFER_HOST, sharing_mode);
        VmaAllocationInfo allocInfo;
        vmaGetAllocationInfo(cur_context().allocator, allocation, &allocInfo);
        pBufferData = allocInfo.pMappedData;
        bufferSize = allocInfo.size;
        return result;
    }
};
class UniformBuffer : protected Buffer {
   protected:
    void* pBufferData;
    VkDeviceSize blockOffset, blockSize;

   public:
    forceinline UniformBuffer() = default;
    forceinline UniformBuffer(
        VkDeviceSize block_size,
        VkBufferCreateFlags flags = 0,
        VkBufferUsageFlags other_usage = 0,
        VkSharingMode sharing_mode = VK_SHARING_MODE_EXCLUSIVE) {
        create(blockSize, flags, other_usage, sharing_mode);
    }
    forceinline UniformBuffer(UniformBuffer&& other) noexcept
        : Buffer(std::move(other)) {
        pBufferData = other.pBufferData;
        other.pBufferData = nullptr;
        blockOffset = other.blockOffset;
        blockSize = other.blockSize;
    }
    forceinline operator VkBuffer() { return handle; }
    forceinline VkBuffer* getPointer() { return &handle; }
    forceinline ~UniformBuffer() { pBufferData = nullptr; }
    forceinline void transfer_data(const void* pData) {
        for (uint32_t i = 0; i < MAX_FLIGHT_COUNT; i++) {
            memcpy((uint8_t*)pBufferData + i * blockOffset, pData, blockSize);
        }
        this->flush_data();
    }
    forceinline void transfer_data(const void* pData,
                                   uint32_t size,
                                   uint32_t offset) {
        uint32_t d;
        for (uint32_t i = 0; i < MAX_FLIGHT_COUNT; i++) {
            d = i * blockOffset + offset;
            memcpy((uint8_t*)pBufferData + d, pData, size);
            this->flush_data(d, size);
        }
    }
    forceinline void* get_pdata() { return pBufferData; }
    forceinline VkDeviceSize get_alignment() { return blockOffset; }
    forceinline VkDeviceSize get_block_size() { return blockSize; }

    forceinline VkResult
    create(VkDeviceSize block_size,
           VkBufferCreateFlags flags = 0,
           VkBufferUsageFlags other_usage = 0,
           VkSharingMode sharing_mode = VK_SHARING_MODE_EXCLUSIVE) {
        blockOffset = calculate_block_alignment(block_size);
        blockSize = block_size;
        VkResult result = this->allocate(
            blockOffset * MAX_FLIGHT_COUNT, flags,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | other_usage,
            VMA_ALLOCATION_CREATE_MAPPED_BIT |
                VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
            VMA_MEMORY_USAGE_AUTO, sharing_mode);
        VmaAllocationInfo allocInfo;
        vmaGetAllocationInfo(cur_context().allocator, allocation, &allocInfo);
        pBufferData = allocInfo.pMappedData;
        return result;
    }
};
class BufferView {
    VkBufferView handle = VK_NULL_HANDLE;

   public:
    forceinline BufferView() = default;
    forceinline BufferView(VkBufferViewCreateInfo& createInfo) {
        create(createInfo);
    }
    forceinline BufferView(
        VkBuffer buffer,
        VkFormat format,
        VkDeviceSize offset = 0,
        VkDeviceSize range = 0 /*VkBufferViewCreateFlags flags*/) {
        create(buffer, format, offset, range);
    }
    forceinline BufferView(BufferView&& other) noexcept {
        handle = other.handle;
        other.handle = VK_NULL_HANDLE;
    }
    forceinline ~BufferView() {
        if (handle)
            vkDestroyBufferView(cur_context().device, handle, nullptr);
        handle = VK_NULL_HANDLE;
    }
    forceinline operator VkBufferView() { return handle; }
    forceinline VkBufferView* getPointer() { return &handle; }
    forceinline VkResult create(VkBufferViewCreateInfo& createInfo) {
        VkResult result = vkCreateBufferView(cur_context().device, &createInfo,
                                             nullptr, &handle);
        if (result) {
            print_error("BufferView", "Failed to create a buffer view! Code:",
                        string_VkResult(result));
        }
        return result;
    }
 forceinline   VkResult create(VkBuffer buffer,
                    VkFormat format,
                    VkDeviceSize offset = 0,
                    VkDeviceSize range = 0 /*VkBufferViewCreateFlags flags*/) {
        VkBufferViewCreateInfo createInfo = {
            .sType = VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO,
            .buffer = buffer,
            .format = format,
            .offset = offset,
            .range = range};
        return create(createInfo);
    }
};
// 图像数据类
// 指定格式时使用 vk_format_utils.h 的内容
class Image {
    VkImage handle = VK_NULL_HANDLE;
    VmaAllocation allocation = VK_NULL_HANDLE;

   public:
  forceinline  Image() = default;
    forceinline Image(VkImageCreateInfo& createInfo,
                      VmaAllocationCreateInfo& allocInfo) {
        create(createInfo, allocInfo);
    }
    forceinline Image(Image&& other) noexcept {
        handle = other.handle;
        other.handle = VK_NULL_HANDLE;
        allocation = other.allocation;
        other.allocation = VK_NULL_HANDLE;
    }
    forceinline ~Image() {
        vmaDestroyImage(cur_context().allocator, handle, allocation);
        handle = VK_NULL_HANDLE;
        allocation = VK_NULL_HANDLE;
    }
    forceinline operator VkImage() { return handle; }
    forceinline VkImage* getPointer() { return &handle; }
    forceinline operator VmaAllocation() { return allocation; }
    forceinline VmaAllocation getAllocation() { return allocation; }
 forceinline   VkResult create(VkImageCreateInfo& createInfo,
                    VmaAllocationCreateInfo& allocInfo) {
        VkResult result =
            vmaCreateImage(cur_context().allocator, &createInfo, &allocInfo,
                           &handle, &allocation, nullptr);
        if (result) {
            print_error("image", "Failed to create an image! Code:",
                        string_VkResult(result));
        }
        return result;
    }
};
class ImageView {
    VkImageView handle = VK_NULL_HANDLE;

   public:
 forceinline   ImageView() = default;
    forceinline ImageView(VkImageViewCreateInfo& createInfo) {
        allocate(createInfo);
    }
forceinline    ImageView(VkImage image,
              VkImageViewType viewType,
              VkFormat format,
              const VkImageSubresourceRange& subresourceRange,
              VkImageViewCreateFlags flags = 0) {
        allocate(image, viewType, format, subresourceRange, flags);
    }
    forceinline ImageView(ImageView&& other) noexcept {
        handle = other.handle;
        other.handle = VK_NULL_HANDLE;
    }
    forceinline ~ImageView() {
        if (handle)
            vkDestroyImageView(cur_context().device, handle, nullptr);
        handle = VK_NULL_HANDLE;
    }
    forceinline operator VkImageView() { return handle; }
    forceinline VkImageView* getPointer() { return &handle; }
    forceinline VkResult allocate(VkImageViewCreateInfo& createInfo) {
        VkResult result = vkCreateImageView(cur_context().device, &createInfo,
                                            nullptr, &handle);
        if (result) {
            print_error("ImageView",
                        "Failed to create an image view! "
                        "Code:",
                        string_VkResult(result));
        }
        return result;
    }
  forceinline  VkResult allocate(VkImage image,
                      VkImageViewType viewType,
                      VkFormat format,
                      const VkImageSubresourceRange& subresourceRange,
                      VkImageViewCreateFlags flags = 0) {
        VkImageViewCreateInfo createInfo = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .flags = flags,
            .image = image,
            .viewType = viewType,
            .format = format,
            .subresourceRange = subresourceRange};
        return allocate(createInfo);
    }
};
class Sampler {
    VkSampler handle = VK_NULL_HANDLE;

   public:
 forceinline   Sampler() = default;
    forceinline Sampler(VkSamplerCreateInfo& createInfo) { create(createInfo); }
    forceinline Sampler(Sampler&& other) noexcept {
        handle = other.handle;
        other.handle = VK_NULL_HANDLE;
    }
    forceinline ~Sampler() {
        if (handle)
            vkDestroySampler(cur_context().device, handle, nullptr);
        handle = VK_NULL_HANDLE;
    }
    forceinline operator VkSampler() { return handle; }
    forceinline VkSampler* getPointer() { return &handle; }
    forceinline VkResult create(VkSamplerCreateInfo& createInfo) {
        VkResult result = vkCreateSampler(cur_context().device, &createInfo,
                                          nullptr, &handle);
        if (result) {
            print_error("Sampler", "Failed to create a Sampler! Code:",
                        string_VkResult(result));
        }
        return result;
    }
};
class DescriptorSetLayout {
    VkDescriptorSetLayout handle = VK_NULL_HANDLE;

   public:
 forceinline   DescriptorSetLayout() = default;
    forceinline DescriptorSetLayout(
        VkDescriptorSetLayoutCreateInfo& createInfo) {
        create(createInfo);
    }
    forceinline DescriptorSetLayout(DescriptorSetLayout&& other) noexcept {
        handle = other.handle;
        other.handle = VK_NULL_HANDLE;
    }
    forceinline ~DescriptorSetLayout() {
        if (handle)
            vkDestroyDescriptorSetLayout(cur_context().device, handle, nullptr);
        handle = VK_NULL_HANDLE;
    }
    forceinline operator VkDescriptorSetLayout() { return handle; }
    forceinline VkDescriptorSetLayout* getPointer() { return &handle; }
    forceinline VkResult create(VkDescriptorSetLayoutCreateInfo& createInfo) {
        VkResult result = vkCreateDescriptorSetLayout(
            cur_context().device, &createInfo, nullptr, &handle);
        if (result) {
            print_error("DescriptorSetLayout",
                        "Failed to create a descriptor set layout! Code:",
                        string_VkResult(result));
        }
        return result;
    }
};
class DescriptorSet {
    friend class DescriptorPool;
    VkDescriptorSet handle = VK_NULL_HANDLE;

   public:
  forceinline  DescriptorSet() = default;
    forceinline DescriptorSet(DescriptorSet&& other) noexcept {
        handle = other.handle;
        other.handle = VK_NULL_HANDLE;
    }
    forceinline operator VkDescriptorSet() { return handle; }
    forceinline VkDescriptorSet* getPointer() { return &handle; }
   forceinline void write(const VkDescriptorImageInfo* pDescriptorImageInfos,
               uint32_t descriptorInfoCount,
               VkDescriptorType descriptorType,
               uint32_t dstBinding = 0,
               uint32_t dstArrayElement = 0) const {
        VkWriteDescriptorSet writeDescriptorSet = {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = handle,
            .dstBinding = dstBinding,
            .dstArrayElement = dstArrayElement,
            .descriptorCount = descriptorInfoCount,
            .descriptorType = descriptorType,
            .pImageInfo = pDescriptorImageInfos};
        update(&writeDescriptorSet);
    }
 forceinline   void write(const VkDescriptorBufferInfo* pDescriptorBufferInfos,
               uint32_t descriptorInfoCount,
               VkDescriptorType descriptorType,
               uint32_t dstBinding = 0,
               uint32_t dstArrayElement = 0) const {
        VkWriteDescriptorSet writeDescriptorSet = {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = handle,
            .dstBinding = dstBinding,
            .dstArrayElement = dstArrayElement,
            .descriptorCount = descriptorInfoCount,
            .descriptorType = descriptorType,
            .pBufferInfo = pDescriptorBufferInfos};
        update(&writeDescriptorSet);
    }
 forceinline   void write(const VkBufferView* pBufferViews,
               uint32_t descriptorInfoCount,
               VkDescriptorType descriptorType,
               uint32_t dstBinding = 0,
               uint32_t dstArrayElement = 0) const {
        VkWriteDescriptorSet writeDescriptorSet = {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = handle,
            .dstBinding = dstBinding,
            .dstArrayElement = dstArrayElement,
            .descriptorCount = descriptorInfoCount,
            .descriptorType = descriptorType,
            .pTexelBufferView = pBufferViews};
        update(&writeDescriptorSet);
    }
    forceinline static void update(VkWriteDescriptorSet* write) {
        vkUpdateDescriptorSets(cur_context().device, 1, write, 0, nullptr);
    }
    forceinline static void update(VkWriteDescriptorSet* write,
                                   VkCopyDescriptorSet* copy) {
        vkUpdateDescriptorSets(cur_context().device, 1, write, 1, copy);
    }
  forceinline  static void update(uint32_t writeCount,
                       VkWriteDescriptorSet* writes,
                       uint32_t copiesCount = 0,
                       VkCopyDescriptorSet* copies = nullptr) {
        vkUpdateDescriptorSets(cur_context().device, writeCount, writes,
                               copiesCount, copies);
    }
};
class DescriptorPool {
    VkDescriptorPool handle = VK_NULL_HANDLE;

   public:
  forceinline  DescriptorPool() = default;
    forceinline DescriptorPool(const VkDescriptorPoolCreateInfo& createInfo) {
        create(createInfo);
    }
  forceinline  DescriptorPool(uint32_t maxSetCount,
                   uint32_t poolSizeCount,
                   const VkDescriptorPoolSize* poolSizes,
                   VkDescriptorPoolCreateFlags flags = 0) {
        create(maxSetCount, poolSizeCount, poolSizes, flags);
    }
    forceinline DescriptorPool(DescriptorPool&& other) noexcept {
        handle = other.handle;
        other.handle = VK_NULL_HANDLE;
    }
    forceinline ~DescriptorPool() {
        if (handle) {
            vkDestroyDescriptorPool(cur_context().device, handle, nullptr);
        }
        handle = VK_NULL_HANDLE;
    }
    forceinline operator VkDescriptorPool() { return handle; }
    forceinline VkDescriptorPool* getPointer() { return &handle; }
    // 分配描述符集
    // setLayouts的数量必须等于sets的数量
   forceinline VkResult allocate_sets(uint32_t setCount,
                           VkDescriptorSet* sets,
                           const VkDescriptorSetLayout* setLayouts) const {
        VkDescriptorSetAllocateInfo allocateInfo = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .descriptorPool = handle,
            .descriptorSetCount = setCount,
            .pSetLayouts = setLayouts};
        VkResult result =
            vkAllocateDescriptorSets(cur_context().device, &allocateInfo, sets);
        if (result) {
            print_error("DescriptorPool",
                        "Failed to allocate descriptor "
                        "sets! Code:",
                        string_VkResult(result));
        }
        return result;
    }
    forceinline VkResult free_sets(uint32_t setCount,
                                   VkDescriptorSet* sets) const {
        VkResult result =
            vkFreeDescriptorSets(cur_context().device, handle, setCount, sets);
        memset(sets, 0, setCount * sizeof(VkDescriptorSet));
        return result;
    }
    forceinline VkResult create(const VkDescriptorPoolCreateInfo& createInfo) {
        VkResult result = vkCreateDescriptorPool(cur_context().device,
                                                 &createInfo, nullptr, &handle);
        if (result) {
            print_error("DescriptorPool",
                        "Failed to create a descriptor "
                        "pool! Code:",
                        string_VkResult(result));
        }
        return result;
    }
  forceinline  VkResult create(uint32_t maxSetCount,
                    uint32_t poolSizeCount,
                    const VkDescriptorPoolSize* poolSizes,
                    VkDescriptorPoolCreateFlags flags = 0) {
        VkDescriptorPoolCreateInfo createInfo = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .flags = flags,
            .maxSets = maxSetCount,
            .poolSizeCount = poolSizeCount,
            .pPoolSizes = poolSizes};
        return create(createInfo);
    }
};
class QueryPool {
    VkQueryPool handle = VK_NULL_HANDLE;

   public:
 forceinline   QueryPool() = default;
    forceinline QueryPool(VkQueryPoolCreateInfo& createInfo) {
        create(createInfo);
    }
  forceinline  QueryPool(VkQueryType queryType,
              uint32_t queryCount,
              VkQueryPipelineStatisticFlags pipelineStatistics =
                  0 /*VkQueryPoolCreateFlags flags*/) {
        create(queryType, queryCount, pipelineStatistics);
    }
    forceinline QueryPool(QueryPool&& other) noexcept {
        handle = other.handle;
        other.handle = VK_NULL_HANDLE;
    }
    forceinline ~QueryPool() {
        if (handle)
            vkDestroyQueryPool(cur_context().device, handle, nullptr);
        handle = VK_NULL_HANDLE;
    }
    forceinline operator VkQueryPool() { return handle; }
    forceinline VkQueryPool* getPointer() { return &handle; }
  forceinline  void cmd_reset(VkCommandBuffer cmdBuf,
                   uint32_t firstQueryIndex,
                   uint32_t queryCount) const {
        vkCmdResetQueryPool(cmdBuf, handle, firstQueryIndex, queryCount);
    }
 forceinline   void cmd_begin(VkCommandBuffer cmdBuf,
                   uint32_t queryIndex,
                   VkQueryControlFlags flags = 0) const {
        vkCmdBeginQuery(cmdBuf, handle, queryIndex, flags);
    }
    forceinline void cmd_end(VkCommandBuffer cmdBuf,
                             uint32_t queryIndex) const {
        vkCmdEndQuery(cmdBuf, handle, queryIndex);
    }
  forceinline  void cmd_write_timestamp(VkCommandBuffer cmdBuf,
                             VkPipelineStageFlagBits pipelineStage,
                             uint32_t queryIndex) const {
        vkCmdWriteTimestamp(cmdBuf, pipelineStage, handle, queryIndex);
    }
 forceinline  void cmd_copy_results(VkCommandBuffer cmdBuf,
                          uint32_t firstQueryIndex,
                          uint32_t queryCount,
                          VkBuffer buffer_dst,
                          VkDeviceSize offset_dst,
                          VkDeviceSize stride,
                          VkQueryResultFlags flags = 0) const {
        vkCmdCopyQueryPoolResults(cmdBuf, handle, firstQueryIndex, queryCount,
                                  buffer_dst, offset_dst, stride, flags);
    }
 forceinline   VkResult get_results(uint32_t firstQueryIndex,
                         uint32_t queryCount,
                         size_t dataSize,
                         void* pData_dst,
                         VkDeviceSize stride,
                         VkQueryResultFlags flags = 0) const {
        VkResult result = vkGetQueryPoolResults(
            cur_context().device, handle, firstQueryIndex, queryCount, dataSize,
            pData_dst, stride, flags);
        if (result) {
            result > 0
                ?  // 若返回值为VK_NOT_READY，则查询尚未结束，有查询结果尚不可获
                print_error("QueryPool", "Not all queries are available! Code:",
                            string_VkResult(result))
                : print_error("QueryPool",
                              "Failed to get query pool results! Code:",
                              string_VkResult(result));
        }
        return result;
    }
    forceinline void reset(uint32_t firstQueryIndex, uint32_t queryCount) {
        vkResetQueryPool(cur_context().device, handle, firstQueryIndex,
                         queryCount);
    }
    forceinline VkResult create(VkQueryPoolCreateInfo& createInfo) {
        VkResult result = vkCreateQueryPool(cur_context().device, &createInfo,
                                            nullptr, &handle);
        if (result) {
            print_error("QueryPool", "Failed to create a query pool! Code:",
                        string_VkResult(result));
        }
        return result;
    }
  forceinline  VkResult create(VkQueryType queryType,
                    uint32_t queryCount,
                    VkQueryPipelineStatisticFlags pipelineStatistics =
                        0 /*VkQueryPoolCreateFlags flags*/) {
        VkQueryPoolCreateInfo createInfo = {
            .sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO,
            .queryType = queryType,
            .queryCount = queryCount,
            .pipelineStatistics = pipelineStatistics};
        return create(createInfo);
    }
};
class OcclusionQueries {
   protected:
    QueryPool queryPool;
    std::vector<uint32_t> occlusionResults;

   public:
 forceinline   OcclusionQueries() = default;
    forceinline OcclusionQueries(uint32_t capacity) { create(capacity); }
    forceinline operator VkQueryPool() { return queryPool; }
    forceinline VkQueryPool* getPointer() { return queryPool.getPointer(); }
    forceinline uint32_t capacity() const { return occlusionResults.size(); }
    forceinline uint32_t passing_sample_count(uint32_t index) const {
        return occlusionResults[index];
    }
    forceinline void cmd_reset(VkCommandBuffer cmdBuf) const {
        queryPool.cmd_reset(cmdBuf, 0, capacity());
    }
  forceinline  void cmd_begin(VkCommandBuffer cmdBuf,
                   uint32_t queryIndex,
                   bool isPrecise = false) const {
        queryPool.cmd_begin(cmdBuf, queryIndex, isPrecise);
    }
    forceinline void cmd_end(VkCommandBuffer cmdBuf,
                             uint32_t queryIndex) const {
        queryPool.cmd_end(cmdBuf, queryIndex);
    }
    /*常用于GPU-driven遮挡剔除*/
 forceinline  void cmd_copy_results(VkCommandBuffer cmdBuf,
                          uint32_t firstQueryIndex,
                          uint32_t queryCount,
                          VkBuffer buffer_dst,
                          VkDeviceSize offset_dst,
                          VkDeviceSize stride) const {
        // 需要等待查询结束以获取正确的数值，flags为VK_QUERY_RESULT_WAIT_BIT
        queryPool.cmd_copy_results(cmdBuf, firstQueryIndex, queryCount,
                                   buffer_dst, offset_dst, stride,
                                   VK_QUERY_RESULT_WAIT_BIT);
    }
    forceinline void create(uint32_t capacity) {
        occlusionResults.resize(capacity);
        occlusionResults.shrink_to_fit();
        queryPool.create(VK_QUERY_TYPE_OCCLUSION, capacity);
    }
    forceinline void recreate(uint32_t capacity) {
        // wait_all();
        queryPool.~QueryPool();
        create(capacity);
    }
    forceinline VkResult get_results(uint32_t queryCount) {
        return queryPool.get_results(0, queryCount, queryCount * 4,
                                     occlusionResults.data(), 4);
    }
    forceinline VkResult get_results() {
        return queryPool.get_results(0, capacity(), capacity() * 4,
                                     occlusionResults.data(), 4);
    }
};
class Event {
    VkEvent handle = VK_NULL_HANDLE;

   public:
  forceinline  Event() = default;
    forceinline Event(VkEventCreateInfo& createInfo) { create(createInfo); }
    forceinline Event(Event&& other) noexcept {
        handle = other.handle;
        other.handle = VK_NULL_HANDLE;
    }
    forceinline ~Event() {
        if (handle)
            vkDestroyEvent(cur_context().device, handle, nullptr);
        handle = VK_NULL_HANDLE;
    }
    forceinline operator VkEvent() { return handle; }
    forceinline VkEvent* getPointer() { return &handle; }
  forceinline  void cmd_set(VkCommandBuffer commandBuffer,
                 VkPipelineStageFlags stage_from) const {
        vkCmdSetEvent(commandBuffer, handle, stage_from);
    }
 forceinline   void cmd_reset(VkCommandBuffer commandBuffer,
                   VkPipelineStageFlags stage_from) const {
        vkCmdResetEvent(commandBuffer, handle, stage_from);
    }
 forceinline   void cmd_wait(VkCommandBuffer commandBuffer,
                  VkPipelineStageFlags stage_from,
                  VkPipelineStageFlags stage_to,
                  VkMemoryBarrier* memoryBarriers,
                  uint32_t memoryBarrierCount,
                  VkBufferMemoryBarrier* bufferMemoryBarriers,
                  uint32_t bufferMemoryBarrierCount,
                  VkImageMemoryBarrier* imageMemoryBarriers,
                  uint32_t imageMemoryBarrierCount) const {
        vkCmdWaitEvents(commandBuffer, 1, &handle, stage_from, stage_to,
                        memoryBarrierCount, memoryBarriers,
                        bufferMemoryBarrierCount, bufferMemoryBarriers,
                        imageMemoryBarrierCount, imageMemoryBarriers);
    }
    forceinline VkResult set() const {
        VkResult result = vkSetEvent(cur_context().device, handle);
        if (result) {
            print_error("Event", "Failed to singal the event! Code:",
                        string_VkResult(result));
        }
        return result;
    }
    forceinline VkResult reset() const {
        VkResult result = vkResetEvent(cur_context().device, handle);
        if (result) {
            print_error("Event", "Failed to unsingal the event! Code:",
                        string_VkResult(result));
        }
        return result;
    }
    forceinline VkResult status() const {
        VkResult result = vkGetEventStatus(cur_context().device, handle);
        if (result < 0)  // vkGetEventStatus(...)成功时有两种结果
        {
            print_error("Event",
                        "Failed to get the status of the "
                        "event! Code:",
                        string_VkResult(result));
        }
        return result;
    }
    forceinline VkResult create(VkEventCreateInfo& createInfo) {
        VkResult result =
            vkCreateEvent(cur_context().device, &createInfo, nullptr, &handle);
        if (result) {
            print_error("Event", "Failed to create a event! Code:",
                        string_VkResult(result));
        }
        return result;
    }
    forceinline VkResult create(VkEventCreateFlags flags = 0) {
        VkEventCreateInfo createInfo = {
            .sType = VK_STRUCTURE_TYPE_EVENT_CREATE_INFO, .flags = flags};
        return create(createInfo);
    }
};
}  // namespace BL
#ifdef forceinline
#undef forceinline
#endif
#endif  //! BOUNDLESS_TYPES_FILE