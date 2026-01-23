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

// todo:  avoid of allocing heap memory;
//        reduce static memory usage;

#include <ostream>
#include <string>
#ifndef BL_UTIL_RESULT_TYPE_FILE
#include <utils/bl_macro.hpp>
// 第三方库include
#include "vulkan/vk_enum_string_helper.h"
// 标准库include
#include <array>
#include <bit>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <source_location>
#include <stdexcept>
#include <type_traits>
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
  FileStructError = -2,
  FileHeadError = -3
};
template <> struct ResultEnumTrait<LoadResult> {
  static constexpr bool is_error_enum = true;
  static constexpr uint16_t category_id = 3;
};
//*****************************************************************************
// result_t
//*****************************************************************************
constexpr int err_value_data_entry_count = 4;
struct result_value_entry {
  const char *name{nullptr};
  char str[16]{'\0'};
};
struct result_value_data_t {
  const char *file_name{nullptr};
  const char *function_name{nullptr};
  uint64_t line{0};
  uint32_t entry_count{0};
  std::array<result_value_entry, err_value_data_entry_count> entries;

  result_value_data_t &
  fill(std::source_location loc = std::source_location::current()) {
    file_name = loc.file_name();
    function_name = loc.function_name();
    line = loc.line();
    return *this;
  }
  template <typename T> result_value_data_t &insert(const char *name, T value) {
#ifdef DEBUG
    if (entry_count >= err_value_data_entry_count)
      throw std::logic_error("too many entry used");
#endif // DEBUG
    entries[entry_count].name = name,
    entries[entry_count++].str = std::to_string(value); // <- heap memory
  }
  void dump(std::ostream &stm, int i) const {
    auto Get = [](const char *s) { return (s ? s : "Unknown"); };
    stm << '[' << i << "] " << Get(file_name) << ':' << line << '@'
        << Get(function_name) << '\n';
    for (uint32_t k = 0; k < entry_count; ++k)
      stm << '\t' << entries[k].name << ':' << entries[k].str << '\n';
  }
};
struct [[nodiscard]] result_t {
  static constexpr uint32_t NullIndex = 0xffff;
  uint32_t m_index : 16;    // 具体错误信息的索引, 0xffff表示无信息
  uint32_t m_category : 16; // 指向result_category_t的数组下标
  int32_t m_type;           // 错误类型编码

  // constructors
  constexpr result_t()
      : m_category(0u), m_index(result_t::NullIndex), m_type(0) {}
  template <typename T>
    requires ResultEnumTrait<T>::is_error_enum &&
             (!ResultEnumTrait<T>::has_detail_info)
  constexpr result_t(const T &errc);
  constexpr result_t(const result_t &) = default;
  constexpr result_t(result_t &&) = default;
  // destructor
  constexpr ~result_t() = default;
  // operators
  constexpr operator uint64_t() const;
  constexpr operator bool() const;
  template <typename T>
    requires ResultEnumTrait<T>::is_error_enum
  constexpr bool operator==(T errc) const;
  // observers
  const char *category() const;
  const char *message() const;
  result_value_data_t &acquire();
  INLINE constexpr bool is_installed() const;
  // modifiers
  result_value_data_t &install(const result_t *next = nullptr);
  void remove();
  template <typename T, typename Func>
    requires ResultEnumTrait<T>::is_error_enum &&
             std::is_invocable<Func, result_value_data_t &>::value
  result_t &forward(T errc, Func &&fn);
  template <typename T>
    requires ResultEnumTrait<T>::is_error_enum &&
             (!ResultEnumTrait<T>::has_detail_info)
  constexpr void set(T errc);
  constexpr void clear();

  template <typename Func>
    requires std::is_invocable<Func, const result_value_data_t &,
                               uint32_t>::value
  void iterate(Func &&fn = [](const result_value_data_t &val, uint32_t i) {
    val.dump(std::cout, i);
  });

private:
  result_value_data_t &install_internal(const result_t *next);
};
static_assert(sizeof(result_t) == 8);
template <typename T> result_t make_result(T errc) { return result_t{errc}; }
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
  result_head_t(); // execute s_ResultValue_head, s_ResultValue initialization
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
INLINE constexpr bool result_t::is_installed() const {
  return m_index != NullIndex;
}
INLINE constexpr result_t::operator uint64_t() const {
  return std::bit_cast<uint64_t>(*this);
}
INLINE constexpr result_t::operator bool() const { return m_type == 0; }
template <typename T>
  requires ResultEnumTrait<T>::is_error_enum
INLINE constexpr bool result_t::operator==(T errc) const {
  return (m_category == ResultEnumTrait<T>::category_id &&
          m_type == static_cast<int32_t>(errc));
}
template <typename T>
  requires ResultEnumTrait<T>::is_error_enum &&
               (!ResultEnumTrait<T>::has_detail_info)
INLINE constexpr result_t::result_t(const T &errc)
    : m_category(ResultEnumTrait<T>::category_id),
      m_type(static_cast<int32_t>(errc)), m_index(result_t::NullIndex) {}
INLINE result_value_data_t &result_t::install(const result_t *next) {
#ifdef DEBUG
  if (m_index != result_t::NullIndex)
    throw std::logic_error("re-install result_t");
#endif // DEBUG
  return install_internal(next);
}
template <typename T, typename Func>
  requires ResultEnumTrait<T>::is_error_enum &&
           std::is_invocable<Func, result_value_data_t &>::value
result_t &result_t::forward(T errc, Func &&fn) {
#ifdef DEBUG
  if (m_index == result_t::NullIndex)
    throw std::logic_error("uninstalled result_t");
#endif // DEBUG
  result_t self = *this;
  m_category = ResultEnumTrait<T>::category_id;
  m_type = static_cast<int32_t>(errc);
  (void)fn(install_internal(&self));
  return *this;
}
template <typename T>
  requires ResultEnumTrait<T>::is_error_enum &&
           (!ResultEnumTrait<T>::has_detail_info)
constexpr void result_t::set(T errc) {
  m_category = ResultEnumTrait<T>::category_id,
  m_type = static_cast<int32_t>(errc), m_index = result_t::NullIndex;
}
INLINE constexpr void result_t::clear() { m_category = 0, m_type = 0; }

template <typename Func>
  requires std::is_invocable<Func, const result_value_data_t &, uint32_t>::value
void result_t::iterate(Func &&fn) {
  uint32_t i = 0;
  result_value_t *p = s_ResultValue.data() + m_index;
  while (p)
    (void)fn(p->m_Data, i++), p = p->m_NextVal;
}
} // namespace blt
#endif // ! BL_UTIL_RESULT_TYPE_FILE
