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
#ifndef _BL_CTX_INTERNAL_HPP_
#define _BL_CTX_INTERNAL_HPP_
#include "bl_output.hpp"
#include "vulkan/vulkan_core.h"
#include <algorithm>
#include <bl_contexts2.hpp>
namespace BLT {
//*****************************************************************************
// WindowContext 类
//*****************************************************************************
template <class BaseCtx>
CtxResult WindowContext<BaseCtx>::create(const SwapchainCreateInfo &info,
                                         ContextBase &ctx) {
  VkSurfaceCapabilitiesKHR surface_capabilities;
  // 获取surface支持能力
  if (VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
          ctx.m_PhysicalDevice, m_Surface, &surface_capabilities)) {
    print_error(s_TypeName,
                "Failed to get physical device surface capabilities! Code:",
                string_VkResult(result));
    return CtxResult::GetPhysicalDeviceSurfaceCapFailed;
  }
  auto &cInfo = m_SwapchainCreateInfo;
  // 如果容许的最大数量与最小数量不等，那么使用最小数量+1
  cInfo.minImageCount =
      surface_capabilities.minImageCount +
      (surface_capabilities.maxImageCount > surface_capabilities.minImageCount);
  // 决定窗口大小
  uint32_t width, height;
  BaseCtx::get_window_size(width, height);
  // surface_capabilities.currentExtent.width为 ~0u 表示大小未确定
  cInfo.imageExtent =
      surface_capabilities.currentExtent.width == (~0u)
          ? VkExtent2D{std::clamp(width,
                                  surface_capabilities.minImageExtent.width,
                                  surface_capabilities.maxImageExtent.width),
                       std::clamp(height,
                                  surface_capabilities.minImageExtent.height,
                                  surface_capabilities.maxImageExtent.height)}
          : surface_capabilities.currentExtent;
  cInfo.imageArrayLayers = 1;
  cInfo.preTransform = surface_capabilities.currentTransform;
  // 指定处理交换链图像透明通道方式
  if (surface_capabilities.supportedCompositeAlpha &
      VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR)
    cInfo.compositeAlpha = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;
  else
    for (size_t i = 0; i < 4; ++i)
      if (surface_capabilities.supportedCompositeAlpha & 1 << i) {
        cInfo.compositeAlpha = VkCompositeAlphaFlagBitsKHR(
            surface_capabilities.supportedCompositeAlpha & 1 << i);
        break;
      }
  // 指定图像的用途
  cInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  cInfo.imageUsage |= surface_capabilities.supportedUsageFlags &
                      VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
  if (surface_capabilities.supportedUsageFlags &
      VK_IMAGE_USAGE_TRANSFER_DST_BIT)
    cInfo.imageUsage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
  else
    print_warning(s_TypeName,
                  "VK_IMAGE_USAGE_TRANSFER_DST_BIT isn't supported!");
  // 指定图像格式
  if (m_AvailableFormats.empty() && acquire_surface_formats(ctx))
    return CtxResult::Failed;
  if (!cInfo.imageFormat)
    if (set_surface_format(
            {VK_FORMAT_R8G8B8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},
            ctx) &&
        set_surface_format(
            {VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},
            ctx)) {
      // 如果找不到上述图像格式和色彩空间的组合，那只能有什么用什么，采用availableSurfaceFormats中的第一组
      cInfo.imageFormat = m_AvailableFormats[0].format;
      cInfo.imageColorSpace = m_AvailableFormats[0].colorSpace;
      print_warning(s_TypeName,
                    "Failed to select a four-component UNORM surface format!");
    }
  // 指定呈现模式
  std::vector<VkPresentModeKHR> surfacePresentModes;
  if (acquire_present_modes(surfacePresentModes, ctx))
    return CtxResult::AcquirePresentModesFailed;
  cInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
  if (!info.m_isFrameRateLimited)
    for (size_t i = 0; i < surfacePresentModes.size(); i++)
      if (surfacePresentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
        cInfo.presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
        break;
      }
  const char *mode_str = string_VkPresentModeKHR(cInfo.presentMode);
  print_log(s_TypeName, "Present Mode", mode_str);
  //---------------------------------------------------------------------------
  cInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  cInfo.flags = info.m_flags;
  cInfo.surface = m_Surface;
  cInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  cInfo.clipped = VK_TRUE;
  cInfo.oldSwapchain = VK_NULL_HANDLE;
  cInfo.pNext = nullptr;
  //---------------------------------------------------------------------------
  if (VkResult result = create_swapchain_Internal(ctx))
    return CtxResult::FuncCreateSwapchainInternalFailed;
  m_CallbackSwapchainConstruct.iterate(this);
  return CtxResult::Success;
}
template <class BaseCtx>
void WindowContext<BaseCtx>::cleanup(ContextBase &ctx) noexcept {
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
    BaseCtx::cleanup();
  }
}
template <class BaseCtx>
VkResult WindowContext<BaseCtx>::create_surface(ContextBase &ctx) {
  VkSurfaceKHR surface = VK_NULL_HANDLE;
  if (VkResult result = BaseCtx::make_surface(ctx, surface)) {
    print_error(s_TypeName, "Failed to create a window surface! Code:",
                string_VkResult(result));
    return VK_RESULT_MAX_ENUM;
  }
  this->m_Surface = surface;
  return VK_SUCCESS;
}
template <class BaseCtx>
VkResult WindowContext<BaseCtx>::recreate_swapchain(ContextBase &ctx) {
  auto &cInfo = m_SwapchainCreateInfo;
  VkSurfaceCapabilitiesKHR surface_capabilities{};
  // 获取窗口表面能力，每次使用必须重新获取
  if (VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
          ctx.m_PhysicalDevice, m_Surface, &surface_capabilities)) {
    print_error(s_TypeName,
                "Failed to get physical device surface capabilities! Code:",
                string_VkResult(result));
    return result;
  }
  if (surface_capabilities.currentExtent.width == 0 ||
      surface_capabilities.currentExtent.height == 0)
    return VK_SUBOPTIMAL_KHR;
  cInfo.imageExtent = surface_capabilities.currentExtent;
  cInfo.oldSwapchain = m_Swapchain;
  VkResult result = vkQueueWaitIdle(ctx.m_Queue_graphics);
  // 仅在等待图形队列成功，且图形与呈现所用队列不同时等待呈现队列
  if (!result && ctx.m_Queue_graphics != ctx.m_Queue_presentation)
    result = vkQueueWaitIdle(ctx.m_Queue_presentation);
  if (result) {
    print_error(s_TypeName, "Failed to wait for the queue to be idle! Code:",
                string_VkResult(result));
    return result;
  }
  m_CallbackSwapchainDestroy.iterate(this);
  std::ranges::for_each(m_SwapchainImageViews, [&ctx](VkImageView view) {
    if (view)
      vkDestroyImageView(ctx.m_Device, view, nullptr);
  });
  if (VkResult result = create_swapchain_Internal(ctx)) {
    print_error(s_TypeName,
                "Create swapchain failed! Code:", string_VkResult(result));
    return result;
  }
  m_CallbackSwapchainConstruct.iterate(this);
  print_log(s_TypeName, "Swapchain recreated!");
  return VK_SUCCESS;
}
template <class BaseCtx>
VkResult WindowContext<BaseCtx>::create_swapchain_Internal(ContextBase &ctx) {
  auto &cInfo = m_SwapchainCreateInfo;
  // 直接创建交换链
  if (VkResult result =
          vkCreateSwapchainKHR(ctx.m_Device, &cInfo, nullptr, &m_Swapchain)) {
    print_error(s_TypeName,
                "Failed to create a swapchain! Code:", string_VkResult(result));
    return result;
  }
  // 获取交换链图像
  uint32_t swapchainImageCount;
  if (VkResult result = vkGetSwapchainImagesKHR(
          ctx.m_Device, m_Swapchain, &swapchainImageCount, nullptr)) {
    print_error(s_TypeName,
                "Failed to get the count of swapchain images! Code:",
                string_VkResult(result));
    return result;
  }
  m_SwapchainImages.resize(swapchainImageCount);
  if (VkResult result = vkGetSwapchainImagesKHR(ctx.m_Device, m_Swapchain,
                                                &swapchainImageCount,
                                                m_SwapchainImages.data())) {
    print_error(s_TypeName, "Failed to get swapchain images! Code:",
                string_VkResult(result));
    return result;
  }
  // 直接创建交换链，并且获取交换链图像和视图
  m_SwapchainImageViews.resize(swapchainImageCount);
  VkImageViewCreateInfo imageViewCreateInfo = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
      .viewType = VK_IMAGE_VIEW_TYPE_2D,
      .format = cInfo.imageFormat,
      //.components = {}, // 四个成员皆为VK_COMPONENT_SWIZZLE_IDENTITY
      .subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}};
  for (size_t i = 0; i < swapchainImageCount; i++) {
    imageViewCreateInfo.image = m_SwapchainImages[i];
    if (VkResult result =
            vkCreateImageView(ctx.m_Device, &imageViewCreateInfo, nullptr,
                              &m_SwapchainImageViews[i])) {
      print_error(s_TypeName, "Failed to create a swapchain image view! Code:",
                  string_VkResult(result));
      return result;
    }
  }
}
template <class BaseCtx>
VkResult WindowContext<BaseCtx>::acquire_surface_formats(ContextBase &ctx) {
  // 获取窗口表面格式例程
  uint32_t surfaceFormatCount;
  if (VkResult result = vkGetPhysicalDeviceSurfaceFormatsKHR(
          ctx.m_PhysicalDevice, m_Surface, &surfaceFormatCount, nullptr)) {
    print_error(s_TypeName,
                "Failed to get the count of surface "
                "formats! Code:",
                string_VkResult(result));
    return result;
  }
  if (!surfaceFormatCount)
    print_error(s_TypeName, "Failed to find any supported surface "
                            "format!"),
        abort();
  m_AvailableFormats.resize(surfaceFormatCount);
  VkResult result = vkGetPhysicalDeviceSurfaceFormatsKHR(
      ctx.m_PhysicalDevice, m_Surface, &surfaceFormatCount,
      m_AvailableFormats.data());
  if (result)
    print_error(s_TypeName,
                "Failed to get surface formats! "
                "Code:",
                string_VkResult(result));
  return VK_SUCCESS;
}
template <class BaseCtx>
VkResult WindowContext<BaseCtx>::acquire_present_modes(
    std::vector<VkPresentModeKHR> &presentModes, ContextBase &ctx) {
  // 获取呈现模式例程
  uint32_t surfacePresentModeCount;
  if (VkResult result = vkGetPhysicalDeviceSurfacePresentModesKHR(
          ctx.m_PhysicalDevice, m_Surface, &surfacePresentModeCount, nullptr)) {
    print_error(s_TypeName,
                "Failed to get the count of surface present modes! Code:",
                string_VkResult(result));
    return result;
  }
  if (!surfacePresentModeCount)
    print_error(s_TypeName, "Failed to find any surface present mode!"),
        abort();
  presentModes.resize(surfacePresentModeCount);
  if (VkResult result = vkGetPhysicalDeviceSurfacePresentModesKHR(
          ctx.m_PhysicalDevice, m_Surface, &surfacePresentModeCount,
          presentModes.data())) {
    print_error(s_TypeName,
                "Failed to get surface present "
                "modes! Code:",
                string_VkResult(result));
    return result;
  }
  return VK_SUCCESS;
}
template <class BaseCtx>
VkResult
WindowContext<BaseCtx>::set_surface_format(VkSurfaceFormatKHR surfaceFormat,
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
  // 如果交换链已存在，调用recreate_swapchain()重建交换链
  if (m_Swapchain)
    return recreate_swapchain(ctx);
  return VK_SUCCESS;
}
} // namespace BLT
#endif // !_BL_CTX_INTERNAL_HPP_
