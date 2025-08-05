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
#include <utils/bl_output.hpp>
#include <clocale>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) ||                 \
    defined(__NT__) && !defined(__CYGWIN__)
#define IS_WINDOWS
#include <Windows.h>
#endif

namespace blt {
void output_setup() {
#ifdef IS_WINDOWS
  std::setlocale(LC_ALL, ".utf-8");
  SetConsoleOutputCP(CP_UTF8);
#endif
  std::cout.sync_with_stdio(false);
}
#ifdef IS_WINDOWS
WORD _get_color_code(ConsoleColor color) {
  using enum ConsoleColor;
  switch (color) {
  case green:
    return FOREGROUND_GREEN;
  case black:
    return 0;
  case blue:
    return FOREGROUND_BLUE;
  case gray:
    return FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
  case purple:
    return FOREGROUND_BLUE | FOREGROUND_RED;
  case red:
    return FOREGROUND_RED;
  case white:
    return FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN;
  case cyan:
    return FOREGROUND_BLUE | FOREGROUND_GREEN;
  case yellow:
    return FOREGROUND_RED | FOREGROUND_GREEN;
  case none:
    return FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN;
  case green_intensity:
    return FOREGROUND_GREEN | FOREGROUND_INTENSITY;
  case black_intensity:
    return 0;
  case blue_intensity:
    return FOREGROUND_BLUE | FOREGROUND_INTENSITY;
  case gray_intensity:
    return FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED |
           FOREGROUND_INTENSITY;
  case purple_intensity:
    return FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY;
  case red_intensity:
    return FOREGROUND_RED | FOREGROUND_INTENSITY;
  case white_intensity:
    return FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN |
           FOREGROUND_INTENSITY;
  case yellow_intensity:
    return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
  case cyan_intensity:
    return FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
  default:
    return 0;
  }
}
#else
std::string _get_color_code(ConsoleColor color) {
  using enum ConsoleColor;
  switch (color) {
  case green:
    return "\033[32m";
  case black:
    return "\033[30m";
  case blue:
    return "\033[34m";
  case gray:
    return "\033[37m";
  case purple:
    return "\033[35m";
  case red:
    return "\033[31m";
  case white:
    return "\033[37m";
  case cyan:
    return "\033[36m";
  case yellow:
    return "\033[33m";
  case none:
    return "\033[0m";
  case green_intensity:
    return "\033[32m;1m";
  case black_intensity:
    return "\033[30m;1m";
  case blue_intensity:
    return "\033[34m;1m";
  case gray_intensity:
    return "\033[37m;1m";
  case purple_intensity:
    return "\033[35m;1m";
  case red_intensity:
    return "\033[31m;1m";
  case white_intensity:
    return "\033[37m;1m";
  case yellow_intensity:
    return "\033[33m;1m";
  case cyan_intensity:
    return "\033[36m;1m";
  default:
    return 0;
  }
}
#endif

#ifdef IS_WINDOWS
WORD _get_background_color_code(ConsoleBackgroundColor color) {
  using enum ConsoleBackgroundColor;
  switch (color) {
  case green:
    return BACKGROUND_GREEN;
  case black:
    return 0;
  case blue:
    return BACKGROUND_BLUE;
  case gray:
    return 0;
  case purple:
    return BACKGROUND_RED | BACKGROUND_BLUE;
  case red:
    return BACKGROUND_RED;
  case white:
    return BACKGROUND_RED | BACKGROUND_BLUE | BACKGROUND_GREEN;
  case cyan:
    return BACKGROUND_BLUE | BACKGROUND_GREEN;
  case yellow:
    return BACKGROUND_RED | BACKGROUND_GREEN;
  case none:
    return 0;
  default:
    return 0;
  }
}
#else
const char *_get_background_color_code(ConsoleBackgroundColor color) {
  using enum ConsoleBackgroundColor;
  switch (color) {
  case green:
    return "\033[42m";
  case black:
    return "\033[40m";
  case blue:
    return "\033[44m";
  case gray:
    return "\033[40m";
  case purple:
    return "\033[45m";
  case red:
    return "\033[41m";
  case white:
    return "\033[47m";
  case cyan:
    return "\033[46m";
  case yellow:
    return "\033[43m";
  case none:
    return "\033[40m";
  default:
    return 0;
  }
}
#endif
std::ostream &operator<<(std::ostream &os, ConsoleColor data) {
#ifdef IS_WINDOWS
  HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
  SetConsoleTextAttribute(handle, _get_color_code(data));
#else
  HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
  os << _get_color_code(data);
#endif
  return os;
}
std::ostream &operator<<(std::ostream &os, ConsoleBackgroundColor data) {
#ifdef IS_WINDOWS
  HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
  SetConsoleTextAttribute(handle, _get_background_color_code(data));
#else
  os << _get_background_color_code(data);
#endif
  return os;
}
#undef IS_WINDOWS
} // namespace blt
