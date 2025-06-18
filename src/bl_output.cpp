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
#include <bl_output.hpp>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) ||                 \
    defined(__NT__) && !defined(__CYGWIN__)
#define IS_WINDOWS
#include <Windows.h>
#endif

namespace BL {
#if IS_WINDOWS
WORD getColorCode(ConsoleColor color) {
    using CC = ConsoleColor;
    switch (color) {
        case CC::green:
            return FOREGROUND_GREEN;
        case CC::black:
            return 0;
        case CC::blue:
            return FOREGROUND_BLUE;
        case CC::gray:
            return FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
        case CC::purple:
            return FOREGROUND_BLUE | FOREGROUND_RED;
        case CC::red:
            return FOREGROUND_RED;
        case CC::white:
            return FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN;
        case CC::cyan:
            return FOREGROUND_BLUE | FOREGROUND_GREEN;
        case CC::yellow:
            return FOREGROUND_RED | FOREGROUND_GREEN;
        case CC::none:
            return FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN;
        case CC::green_intensity:
            return FOREGROUND_GREEN | FOREGROUND_INTENSITY;
        case CC::BlackIntensity:
            return 0;
        case CC::blue_intensity:
            return FOREGROUND_BLUE | FOREGROUND_INTENSITY;
        case CC::gray_intensity:
            return FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED |
                   FOREGROUND_INTENSITY;
        case CC::purple_intensity:
            return FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY;
        case CC::red_intensity:
            return FOREGROUND_RED | FOREGROUND_INTENSITY;
        case CC::white_intensity:
            return FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN |
                   FOREGROUND_INTENSITY;
        case CC::yellow_intensity:
            return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
        case CC::cyan_intensity:
            return FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
        default:
            return 0;
    }
}
#else
std::string getColorCode(ConsoleColor color) {
    using CC = ConsoleColor;
    switch (color) {
        case CC::green:
            return "\033[32m";
        case CC::black:
            return "\033[30m";
        case CC::blue:
            return "\033[34m";
        case CC::gray:
            return "\033[37m";
        case CC::purple:
            return "\033[35m";
        case CC::red:
            return "\033[31m";
        case CC::white:
            return "\033[37m";
        case CC::cyan:
            return "\033[36m";
        case CC::yellow:
            return "\033[33m";
        case CC::none:
            return "\033[0m";
        case CC::green_intensity:
            return "\033[32m;1m";
        case CC::black_intensity:
            return "\033[30m;1m";
        case CC::blue_intensity:
            return "\033[34m;1m";
        case CC::gray_intensity:
            return "\033[37m;1m";
        case CC::purple_intensity:
            return "\033[35m;1m";
        case CC::red_intensity:
            return "\033[31m;1m";
        case CC::white_intensity:
            return "\033[37m;1m";
        case CC::yellow_intensity:
            return "\033[33m;1m";
        case CC::cyan_intensity:
            return "\033[36m;1m";
        default:
            return 0;
    }
}
#endif

#if IS_WINDOWS
WORD getBackgroundColorCode(ConsoleBackgroundColor color) {
    using BC = ConsoleBackgroundColor;
    switch (color) {
        case BC::Green:
            return BACKGROUND_GREEN;
        case BC::Black:
            return 0;
        case BC::Blue:
            return BACKGROUND_BLUE;
        case BC::Gray:
            return 0;
        case BC::Purple:
            return BACKGROUND_RED | BACKGROUND_BLUE;
        case BC::Red:
            return BACKGROUND_RED;
        case BC::White:
            return BACKGROUND_RED | BACKGROUND_BLUE | BACKGROUND_GREEN;
        case BC::Cyan:
            return BACKGROUND_BLUE | BACKGROUND_GREEN;
        case BC::Yellow:
            return BACKGROUND_RED | BACKGROUND_GREEN;
        case BC::None:
            return 0;
        default:
            return 0;
    }
}
#else
const char* getBackgroundColorCode(ConsoleBackgroundColor color) {
    using BC = ConsoleBackgroundColor;
    switch (color) {
        case BC::green:
            return "\033[42m";
        case BC::black:
            return "\033[40m";
        case BC::blue:
            return "\033[44m";
        case BC::gray:
            return "\033[40m";
        case BC::purple:
            return "\033[45m";
        case BC::red:
            return "\033[41m";
        case BC::white:
            return "\033[47m";
        case BC::cyan:
            return "\033[46m";
        case BC::yellow:
            return "\033[43m";
        case BC::none:
            return "\033[40m";
        default:
            return 0;
    }
}
#endif
std::ostream& operator<<(std::ostream& os, ConsoleColor data) {
#if IS_WINDOWS
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(handle, getColorCode(data));
#else
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    os << GetColorCode(data);
#endif
    return os;
}
std::ostream& operator<<(std::ostream& os, ConsoleBackgroundColor data) {
#if IS_WINDOWS
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(handle, getBackgroundColorCode(data));
#else
    os << GetBackgroundColorCode(data);
#endif
    return os;
}
#undef IS_WINDOWS
} // namespace BL
