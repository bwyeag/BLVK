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
#ifndef _BL_LOADER_SHADER_HPP_FILE_
#define _BL_LOADER_SHADER_HPP_FILE_
#include <bl_util.hpp>
#include <bl_vktypes.hpp>
#include <cstdint>
#include <vector>
namespace blt {
namespace _shader_traits {
struct ShaderStageInfo {
  uint32_t m_ModuleFlagBits;
  uint64_t m_ModuleCodeSize;
  uint64_t m_MoeuleCodeOffset;
};
struct ShaderFile {
  constexpr static uint32_t magic_number = 0x53484144; // hex of ascii: SHAD
  constexpr static uint32_t current_version = 1;
  uint32_t m_HeadNumber;
  uint32_t m_FileVersion;
  uint32_t m_HeadSize;
  uint32_t m_StagesCount;
  ShaderStageInfo m_Stages[];
};
} // namespace _shader_traits
LoadResult load_shader(std::vector<VkPipelineShaderStageCreateInfo> &ret,
                       const char *path);
} // namespace blt
#endif //! _BL_LOADER_SHADER_HPP_FILE_
