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
#include <cassert>
#ifndef BL_UTIL_RESULT_TYPE_FILE
#include <utils/bl_macro.hpp>
// 第三方库include
#include "vulkan/vk_enum_string_helper.h"
// 标准库include
#include <array>
#include <bit>
#include <cstdint>
#include <cstdlib>
namespace blt {
template <typename T> struct ResultEnumTrait {
  static constexpr bool is_error_enum = false;
  static constexpr uint16_t category_id = 0xffff;
};
template <> struct ResultEnumTrait<VkResult> {
  static constexpr bool is_error_enum = true;
  static constexpr uint16_t category_id = 0;
};
enum class CtxResult : int32_t {
  Success = 0,

  WindowCreateFailed = -1024,
  DeviceCreateFailed = -1025,
  InstanceCreateFailed = -1026,
  DebugCreateFailed = -1027,
  VmaCreateFailed = -1028,
  GLFWInitFailed = -1029,

  FuncGetVideoModeFailed = -2048,
  FuncGetPhysicalDeviceSurfaceCapFailed = -2049,
  FuncCreateSwapchainInternalFailed = -2050,

  AcquirePresentModesFailed = -4096,
  AcquireDeviceExtensionsFailed = -4097,
  AcquirePhysicalDevicesFailed = -4098,
  AcquireApiVersionFailed = -4099,
  AcquireGlfwExtFailed = -4100,
  AcquireSurfaceFormatsFailed = -4101,

  NoFitDevice = -8192,
  VulkanAPIVersionTooLow = -8193,

  CheckExtFailed = -16384,
  CheckLayerFailed = -16385
};
template <> struct ResultEnumTrait<CtxResult> {
  static constexpr bool is_error_enum = true;
  static constexpr uint16_t category_id = 1;
};
enum class RenderResult : int32_t {
  Success = 0,
  NullPointer = -1,
  SwapImageFailed = -2,
  QueueSubmitFailed = -3,
  PresentImageFailed = -4
};
template <> struct ResultEnumTrait<RenderResult> {
  static constexpr bool is_error_enum = true;
  static constexpr uint16_t category_id = 2;
};
enum class LoadResult : int32_t {
  Success = 0,
  FileNotFound = -1,
  FileStructError = -2
};
template <> struct ResultEnumTrait<LoadResult> {
  static constexpr bool is_error_enum = true;
  static constexpr uint16_t category_id = 3;
};
//*****************************************************************************
// result_t
//*****************************************************************************
struct result_value_data_t {
  const char *file_name;
  const char *function_name;
  int line;
};
struct [[nodiscard]] result_t {
  static constexpr uint32_t NullIndex = 0xffff;
  uint32_t m_index : 16;    // 具体错误信息的索引, 0xffff表示无信息
  uint32_t m_category : 16; // 指向result_category_t的数组下标
  int32_t m_type;           // 错误类型编码

  constexpr result_t()
      : m_category(0u), m_index(result_t::NullIndex), m_type(0) {}
  template <typename T>
    requires ResultEnumTrait<T>::is_error_enum &&
             (!ResultEnumTrait<T>::has_detail_info)
  constexpr result_t(const T &code);
  constexpr result_t(const result_t &) = default;
  constexpr result_t(result_t &&) = default;
  constexpr ~result_t() = default;
  const char *category() const;
  const char *message() const;
  result_value_data_t &acquire();
  INLINE constexpr operator bool() const { return m_type == 0; }
  INLINE constexpr operator uint64_t() const {
    return std::bit_cast<uint64_t>(*this);
  }
  template <typename T> INLINE constexpr bool operator==(T code) const {
    return (m_category == ResultEnumTrait<T>::category_id &&
            m_type == static_cast<int32_t>(code));
  }
  template <typename Func> void install(Func &&fn, const result_t &next);
  template <typename Func> void remove(Func &&fn);
  void remove();
};
static_assert(sizeof(result_t) == 8);
template <typename T> result_t make_result(T code) { return result_t{code}; }
//*****************************************************************************
// static data
//*****************************************************************************
constexpr int err_category_count = 4;
struct result_category_t {
  const char *m_CategoryName;
  const char *(*GetMessage)(int32_t);
};
extern const std::array<result_category_t, err_category_count> s_CategoryInfo;
constexpr int err_info_capacity = 32;
struct result_value_t {
  uint64_t m_Head; // 保存一个result_t或者一个指针
  result_value_t *m_NextVal;
  result_value_data_t m_Data;
  result_value_t() : m_Head(0u) {}
};
struct result_head_t {
  result_value_t *ptr;
  result_head_t();
};
extern thread_local std::array<result_value_t, err_info_capacity> s_ResultValue;
extern thread_local result_head_t s_ResultValue_head;
//*****************************************************************************
INLINE const char *result_t::category() const {
  return s_CategoryInfo[m_index].m_CategoryName;
}
INLINE const char *result_t::message() const {
  return s_CategoryInfo[m_index].GetMessage(m_type);
}
INLINE result_value_data_t &result_t::acquire() {
  return s_ResultValue[m_index].m_Data;
}
template <typename T>
  requires ResultEnumTrait<T>::is_error_enum &&
               (!ResultEnumTrait<T>::has_detail_info)
INLINE constexpr result_t::result_t(const T &code)
    : m_category(ResultEnumTrait<T>::category_id),
      m_type(static_cast<int32_t>(code)), m_index(result_t::NullIndex) {}
template <typename Func>
void result_t::install(Func &&fn, const result_t &next) {
  result_value_t *p = s_ResultValue_head.ptr;
  s_ResultValue_head.ptr =
      std::bit_cast<decltype(p)>(s_ResultValue_head.ptr->m_Head);
  m_index = p - s_ResultValue.data();
  p->m_NextVal = (next.m_index != result_t::NullIndex)
                     ? s_ResultValue.data() + next.m_index
                     : nullptr;
  p->m_Head = uint64_t(*this);
  fn(p->m_Data);
}
template <typename Func> void result_t::remove(Func &&fn) {
#ifdef DEBUG
  if (m_index != result_t::NullIndex) {
#endif // DEBUG
    result_value_t *p = s_ResultValue.data() + m_index;
    fn(s_ResultValue[m_index].m_Data);
    p->m_Head = std::bit_cast<uint64_t>(s_ResultValue_head.ptr);
    s_ResultValue_head.ptr = p;
#ifdef DEBUG
  }
#endif // DEBUG
}
INLINE void result_t::remove() {
#ifdef DEBUG
  if (m_index != result_t::NullIndex) {
#endif // DEBUG
    result_value_t *p = s_ResultValue.data() + m_index;
    p->m_Head = std::bit_cast<uint64_t>(s_ResultValue_head.ptr);
    s_ResultValue_head.ptr = p;
#ifdef DEBUG
  }
#endif // DEBUG
}
} // namespace blt
#endif // !BL_UTIL_RESULT_TYPE_FILE
