# BLVK

## 依赖

依赖于以下几个库:

- freetype
- freeimage
- glfw
- vulkan
- utfcpp
- vma (VulkanMemoryAllocator)

使用 CPM 进行包管理, 请将 CMakeCacke 中的 CPM_SOURCE_CACHE 设置为自己的路径或 OFF  
缺少的库将下载到 build/_deps 中, CPM 会自动下载.

## 宏

使用 BL_DEBUG 宏设置是否是 debug 状态