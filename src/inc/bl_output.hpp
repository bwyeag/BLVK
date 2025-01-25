#ifndef BL_OUTPUT_HPP_FILE
#define BL_OUTPUT_HPP_FILE
#include <chrono>
#include <initializer_list>
#include <iomanip>
#include <iostream>
#include <source_location>
#include <string>
#include <system_error>
#define IS_WINDOWS                                             \
    defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || \
        defined(__NT__) && !defined(__CYGWIN__)
#if IS_WINDOWS
#include <Windows.h>
#endif  // IS_WINDOWS
namespace BL {
enum class ConsoleColor {
    Green,
    Red,
    Blue,
    White,
    Black,
    Yellow,
    Purple,
    Gray,
    Cyan,
    None,
    GreenIntensity,
    RedIntensity,
    BlueIntensity,
    WhiteIntensity,
    BlackIntensity,
    YellowIntensity,
    PurpleIntensity,
    GrayIntensity,
    CyanIntensity
};

enum class ConsoleBackgroundColor {
    Green,
    Red,
    Blue,
    White,
    Black,
    Yellow,
    Purple,
    Gray,
    Cyan,
    None
};

#if IS_WINDOWS
WORD getColorCode(ConsoleColor color);
WORD getBackgroundColorCode(ConsoleBackgroundColor color);
#else
const char* getColorCode(ConsoleColor color);
const char* getBackgroundColorCode(ConsoleBackgroundColor color);
#endif
std::ostream& operator<<(std::ostream& os, ConsoleColor data);
std::ostream& operator<<(std::ostream& os, ConsoleBackgroundColor data);

namespace _internal {
// 打印文件位置
inline void print_source_loc(std::ostream& stm,
                             const std::source_location& loc) {
    stm << '[' << loc.file_name() << "->" << loc.function_name()
        << "|L:" << loc.line() << ']';
}
// 打印时间点
inline void print_time(std::ostream& stm) {
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
void print_error_internal(const std::source_location loc,
                          const char* type,
                          const Types&... args) {
    std::cerr << ConsoleColor::Red;
    print_time(std::cerr);
    print_source_loc(std::cerr, loc);
    std::cerr << '[' << type << ']' << ConsoleColor::None;
    std::initializer_list<int>{([&args] { std::cerr << args << ' '; }(), 0)...};
    std::cerr << '\n';
}
// 打印警告
template <typename... Types>
void print_warning_internal(const std::source_location loc,
                            const char* type,
                            const Types&... args) {
    std::cerr << ConsoleColor::Yellow;
    print_time(std::cerr);
    print_source_loc(std::cerr, loc);
    std::cerr << '[' << type << ']' << ConsoleColor::None;
    std::initializer_list<int>{([&args] { std::cerr << args << ' '; }(), 0)...};
    std::cerr << '\n';
}
// 打印输出
template <typename... Types>
void print_log_internal(const char* type, const Types&... args) {
    std::cout << ConsoleColor::Green << '[' << type << ']'
              << ConsoleColor::None;
    std::initializer_list<int>{([&args] { std::cout << args << ' '; }(), 0)...};
    std::cout << '\n';
}

template <typename T, typename Arg, typename... OtherArgs>
constexpr static bool is_all_same =
    std::is_same_v<T, Args> &&
    (sizeof...(OtherArgs) > 0 ? is_all_same<T, OtherArgs...> : true);
template <typename... Types>
void print_errorcode_internal(const Types&... ecs) {
    static_assert(_internal::is_all_same<std::error_code, Types...>,
                  "Wrong Argument Types!");
    std::cerr << ConsoleColor::Red;
    print_time(std::cerr);
    std::initializer_list<int>{(
        [&ecs] {
            std::cerr << ConsoleColor::Red << '[' << ecs.category().name()
                      << ']' << ConsoleColor::None << ecs.message() << '\n';
        }(),
        0)...};
    std::cerr << '\n';
}
}  // namespace _internal
#define print_error(type, ...)                                             \
    _internal::print_error_internal(std::source_location::current(), type, \
                                    __VA_ARGS__)
#define print_warning(type, ...)                                           \
    _internal::print_error_internal(std::source_location::current(), type, \
                                    __VA_ARGS__)
#define print_log(type, ...) _internal::print_log_internal(type, __VA_ARGS__)
#define print_errorcode(...) _internal::print_errorcode_internal(__VA_ARGS__)
}  // namespace BL
#endif  //! BL_OUTPUT_HPP_FILE