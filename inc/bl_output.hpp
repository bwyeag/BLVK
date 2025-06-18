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
#ifndef BL_OUTPUT_HPP_FILE
#define BL_OUTPUT_HPP_FILE
#include <chrono>
#include <initializer_list>
#include <iomanip>
#include <iostream>
#include <source_location>
#include <system_error>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) ||                 \
    defined(__NT__) && !defined(__CYGWIN__)
#define IS_WINDOWS
#include <Windows.h>
#endif
namespace BL {
enum class ConsoleColor {
  green,
  red,
  blue,
  white,
  black,
  yellow,
  purple,
  gray,
  cyan,
  none,
  green_intensity,
  red_intensity,
  blue_intensity,
  white_intensity,
  black_intensity,
  yellow_intensity,
  purple_intensity,
  gray_intensity,
  cyan_intensity
};

enum class ConsoleBackgroundColor {
  green,
  red,
  blue,
  white,
  black,
  yellow,
  purple,
  gray,
  cyan,
  none
};

#ifdef IS_WINDOWS
WORD getColorCode(ConsoleColor color);
WORD getBackgroundColorCode(ConsoleBackgroundColor color);
#else
const char *getColorCode(ConsoleColor color);
const char *getBackgroundColorCode(ConsoleBackgroundColor color);
#endif

std::ostream &operator<<(std::ostream &os, ConsoleColor data);
std::ostream &operator<<(std::ostream &os, ConsoleBackgroundColor data);

namespace _internal {
// 打印文件位置
inline void print_source_loc(std::ostream &stm,
                             const std::source_location &loc) {
  stm << '[' << loc.file_name() << "->" << loc.function_name()
      << "|L:" << loc.line() << ']';
}
// 打印时间点
inline void print_time(std::ostream &stm) {
  auto now = std::chrono::system_clock::now();
  auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
      now.time_since_epoch());
  std::time_t t = std::chrono::system_clock::to_time_t(now);
  stm << '[';
  stm << std::put_time(std::localtime(&t), "%Y-%m-%d %H:%M:%S.")
      << (now_ms.count() % 1000) << ']';
}
// 打印错误
template <typename... Types>
void print_error_internal(const std::source_location loc, const char *type,
                          const Types &...args) {
  std::cerr << ConsoleColor::red;
  print_time(std::cerr);
  print_source_loc(std::cerr, loc);
  std::cerr << '[' << type << ']' << ConsoleColor::none;
  std::initializer_list<int>{([&args] { std::cerr << args << ' '; }(), 0)...};
  std::cerr << '\n';
}
// 打印警告
template <typename... Types>
void print_warning_internal(const std::source_location loc, const char *type,
                            const Types &...args) {
  std::cerr << ConsoleColor::yellow;
  print_time(std::cerr);
  print_source_loc(std::cerr, loc);
  std::cerr << '[' << type << ']' << ConsoleColor::none;
  std::initializer_list<int>{([&args] { std::cerr << args << ' '; }(), 0)...};
  std::cerr << '\n';
}
// 打印输出
template <typename... Types>
void print_log_internal(const char *type, const Types &...args) {
  std::cout << ConsoleColor::green << '[' << type << ']' << ConsoleColor::none;
  std::initializer_list<int>{([&args] { std::cout << args << ' '; }(), 0)...};
  std::cout << '\n';
}

template <typename T, typename Arg, typename... OtherArgs>
constexpr static bool is_all_same =
    std::is_same_v<T, Arg> &&
    (sizeof...(OtherArgs) > 0 ? is_all_same<T, OtherArgs...> : true);
template <typename... Types>
void print_errorcode_internal(const Types &...ecs) {
  static_assert(_internal::is_all_same<std::error_code, Types...>,
                "Wrong Argument Types!");
  std::cerr << ConsoleColor::red;
  print_time(std::cerr);
  std::initializer_list<int>{(
      [&ecs] {
        std::cerr << ConsoleColor::red << '[' << ecs.category().name() << ']'
                  << ConsoleColor::none << ecs.message() << '\n';
      }(),
      0)...};
  std::cerr << '\n';
}
} // namespace _internal
#define print_error(type, ...)                                                 \
  _internal::print_error_internal(std::source_location::current(), type,       \
                                  __VA_ARGS__)
#define print_warning(type, ...)                                               \
  _internal::print_error_internal(std::source_location::current(), type,       \
                                  __VA_ARGS__)
#define print_log(type, ...) _internal::print_log_internal(type, __VA_ARGS__)
#define print_errorcode(...) _internal::print_errorcode_internal(__VA_ARGS__)
} // namespace BL
#undef IS_WINDOWS
#endif //! BL_OUTPUT_HPP_FILE
