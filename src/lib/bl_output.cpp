#include <bl_output.hpp>

namespace BL {
#if IS_WINDOWS
WORD getColorCode(ConsoleColor color) {
    using CC = ConsoleColor;
    switch (color) {
        case CC::Green:
            return FOREGROUND_GREEN;
        case CC::Black:
            return 0;
        case CC::Blue:
            return FOREGROUND_BLUE;
        case CC::Gray:
            return FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
        case CC::Purple:
            return FOREGROUND_BLUE | FOREGROUND_RED;
        case CC::Red:
            return FOREGROUND_RED;
        case CC::White:
            return FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN;
        case CC::Cyan:
            return FOREGROUND_BLUE | FOREGROUND_GREEN;
        case CC::Yellow:
            return FOREGROUND_RED | FOREGROUND_GREEN;
        case CC::None:
            return FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN;
        case CC::GreenIntensity:
            return FOREGROUND_GREEN | FOREGROUND_INTENSITY;
        case CC::BlackIntensity:
            return 0;
        case CC::BlueIntensity:
            return FOREGROUND_BLUE | FOREGROUND_INTENSITY;
        case CC::GrayIntensity:
            return FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED |
                   FOREGROUND_INTENSITY;
        case CC::PurpleIntensity:
            return FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY;
        case CC::RedIntensity:
            return FOREGROUND_RED | FOREGROUND_INTENSITY;
        case CC::WhiteIntensity:
            return FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN |
                   FOREGROUND_INTENSITY;
        case CC::YellowIntensity:
            return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
        case CC::CyanIntensity:
            return FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
        default:
            return 0;
    }
}
#else
std::string getColorCode(ConsoleColor color) {
    using CC = ConsoleColor;
    switch (color) {
        case CC::Green:
            return "\033[32m";
        case CC::Black:
            return "\033[30m";
        case CC::Blue:
            return "\033[34m";
        case CC::Gray:
            return "\033[37m";
        case CC::Purple:
            return "\033[35m";
        case CC::Red:
            return "\033[31m";
        case CC::White:
            return "\033[37m";
        case CC::Cyan:
            return "\033[36m";
        case CC::Yellow:
            return "\033[33m";
        case CC::None:
            return "\033[0m";
        case CC::GreenIntensity:
            return "\033[32m;1m";
        case CC::BlackIntensity:
            return "\033[30m;1m";
        case CC::BlueIntensity:
            return "\033[34m;1m";
        case CC::GrayIntensity:
            return "\033[37m;1m";
        case CC::PurpleIntensity:
            return "\033[35m;1m";
        case CC::RedIntensity:
            return "\033[31m;1m";
        case CC::WhiteIntensity:
            return "\033[37m;1m";
        case CC::YellowIntensity:
            return "\033[33m;1m";
        case CC::CyanIntensity:
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
        case BC::Green:
            return "\033[42m";
        case BC::Black:
            return "\033[40m";
        case BC::Blue:
            return "\033[44m";
        case BC::Gray:
            return "\033[40m";
        case BC::Purple:
            return "\033[45m";
        case BC::Red:
            return "\033[41m";
        case BC::White:
            return "\033[47m";
        case BC::Cyan:
            return "\033[46m";
        case BC::Yellow:
            return "\033[43m";
        case BC::None:
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
} // namespace BL