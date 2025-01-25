#ifndef BL_REFLECT_HPP_FILE
#define BL_REFLECT_HPP_FILE
#include <array>
#include <cstdint>
#include <sstream>
#include <string_view>
#include <type_traits>
namespace BL {
struct _any_t {
    template <typename T>
    constexpr operator T() {}
};
// 编译时计算类型成员数量
template <typename T, typename... Args>
consteval size_t count_members() {
    if constexpr (requires() { T{Args{}..., _any_t{}}; })
        return count_members<T, Args..., _any_t>();
    else
        return sizeof...(Args);
}
// 获取编译时类型成员数量
template <typename T>
constexpr static size_t member_count = count_members<T>();

// 成员变量名称类型
template <size_t N>
using reflect_names_t = std::array<const char*, N>;
// 基本名称
constexpr static char base_name[] = "Member";
// 生成自动变量名字
template <size_t N>
consteval reflect_names_t<N> generate_names() {
    reflect_names_t<N> result;
    for (size_t i = 0; i < N; i++) {
        result[i] = base_name;
    }
    return result;
}

// 标记类型是否有反射数据
template <typename T>
constexpr bool reflect_registered = false;
// 反射类型变量名称
template <typename T>
constexpr static reflect_names_t<member_count<T>> reflect_names =
    generate_names<member_count<T>>();
// 一个帮手宏，帮助实现快捷的反射登记
#define REFLECT_REGISTER(Type, ...)                                            \
    template <>                                                                \
    constexpr bool reflect_registered<Type> = true;                            \
    template <>                                                                \
    constexpr static reflect_names_t<member_count<Type>> reflect_names<Type> = \
        {__VA_ARGS__};
// 遍历一个类型的成员变量
// 使用 helpers\reflect_gen.py 生成，此处最大64个成员
template <typename Visitor>
constexpr void visit_members(const auto&& object, Visitor&& func) {
    using T = std::decay_t<decltype(object)>;
    constexpr size_t cnt = member_count<T>;
    reflect_names_t<cnt>& names = reflect_names<T>;
    if constexpr (cnt == 0) {
        return;
    } else if constexpr (cnt == 1) {
        const auto& [m0] = object;
        func(m0, names[0]);
    } else if constexpr (cnt == 2) {
        const auto& [m0, m1] = object;
        func(m0, names[0]);
        func(m1, names[1]);
    } else if constexpr (cnt == 3) {
        const auto& [m0, m1, m2] = object;
        func(m0, names[0]);
        func(m1, names[1]);
        func(m2, names[2]);
    } else if constexpr (cnt == 4) {
        const auto& [m0, m1, m2, m3] = object;
        func(m0, names[0]);
        func(m1, names[1]);
        func(m2, names[2]);
        func(m3, names[3]);
    } else if constexpr (cnt == 5) {
        const auto& [m0, m1, m2, m3, m4] = object;
        func(m0, names[0]);
        func(m1, names[1]);
        func(m2, names[2]);
        func(m3, names[3]);
        func(m4, names[4]);
    } else if constexpr (cnt == 6) {
        const auto& [m0, m1, m2, m3, m4, m5] = object;
        func(m0, names[0]);
        func(m1, names[1]);
        func(m2, names[2]);
        func(m3, names[3]);
        func(m4, names[4]);
        func(m5, names[5]);
    } else if constexpr (cnt == 7) {
        const auto& [m0, m1, m2, m3, m4, m5, m6] = object;
        func(m0, names[0]);
        func(m1, names[1]);
        func(m2, names[2]);
        func(m3, names[3]);
        func(m4, names[4]);
        func(m5, names[5]);
        func(m6, names[6]);
    } else if constexpr (cnt == 8) {
        const auto& [m0, m1, m2, m3, m4, m5, m6, m7] = object;
        func(m0, names[0]);
        func(m1, names[1]);
        func(m2, names[2]);
        func(m3, names[3]);
        func(m4, names[4]);
        func(m5, names[5]);
        func(m6, names[6]);
        func(m7, names[7]);
    } else if constexpr (cnt == 9) {
        const auto& [m0, m1, m2, m3, m4, m5, m6, m7, m8] = object;
        func(m0, names[0]);
        func(m1, names[1]);
        func(m2, names[2]);
        func(m3, names[3]);
        func(m4, names[4]);
        func(m5, names[5]);
        func(m6, names[6]);
        func(m7, names[7]);
        func(m8, names[8]);
    } else if constexpr (cnt == 10) {
        const auto& [m0, m1, m2, m3, m4, m5, m6, m7, m8, m9] = object;
        func(m0, names[0]);
        func(m1, names[1]);
        func(m2, names[2]);
        func(m3, names[3]);
        func(m4, names[4]);
        func(m5, names[5]);
        func(m6, names[6]);
        func(m7, names[7]);
        func(m8, names[8]);
        func(m9, names[9]);
    } else if constexpr (cnt == 11) {
        const auto& [m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10] = object;
        func(m0, names[0]);
        func(m1, names[1]);
        func(m2, names[2]);
        func(m3, names[3]);
        func(m4, names[4]);
        func(m5, names[5]);
        func(m6, names[6]);
        func(m7, names[7]);
        func(m8, names[8]);
        func(m9, names[9]);
        func(m10, names[10]);
    } else if constexpr (cnt == 12) {
        const auto& [m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11] = object;
        func(m0, names[0]);
        func(m1, names[1]);
        func(m2, names[2]);
        func(m3, names[3]);
        func(m4, names[4]);
        func(m5, names[5]);
        func(m6, names[6]);
        func(m7, names[7]);
        func(m8, names[8]);
        func(m9, names[9]);
        func(m10, names[10]);
        func(m11, names[11]);
    } else if constexpr (cnt == 13) {
        const auto& [m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12] =
            object;
        func(m0, names[0]);
        func(m1, names[1]);
        func(m2, names[2]);
        func(m3, names[3]);
        func(m4, names[4]);
        func(m5, names[5]);
        func(m6, names[6]);
        func(m7, names[7]);
        func(m8, names[8]);
        func(m9, names[9]);
        func(m10, names[10]);
        func(m11, names[11]);
        func(m12, names[12]);
    } else if constexpr (cnt == 14) {
        const auto& [m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12,
                     m13] = object;
        func(m0, names[0]);
        func(m1, names[1]);
        func(m2, names[2]);
        func(m3, names[3]);
        func(m4, names[4]);
        func(m5, names[5]);
        func(m6, names[6]);
        func(m7, names[7]);
        func(m8, names[8]);
        func(m9, names[9]);
        func(m10, names[10]);
        func(m11, names[11]);
        func(m12, names[12]);
        func(m13, names[13]);
    } else if constexpr (cnt == 15) {
        const auto& [m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13,
                     m14] = object;
        func(m0, names[0]);
        func(m1, names[1]);
        func(m2, names[2]);
        func(m3, names[3]);
        func(m4, names[4]);
        func(m5, names[5]);
        func(m6, names[6]);
        func(m7, names[7]);
        func(m8, names[8]);
        func(m9, names[9]);
        func(m10, names[10]);
        func(m11, names[11]);
        func(m12, names[12]);
        func(m13, names[13]);
        func(m14, names[14]);
    } else if constexpr (cnt == 16) {
        const auto& [m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13,
                     m14, m15] = object;
        func(m0, names[0]);
        func(m1, names[1]);
        func(m2, names[2]);
        func(m3, names[3]);
        func(m4, names[4]);
        func(m5, names[5]);
        func(m6, names[6]);
        func(m7, names[7]);
        func(m8, names[8]);
        func(m9, names[9]);
        func(m10, names[10]);
        func(m11, names[11]);
        func(m12, names[12]);
        func(m13, names[13]);
        func(m14, names[14]);
        func(m15, names[15]);
    } else if constexpr (cnt == 17) {
        const auto& [m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13,
                     m14, m15, m16] = object;
        func(m0, names[0]);
        func(m1, names[1]);
        func(m2, names[2]);
        func(m3, names[3]);
        func(m4, names[4]);
        func(m5, names[5]);
        func(m6, names[6]);
        func(m7, names[7]);
        func(m8, names[8]);
        func(m9, names[9]);
        func(m10, names[10]);
        func(m11, names[11]);
        func(m12, names[12]);
        func(m13, names[13]);
        func(m14, names[14]);
        func(m15, names[15]);
        func(m16, names[16]);
    } else if constexpr (cnt == 18) {
        const auto& [m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13,
                     m14, m15, m16, m17] = object;
        func(m0, names[0]);
        func(m1, names[1]);
        func(m2, names[2]);
        func(m3, names[3]);
        func(m4, names[4]);
        func(m5, names[5]);
        func(m6, names[6]);
        func(m7, names[7]);
        func(m8, names[8]);
        func(m9, names[9]);
        func(m10, names[10]);
        func(m11, names[11]);
        func(m12, names[12]);
        func(m13, names[13]);
        func(m14, names[14]);
        func(m15, names[15]);
        func(m16, names[16]);
        func(m17, names[17]);
    } else if constexpr (cnt == 19) {
        const auto& [m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13,
                     m14, m15, m16, m17, m18] = object;
        func(m0, names[0]);
        func(m1, names[1]);
        func(m2, names[2]);
        func(m3, names[3]);
        func(m4, names[4]);
        func(m5, names[5]);
        func(m6, names[6]);
        func(m7, names[7]);
        func(m8, names[8]);
        func(m9, names[9]);
        func(m10, names[10]);
        func(m11, names[11]);
        func(m12, names[12]);
        func(m13, names[13]);
        func(m14, names[14]);
        func(m15, names[15]);
        func(m16, names[16]);
        func(m17, names[17]);
        func(m18, names[18]);
    } else if constexpr (cnt == 20) {
        const auto& [m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13,
                     m14, m15, m16, m17, m18, m19] = object;
        func(m0, names[0]);
        func(m1, names[1]);
        func(m2, names[2]);
        func(m3, names[3]);
        func(m4, names[4]);
        func(m5, names[5]);
        func(m6, names[6]);
        func(m7, names[7]);
        func(m8, names[8]);
        func(m9, names[9]);
        func(m10, names[10]);
        func(m11, names[11]);
        func(m12, names[12]);
        func(m13, names[13]);
        func(m14, names[14]);
        func(m15, names[15]);
        func(m16, names[16]);
        func(m17, names[17]);
        func(m18, names[18]);
        func(m19, names[19]);
    } else if constexpr (cnt == 21) {
        const auto& [m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13,
                     m14, m15, m16, m17, m18, m19, m20] = object;
        func(m0, names[0]);
        func(m1, names[1]);
        func(m2, names[2]);
        func(m3, names[3]);
        func(m4, names[4]);
        func(m5, names[5]);
        func(m6, names[6]);
        func(m7, names[7]);
        func(m8, names[8]);
        func(m9, names[9]);
        func(m10, names[10]);
        func(m11, names[11]);
        func(m12, names[12]);
        func(m13, names[13]);
        func(m14, names[14]);
        func(m15, names[15]);
        func(m16, names[16]);
        func(m17, names[17]);
        func(m18, names[18]);
        func(m19, names[19]);
        func(m20, names[20]);
    } else if constexpr (cnt == 22) {
        const auto& [m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13,
                     m14, m15, m16, m17, m18, m19, m20, m21] = object;
        func(m0, names[0]);
        func(m1, names[1]);
        func(m2, names[2]);
        func(m3, names[3]);
        func(m4, names[4]);
        func(m5, names[5]);
        func(m6, names[6]);
        func(m7, names[7]);
        func(m8, names[8]);
        func(m9, names[9]);
        func(m10, names[10]);
        func(m11, names[11]);
        func(m12, names[12]);
        func(m13, names[13]);
        func(m14, names[14]);
        func(m15, names[15]);
        func(m16, names[16]);
        func(m17, names[17]);
        func(m18, names[18]);
        func(m19, names[19]);
        func(m20, names[20]);
        func(m21, names[21]);
    } else if constexpr (cnt == 23) {
        const auto& [m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13,
                     m14, m15, m16, m17, m18, m19, m20, m21, m22] = object;
        func(m0, names[0]);
        func(m1, names[1]);
        func(m2, names[2]);
        func(m3, names[3]);
        func(m4, names[4]);
        func(m5, names[5]);
        func(m6, names[6]);
        func(m7, names[7]);
        func(m8, names[8]);
        func(m9, names[9]);
        func(m10, names[10]);
        func(m11, names[11]);
        func(m12, names[12]);
        func(m13, names[13]);
        func(m14, names[14]);
        func(m15, names[15]);
        func(m16, names[16]);
        func(m17, names[17]);
        func(m18, names[18]);
        func(m19, names[19]);
        func(m20, names[20]);
        func(m21, names[21]);
        func(m22, names[22]);
    } else if constexpr (cnt == 24) {
        const auto& [m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13,
                     m14, m15, m16, m17, m18, m19, m20, m21, m22, m23] = object;
        func(m0, names[0]);
        func(m1, names[1]);
        func(m2, names[2]);
        func(m3, names[3]);
        func(m4, names[4]);
        func(m5, names[5]);
        func(m6, names[6]);
        func(m7, names[7]);
        func(m8, names[8]);
        func(m9, names[9]);
        func(m10, names[10]);
        func(m11, names[11]);
        func(m12, names[12]);
        func(m13, names[13]);
        func(m14, names[14]);
        func(m15, names[15]);
        func(m16, names[16]);
        func(m17, names[17]);
        func(m18, names[18]);
        func(m19, names[19]);
        func(m20, names[20]);
        func(m21, names[21]);
        func(m22, names[22]);
        func(m23, names[23]);
    } else if constexpr (cnt == 25) {
        const auto& [m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13,
                     m14, m15, m16, m17, m18, m19, m20, m21, m22, m23, m24] =
            object;
        func(m0, names[0]);
        func(m1, names[1]);
        func(m2, names[2]);
        func(m3, names[3]);
        func(m4, names[4]);
        func(m5, names[5]);
        func(m6, names[6]);
        func(m7, names[7]);
        func(m8, names[8]);
        func(m9, names[9]);
        func(m10, names[10]);
        func(m11, names[11]);
        func(m12, names[12]);
        func(m13, names[13]);
        func(m14, names[14]);
        func(m15, names[15]);
        func(m16, names[16]);
        func(m17, names[17]);
        func(m18, names[18]);
        func(m19, names[19]);
        func(m20, names[20]);
        func(m21, names[21]);
        func(m22, names[22]);
        func(m23, names[23]);
        func(m24, names[24]);
    } else if constexpr (cnt == 26) {
        const auto& [m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13,
                     m14, m15, m16, m17, m18, m19, m20, m21, m22, m23, m24,
                     m25] = object;
        func(m0, names[0]);
        func(m1, names[1]);
        func(m2, names[2]);
        func(m3, names[3]);
        func(m4, names[4]);
        func(m5, names[5]);
        func(m6, names[6]);
        func(m7, names[7]);
        func(m8, names[8]);
        func(m9, names[9]);
        func(m10, names[10]);
        func(m11, names[11]);
        func(m12, names[12]);
        func(m13, names[13]);
        func(m14, names[14]);
        func(m15, names[15]);
        func(m16, names[16]);
        func(m17, names[17]);
        func(m18, names[18]);
        func(m19, names[19]);
        func(m20, names[20]);
        func(m21, names[21]);
        func(m22, names[22]);
        func(m23, names[23]);
        func(m24, names[24]);
        func(m25, names[25]);
    } else if constexpr (cnt == 27) {
        const auto& [m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13,
                     m14, m15, m16, m17, m18, m19, m20, m21, m22, m23, m24, m25,
                     m26] = object;
        func(m0, names[0]);
        func(m1, names[1]);
        func(m2, names[2]);
        func(m3, names[3]);
        func(m4, names[4]);
        func(m5, names[5]);
        func(m6, names[6]);
        func(m7, names[7]);
        func(m8, names[8]);
        func(m9, names[9]);
        func(m10, names[10]);
        func(m11, names[11]);
        func(m12, names[12]);
        func(m13, names[13]);
        func(m14, names[14]);
        func(m15, names[15]);
        func(m16, names[16]);
        func(m17, names[17]);
        func(m18, names[18]);
        func(m19, names[19]);
        func(m20, names[20]);
        func(m21, names[21]);
        func(m22, names[22]);
        func(m23, names[23]);
        func(m24, names[24]);
        func(m25, names[25]);
        func(m26, names[26]);
    } else if constexpr (cnt == 28) {
        const auto& [m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13,
                     m14, m15, m16, m17, m18, m19, m20, m21, m22, m23, m24, m25,
                     m26, m27] = object;
        func(m0, names[0]);
        func(m1, names[1]);
        func(m2, names[2]);
        func(m3, names[3]);
        func(m4, names[4]);
        func(m5, names[5]);
        func(m6, names[6]);
        func(m7, names[7]);
        func(m8, names[8]);
        func(m9, names[9]);
        func(m10, names[10]);
        func(m11, names[11]);
        func(m12, names[12]);
        func(m13, names[13]);
        func(m14, names[14]);
        func(m15, names[15]);
        func(m16, names[16]);
        func(m17, names[17]);
        func(m18, names[18]);
        func(m19, names[19]);
        func(m20, names[20]);
        func(m21, names[21]);
        func(m22, names[22]);
        func(m23, names[23]);
        func(m24, names[24]);
        func(m25, names[25]);
        func(m26, names[26]);
        func(m27, names[27]);
    } else if constexpr (cnt == 29) {
        const auto& [m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13,
                     m14, m15, m16, m17, m18, m19, m20, m21, m22, m23, m24, m25,
                     m26, m27, m28] = object;
        func(m0, names[0]);
        func(m1, names[1]);
        func(m2, names[2]);
        func(m3, names[3]);
        func(m4, names[4]);
        func(m5, names[5]);
        func(m6, names[6]);
        func(m7, names[7]);
        func(m8, names[8]);
        func(m9, names[9]);
        func(m10, names[10]);
        func(m11, names[11]);
        func(m12, names[12]);
        func(m13, names[13]);
        func(m14, names[14]);
        func(m15, names[15]);
        func(m16, names[16]);
        func(m17, names[17]);
        func(m18, names[18]);
        func(m19, names[19]);
        func(m20, names[20]);
        func(m21, names[21]);
        func(m22, names[22]);
        func(m23, names[23]);
        func(m24, names[24]);
        func(m25, names[25]);
        func(m26, names[26]);
        func(m27, names[27]);
        func(m28, names[28]);
    } else if constexpr (cnt == 30) {
        const auto& [m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13,
                     m14, m15, m16, m17, m18, m19, m20, m21, m22, m23, m24, m25,
                     m26, m27, m28, m29] = object;
        func(m0, names[0]);
        func(m1, names[1]);
        func(m2, names[2]);
        func(m3, names[3]);
        func(m4, names[4]);
        func(m5, names[5]);
        func(m6, names[6]);
        func(m7, names[7]);
        func(m8, names[8]);
        func(m9, names[9]);
        func(m10, names[10]);
        func(m11, names[11]);
        func(m12, names[12]);
        func(m13, names[13]);
        func(m14, names[14]);
        func(m15, names[15]);
        func(m16, names[16]);
        func(m17, names[17]);
        func(m18, names[18]);
        func(m19, names[19]);
        func(m20, names[20]);
        func(m21, names[21]);
        func(m22, names[22]);
        func(m23, names[23]);
        func(m24, names[24]);
        func(m25, names[25]);
        func(m26, names[26]);
        func(m27, names[27]);
        func(m28, names[28]);
        func(m29, names[29]);
    } else if constexpr (cnt == 31) {
        const auto& [m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13,
                     m14, m15, m16, m17, m18, m19, m20, m21, m22, m23, m24, m25,
                     m26, m27, m28, m29, m30] = object;
        func(m0, names[0]);
        func(m1, names[1]);
        func(m2, names[2]);
        func(m3, names[3]);
        func(m4, names[4]);
        func(m5, names[5]);
        func(m6, names[6]);
        func(m7, names[7]);
        func(m8, names[8]);
        func(m9, names[9]);
        func(m10, names[10]);
        func(m11, names[11]);
        func(m12, names[12]);
        func(m13, names[13]);
        func(m14, names[14]);
        func(m15, names[15]);
        func(m16, names[16]);
        func(m17, names[17]);
        func(m18, names[18]);
        func(m19, names[19]);
        func(m20, names[20]);
        func(m21, names[21]);
        func(m22, names[22]);
        func(m23, names[23]);
        func(m24, names[24]);
        func(m25, names[25]);
        func(m26, names[26]);
        func(m27, names[27]);
        func(m28, names[28]);
        func(m29, names[29]);
        func(m30, names[30]);
    } else if constexpr (cnt == 32) {
        const auto& [m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13,
                     m14, m15, m16, m17, m18, m19, m20, m21, m22, m23, m24, m25,
                     m26, m27, m28, m29, m30, m31] = object;
        func(m0, names[0]);
        func(m1, names[1]);
        func(m2, names[2]);
        func(m3, names[3]);
        func(m4, names[4]);
        func(m5, names[5]);
        func(m6, names[6]);
        func(m7, names[7]);
        func(m8, names[8]);
        func(m9, names[9]);
        func(m10, names[10]);
        func(m11, names[11]);
        func(m12, names[12]);
        func(m13, names[13]);
        func(m14, names[14]);
        func(m15, names[15]);
        func(m16, names[16]);
        func(m17, names[17]);
        func(m18, names[18]);
        func(m19, names[19]);
        func(m20, names[20]);
        func(m21, names[21]);
        func(m22, names[22]);
        func(m23, names[23]);
        func(m24, names[24]);
        func(m25, names[25]);
        func(m26, names[26]);
        func(m27, names[27]);
        func(m28, names[28]);
        func(m29, names[29]);
        func(m30, names[30]);
        func(m31, names[31]);
    } else if constexpr (cnt == 33) {
        const auto& [m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13,
                     m14, m15, m16, m17, m18, m19, m20, m21, m22, m23, m24, m25,
                     m26, m27, m28, m29, m30, m31, m32] = object;
        func(m0, names[0]);
        func(m1, names[1]);
        func(m2, names[2]);
        func(m3, names[3]);
        func(m4, names[4]);
        func(m5, names[5]);
        func(m6, names[6]);
        func(m7, names[7]);
        func(m8, names[8]);
        func(m9, names[9]);
        func(m10, names[10]);
        func(m11, names[11]);
        func(m12, names[12]);
        func(m13, names[13]);
        func(m14, names[14]);
        func(m15, names[15]);
        func(m16, names[16]);
        func(m17, names[17]);
        func(m18, names[18]);
        func(m19, names[19]);
        func(m20, names[20]);
        func(m21, names[21]);
        func(m22, names[22]);
        func(m23, names[23]);
        func(m24, names[24]);
        func(m25, names[25]);
        func(m26, names[26]);
        func(m27, names[27]);
        func(m28, names[28]);
        func(m29, names[29]);
        func(m30, names[30]);
        func(m31, names[31]);
        func(m32, names[32]);
    } else if constexpr (cnt == 34) {
        const auto& [m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13,
                     m14, m15, m16, m17, m18, m19, m20, m21, m22, m23, m24, m25,
                     m26, m27, m28, m29, m30, m31, m32, m33] = object;
        func(m0, names[0]);
        func(m1, names[1]);
        func(m2, names[2]);
        func(m3, names[3]);
        func(m4, names[4]);
        func(m5, names[5]);
        func(m6, names[6]);
        func(m7, names[7]);
        func(m8, names[8]);
        func(m9, names[9]);
        func(m10, names[10]);
        func(m11, names[11]);
        func(m12, names[12]);
        func(m13, names[13]);
        func(m14, names[14]);
        func(m15, names[15]);
        func(m16, names[16]);
        func(m17, names[17]);
        func(m18, names[18]);
        func(m19, names[19]);
        func(m20, names[20]);
        func(m21, names[21]);
        func(m22, names[22]);
        func(m23, names[23]);
        func(m24, names[24]);
        func(m25, names[25]);
        func(m26, names[26]);
        func(m27, names[27]);
        func(m28, names[28]);
        func(m29, names[29]);
        func(m30, names[30]);
        func(m31, names[31]);
        func(m32, names[32]);
        func(m33, names[33]);
    } else if constexpr (cnt == 35) {
        const auto& [m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13,
                     m14, m15, m16, m17, m18, m19, m20, m21, m22, m23, m24, m25,
                     m26, m27, m28, m29, m30, m31, m32, m33, m34] = object;
        func(m0, names[0]);
        func(m1, names[1]);
        func(m2, names[2]);
        func(m3, names[3]);
        func(m4, names[4]);
        func(m5, names[5]);
        func(m6, names[6]);
        func(m7, names[7]);
        func(m8, names[8]);
        func(m9, names[9]);
        func(m10, names[10]);
        func(m11, names[11]);
        func(m12, names[12]);
        func(m13, names[13]);
        func(m14, names[14]);
        func(m15, names[15]);
        func(m16, names[16]);
        func(m17, names[17]);
        func(m18, names[18]);
        func(m19, names[19]);
        func(m20, names[20]);
        func(m21, names[21]);
        func(m22, names[22]);
        func(m23, names[23]);
        func(m24, names[24]);
        func(m25, names[25]);
        func(m26, names[26]);
        func(m27, names[27]);
        func(m28, names[28]);
        func(m29, names[29]);
        func(m30, names[30]);
        func(m31, names[31]);
        func(m32, names[32]);
        func(m33, names[33]);
        func(m34, names[34]);
    } else if constexpr (cnt == 36) {
        const auto& [m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13,
                     m14, m15, m16, m17, m18, m19, m20, m21, m22, m23, m24, m25,
                     m26, m27, m28, m29, m30, m31, m32, m33, m34, m35] = object;
        func(m0, names[0]);
        func(m1, names[1]);
        func(m2, names[2]);
        func(m3, names[3]);
        func(m4, names[4]);
        func(m5, names[5]);
        func(m6, names[6]);
        func(m7, names[7]);
        func(m8, names[8]);
        func(m9, names[9]);
        func(m10, names[10]);
        func(m11, names[11]);
        func(m12, names[12]);
        func(m13, names[13]);
        func(m14, names[14]);
        func(m15, names[15]);
        func(m16, names[16]);
        func(m17, names[17]);
        func(m18, names[18]);
        func(m19, names[19]);
        func(m20, names[20]);
        func(m21, names[21]);
        func(m22, names[22]);
        func(m23, names[23]);
        func(m24, names[24]);
        func(m25, names[25]);
        func(m26, names[26]);
        func(m27, names[27]);
        func(m28, names[28]);
        func(m29, names[29]);
        func(m30, names[30]);
        func(m31, names[31]);
        func(m32, names[32]);
        func(m33, names[33]);
        func(m34, names[34]);
        func(m35, names[35]);
    } else if constexpr (cnt == 37) {
        const auto& [m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13,
                     m14, m15, m16, m17, m18, m19, m20, m21, m22, m23, m24, m25,
                     m26, m27, m28, m29, m30, m31, m32, m33, m34, m35, m36] =
            object;
        func(m0, names[0]);
        func(m1, names[1]);
        func(m2, names[2]);
        func(m3, names[3]);
        func(m4, names[4]);
        func(m5, names[5]);
        func(m6, names[6]);
        func(m7, names[7]);
        func(m8, names[8]);
        func(m9, names[9]);
        func(m10, names[10]);
        func(m11, names[11]);
        func(m12, names[12]);
        func(m13, names[13]);
        func(m14, names[14]);
        func(m15, names[15]);
        func(m16, names[16]);
        func(m17, names[17]);
        func(m18, names[18]);
        func(m19, names[19]);
        func(m20, names[20]);
        func(m21, names[21]);
        func(m22, names[22]);
        func(m23, names[23]);
        func(m24, names[24]);
        func(m25, names[25]);
        func(m26, names[26]);
        func(m27, names[27]);
        func(m28, names[28]);
        func(m29, names[29]);
        func(m30, names[30]);
        func(m31, names[31]);
        func(m32, names[32]);
        func(m33, names[33]);
        func(m34, names[34]);
        func(m35, names[35]);
        func(m36, names[36]);
    } else if constexpr (cnt == 38) {
        const auto& [m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13,
                     m14, m15, m16, m17, m18, m19, m20, m21, m22, m23, m24, m25,
                     m26, m27, m28, m29, m30, m31, m32, m33, m34, m35, m36,
                     m37] = object;
        func(m0, names[0]);
        func(m1, names[1]);
        func(m2, names[2]);
        func(m3, names[3]);
        func(m4, names[4]);
        func(m5, names[5]);
        func(m6, names[6]);
        func(m7, names[7]);
        func(m8, names[8]);
        func(m9, names[9]);
        func(m10, names[10]);
        func(m11, names[11]);
        func(m12, names[12]);
        func(m13, names[13]);
        func(m14, names[14]);
        func(m15, names[15]);
        func(m16, names[16]);
        func(m17, names[17]);
        func(m18, names[18]);
        func(m19, names[19]);
        func(m20, names[20]);
        func(m21, names[21]);
        func(m22, names[22]);
        func(m23, names[23]);
        func(m24, names[24]);
        func(m25, names[25]);
        func(m26, names[26]);
        func(m27, names[27]);
        func(m28, names[28]);
        func(m29, names[29]);
        func(m30, names[30]);
        func(m31, names[31]);
        func(m32, names[32]);
        func(m33, names[33]);
        func(m34, names[34]);
        func(m35, names[35]);
        func(m36, names[36]);
        func(m37, names[37]);
    } else if constexpr (cnt == 39) {
        const auto& [m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13,
                     m14, m15, m16, m17, m18, m19, m20, m21, m22, m23, m24, m25,
                     m26, m27, m28, m29, m30, m31, m32, m33, m34, m35, m36, m37,
                     m38] = object;
        func(m0, names[0]);
        func(m1, names[1]);
        func(m2, names[2]);
        func(m3, names[3]);
        func(m4, names[4]);
        func(m5, names[5]);
        func(m6, names[6]);
        func(m7, names[7]);
        func(m8, names[8]);
        func(m9, names[9]);
        func(m10, names[10]);
        func(m11, names[11]);
        func(m12, names[12]);
        func(m13, names[13]);
        func(m14, names[14]);
        func(m15, names[15]);
        func(m16, names[16]);
        func(m17, names[17]);
        func(m18, names[18]);
        func(m19, names[19]);
        func(m20, names[20]);
        func(m21, names[21]);
        func(m22, names[22]);
        func(m23, names[23]);
        func(m24, names[24]);
        func(m25, names[25]);
        func(m26, names[26]);
        func(m27, names[27]);
        func(m28, names[28]);
        func(m29, names[29]);
        func(m30, names[30]);
        func(m31, names[31]);
        func(m32, names[32]);
        func(m33, names[33]);
        func(m34, names[34]);
        func(m35, names[35]);
        func(m36, names[36]);
        func(m37, names[37]);
        func(m38, names[38]);
    } else if constexpr (cnt == 40) {
        const auto& [m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13,
                     m14, m15, m16, m17, m18, m19, m20, m21, m22, m23, m24, m25,
                     m26, m27, m28, m29, m30, m31, m32, m33, m34, m35, m36, m37,
                     m38, m39] = object;
        func(m0, names[0]);
        func(m1, names[1]);
        func(m2, names[2]);
        func(m3, names[3]);
        func(m4, names[4]);
        func(m5, names[5]);
        func(m6, names[6]);
        func(m7, names[7]);
        func(m8, names[8]);
        func(m9, names[9]);
        func(m10, names[10]);
        func(m11, names[11]);
        func(m12, names[12]);
        func(m13, names[13]);
        func(m14, names[14]);
        func(m15, names[15]);
        func(m16, names[16]);
        func(m17, names[17]);
        func(m18, names[18]);
        func(m19, names[19]);
        func(m20, names[20]);
        func(m21, names[21]);
        func(m22, names[22]);
        func(m23, names[23]);
        func(m24, names[24]);
        func(m25, names[25]);
        func(m26, names[26]);
        func(m27, names[27]);
        func(m28, names[28]);
        func(m29, names[29]);
        func(m30, names[30]);
        func(m31, names[31]);
        func(m32, names[32]);
        func(m33, names[33]);
        func(m34, names[34]);
        func(m35, names[35]);
        func(m36, names[36]);
        func(m37, names[37]);
        func(m38, names[38]);
        func(m39, names[39]);
    } else if constexpr (cnt == 41) {
        const auto& [m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13,
                     m14, m15, m16, m17, m18, m19, m20, m21, m22, m23, m24, m25,
                     m26, m27, m28, m29, m30, m31, m32, m33, m34, m35, m36, m37,
                     m38, m39, m40] = object;
        func(m0, names[0]);
        func(m1, names[1]);
        func(m2, names[2]);
        func(m3, names[3]);
        func(m4, names[4]);
        func(m5, names[5]);
        func(m6, names[6]);
        func(m7, names[7]);
        func(m8, names[8]);
        func(m9, names[9]);
        func(m10, names[10]);
        func(m11, names[11]);
        func(m12, names[12]);
        func(m13, names[13]);
        func(m14, names[14]);
        func(m15, names[15]);
        func(m16, names[16]);
        func(m17, names[17]);
        func(m18, names[18]);
        func(m19, names[19]);
        func(m20, names[20]);
        func(m21, names[21]);
        func(m22, names[22]);
        func(m23, names[23]);
        func(m24, names[24]);
        func(m25, names[25]);
        func(m26, names[26]);
        func(m27, names[27]);
        func(m28, names[28]);
        func(m29, names[29]);
        func(m30, names[30]);
        func(m31, names[31]);
        func(m32, names[32]);
        func(m33, names[33]);
        func(m34, names[34]);
        func(m35, names[35]);
        func(m36, names[36]);
        func(m37, names[37]);
        func(m38, names[38]);
        func(m39, names[39]);
        func(m40, names[40]);
    } else if constexpr (cnt == 42) {
        const auto& [m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13,
                     m14, m15, m16, m17, m18, m19, m20, m21, m22, m23, m24, m25,
                     m26, m27, m28, m29, m30, m31, m32, m33, m34, m35, m36, m37,
                     m38, m39, m40, m41] = object;
        func(m0, names[0]);
        func(m1, names[1]);
        func(m2, names[2]);
        func(m3, names[3]);
        func(m4, names[4]);
        func(m5, names[5]);
        func(m6, names[6]);
        func(m7, names[7]);
        func(m8, names[8]);
        func(m9, names[9]);
        func(m10, names[10]);
        func(m11, names[11]);
        func(m12, names[12]);
        func(m13, names[13]);
        func(m14, names[14]);
        func(m15, names[15]);
        func(m16, names[16]);
        func(m17, names[17]);
        func(m18, names[18]);
        func(m19, names[19]);
        func(m20, names[20]);
        func(m21, names[21]);
        func(m22, names[22]);
        func(m23, names[23]);
        func(m24, names[24]);
        func(m25, names[25]);
        func(m26, names[26]);
        func(m27, names[27]);
        func(m28, names[28]);
        func(m29, names[29]);
        func(m30, names[30]);
        func(m31, names[31]);
        func(m32, names[32]);
        func(m33, names[33]);
        func(m34, names[34]);
        func(m35, names[35]);
        func(m36, names[36]);
        func(m37, names[37]);
        func(m38, names[38]);
        func(m39, names[39]);
        func(m40, names[40]);
        func(m41, names[41]);
    } else if constexpr (cnt == 43) {
        const auto& [m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13,
                     m14, m15, m16, m17, m18, m19, m20, m21, m22, m23, m24, m25,
                     m26, m27, m28, m29, m30, m31, m32, m33, m34, m35, m36, m37,
                     m38, m39, m40, m41, m42] = object;
        func(m0, names[0]);
        func(m1, names[1]);
        func(m2, names[2]);
        func(m3, names[3]);
        func(m4, names[4]);
        func(m5, names[5]);
        func(m6, names[6]);
        func(m7, names[7]);
        func(m8, names[8]);
        func(m9, names[9]);
        func(m10, names[10]);
        func(m11, names[11]);
        func(m12, names[12]);
        func(m13, names[13]);
        func(m14, names[14]);
        func(m15, names[15]);
        func(m16, names[16]);
        func(m17, names[17]);
        func(m18, names[18]);
        func(m19, names[19]);
        func(m20, names[20]);
        func(m21, names[21]);
        func(m22, names[22]);
        func(m23, names[23]);
        func(m24, names[24]);
        func(m25, names[25]);
        func(m26, names[26]);
        func(m27, names[27]);
        func(m28, names[28]);
        func(m29, names[29]);
        func(m30, names[30]);
        func(m31, names[31]);
        func(m32, names[32]);
        func(m33, names[33]);
        func(m34, names[34]);
        func(m35, names[35]);
        func(m36, names[36]);
        func(m37, names[37]);
        func(m38, names[38]);
        func(m39, names[39]);
        func(m40, names[40]);
        func(m41, names[41]);
        func(m42, names[42]);
    } else if constexpr (cnt == 44) {
        const auto& [m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13,
                     m14, m15, m16, m17, m18, m19, m20, m21, m22, m23, m24, m25,
                     m26, m27, m28, m29, m30, m31, m32, m33, m34, m35, m36, m37,
                     m38, m39, m40, m41, m42, m43] = object;
        func(m0, names[0]);
        func(m1, names[1]);
        func(m2, names[2]);
        func(m3, names[3]);
        func(m4, names[4]);
        func(m5, names[5]);
        func(m6, names[6]);
        func(m7, names[7]);
        func(m8, names[8]);
        func(m9, names[9]);
        func(m10, names[10]);
        func(m11, names[11]);
        func(m12, names[12]);
        func(m13, names[13]);
        func(m14, names[14]);
        func(m15, names[15]);
        func(m16, names[16]);
        func(m17, names[17]);
        func(m18, names[18]);
        func(m19, names[19]);
        func(m20, names[20]);
        func(m21, names[21]);
        func(m22, names[22]);
        func(m23, names[23]);
        func(m24, names[24]);
        func(m25, names[25]);
        func(m26, names[26]);
        func(m27, names[27]);
        func(m28, names[28]);
        func(m29, names[29]);
        func(m30, names[30]);
        func(m31, names[31]);
        func(m32, names[32]);
        func(m33, names[33]);
        func(m34, names[34]);
        func(m35, names[35]);
        func(m36, names[36]);
        func(m37, names[37]);
        func(m38, names[38]);
        func(m39, names[39]);
        func(m40, names[40]);
        func(m41, names[41]);
        func(m42, names[42]);
        func(m43, names[43]);
    } else if constexpr (cnt == 45) {
        const auto& [m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13,
                     m14, m15, m16, m17, m18, m19, m20, m21, m22, m23, m24, m25,
                     m26, m27, m28, m29, m30, m31, m32, m33, m34, m35, m36, m37,
                     m38, m39, m40, m41, m42, m43, m44] = object;
        func(m0, names[0]);
        func(m1, names[1]);
        func(m2, names[2]);
        func(m3, names[3]);
        func(m4, names[4]);
        func(m5, names[5]);
        func(m6, names[6]);
        func(m7, names[7]);
        func(m8, names[8]);
        func(m9, names[9]);
        func(m10, names[10]);
        func(m11, names[11]);
        func(m12, names[12]);
        func(m13, names[13]);
        func(m14, names[14]);
        func(m15, names[15]);
        func(m16, names[16]);
        func(m17, names[17]);
        func(m18, names[18]);
        func(m19, names[19]);
        func(m20, names[20]);
        func(m21, names[21]);
        func(m22, names[22]);
        func(m23, names[23]);
        func(m24, names[24]);
        func(m25, names[25]);
        func(m26, names[26]);
        func(m27, names[27]);
        func(m28, names[28]);
        func(m29, names[29]);
        func(m30, names[30]);
        func(m31, names[31]);
        func(m32, names[32]);
        func(m33, names[33]);
        func(m34, names[34]);
        func(m35, names[35]);
        func(m36, names[36]);
        func(m37, names[37]);
        func(m38, names[38]);
        func(m39, names[39]);
        func(m40, names[40]);
        func(m41, names[41]);
        func(m42, names[42]);
        func(m43, names[43]);
        func(m44, names[44]);
    } else if constexpr (cnt == 46) {
        const auto& [m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13,
                     m14, m15, m16, m17, m18, m19, m20, m21, m22, m23, m24, m25,
                     m26, m27, m28, m29, m30, m31, m32, m33, m34, m35, m36, m37,
                     m38, m39, m40, m41, m42, m43, m44, m45] = object;
        func(m0, names[0]);
        func(m1, names[1]);
        func(m2, names[2]);
        func(m3, names[3]);
        func(m4, names[4]);
        func(m5, names[5]);
        func(m6, names[6]);
        func(m7, names[7]);
        func(m8, names[8]);
        func(m9, names[9]);
        func(m10, names[10]);
        func(m11, names[11]);
        func(m12, names[12]);
        func(m13, names[13]);
        func(m14, names[14]);
        func(m15, names[15]);
        func(m16, names[16]);
        func(m17, names[17]);
        func(m18, names[18]);
        func(m19, names[19]);
        func(m20, names[20]);
        func(m21, names[21]);
        func(m22, names[22]);
        func(m23, names[23]);
        func(m24, names[24]);
        func(m25, names[25]);
        func(m26, names[26]);
        func(m27, names[27]);
        func(m28, names[28]);
        func(m29, names[29]);
        func(m30, names[30]);
        func(m31, names[31]);
        func(m32, names[32]);
        func(m33, names[33]);
        func(m34, names[34]);
        func(m35, names[35]);
        func(m36, names[36]);
        func(m37, names[37]);
        func(m38, names[38]);
        func(m39, names[39]);
        func(m40, names[40]);
        func(m41, names[41]);
        func(m42, names[42]);
        func(m43, names[43]);
        func(m44, names[44]);
        func(m45, names[45]);
    } else if constexpr (cnt == 47) {
        const auto& [m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13,
                     m14, m15, m16, m17, m18, m19, m20, m21, m22, m23, m24, m25,
                     m26, m27, m28, m29, m30, m31, m32, m33, m34, m35, m36, m37,
                     m38, m39, m40, m41, m42, m43, m44, m45, m46] = object;
        func(m0, names[0]);
        func(m1, names[1]);
        func(m2, names[2]);
        func(m3, names[3]);
        func(m4, names[4]);
        func(m5, names[5]);
        func(m6, names[6]);
        func(m7, names[7]);
        func(m8, names[8]);
        func(m9, names[9]);
        func(m10, names[10]);
        func(m11, names[11]);
        func(m12, names[12]);
        func(m13, names[13]);
        func(m14, names[14]);
        func(m15, names[15]);
        func(m16, names[16]);
        func(m17, names[17]);
        func(m18, names[18]);
        func(m19, names[19]);
        func(m20, names[20]);
        func(m21, names[21]);
        func(m22, names[22]);
        func(m23, names[23]);
        func(m24, names[24]);
        func(m25, names[25]);
        func(m26, names[26]);
        func(m27, names[27]);
        func(m28, names[28]);
        func(m29, names[29]);
        func(m30, names[30]);
        func(m31, names[31]);
        func(m32, names[32]);
        func(m33, names[33]);
        func(m34, names[34]);
        func(m35, names[35]);
        func(m36, names[36]);
        func(m37, names[37]);
        func(m38, names[38]);
        func(m39, names[39]);
        func(m40, names[40]);
        func(m41, names[41]);
        func(m42, names[42]);
        func(m43, names[43]);
        func(m44, names[44]);
        func(m45, names[45]);
        func(m46, names[46]);
    } else if constexpr (cnt == 48) {
        const auto& [m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13,
                     m14, m15, m16, m17, m18, m19, m20, m21, m22, m23, m24, m25,
                     m26, m27, m28, m29, m30, m31, m32, m33, m34, m35, m36, m37,
                     m38, m39, m40, m41, m42, m43, m44, m45, m46, m47] = object;
        func(m0, names[0]);
        func(m1, names[1]);
        func(m2, names[2]);
        func(m3, names[3]);
        func(m4, names[4]);
        func(m5, names[5]);
        func(m6, names[6]);
        func(m7, names[7]);
        func(m8, names[8]);
        func(m9, names[9]);
        func(m10, names[10]);
        func(m11, names[11]);
        func(m12, names[12]);
        func(m13, names[13]);
        func(m14, names[14]);
        func(m15, names[15]);
        func(m16, names[16]);
        func(m17, names[17]);
        func(m18, names[18]);
        func(m19, names[19]);
        func(m20, names[20]);
        func(m21, names[21]);
        func(m22, names[22]);
        func(m23, names[23]);
        func(m24, names[24]);
        func(m25, names[25]);
        func(m26, names[26]);
        func(m27, names[27]);
        func(m28, names[28]);
        func(m29, names[29]);
        func(m30, names[30]);
        func(m31, names[31]);
        func(m32, names[32]);
        func(m33, names[33]);
        func(m34, names[34]);
        func(m35, names[35]);
        func(m36, names[36]);
        func(m37, names[37]);
        func(m38, names[38]);
        func(m39, names[39]);
        func(m40, names[40]);
        func(m41, names[41]);
        func(m42, names[42]);
        func(m43, names[43]);
        func(m44, names[44]);
        func(m45, names[45]);
        func(m46, names[46]);
        func(m47, names[47]);
    } else if constexpr (cnt == 49) {
        const auto& [m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13,
                     m14, m15, m16, m17, m18, m19, m20, m21, m22, m23, m24, m25,
                     m26, m27, m28, m29, m30, m31, m32, m33, m34, m35, m36, m37,
                     m38, m39, m40, m41, m42, m43, m44, m45, m46, m47, m48] =
            object;
        func(m0, names[0]);
        func(m1, names[1]);
        func(m2, names[2]);
        func(m3, names[3]);
        func(m4, names[4]);
        func(m5, names[5]);
        func(m6, names[6]);
        func(m7, names[7]);
        func(m8, names[8]);
        func(m9, names[9]);
        func(m10, names[10]);
        func(m11, names[11]);
        func(m12, names[12]);
        func(m13, names[13]);
        func(m14, names[14]);
        func(m15, names[15]);
        func(m16, names[16]);
        func(m17, names[17]);
        func(m18, names[18]);
        func(m19, names[19]);
        func(m20, names[20]);
        func(m21, names[21]);
        func(m22, names[22]);
        func(m23, names[23]);
        func(m24, names[24]);
        func(m25, names[25]);
        func(m26, names[26]);
        func(m27, names[27]);
        func(m28, names[28]);
        func(m29, names[29]);
        func(m30, names[30]);
        func(m31, names[31]);
        func(m32, names[32]);
        func(m33, names[33]);
        func(m34, names[34]);
        func(m35, names[35]);
        func(m36, names[36]);
        func(m37, names[37]);
        func(m38, names[38]);
        func(m39, names[39]);
        func(m40, names[40]);
        func(m41, names[41]);
        func(m42, names[42]);
        func(m43, names[43]);
        func(m44, names[44]);
        func(m45, names[45]);
        func(m46, names[46]);
        func(m47, names[47]);
        func(m48, names[48]);
    } else if constexpr (cnt == 50) {
        const auto& [m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13,
                     m14, m15, m16, m17, m18, m19, m20, m21, m22, m23, m24, m25,
                     m26, m27, m28, m29, m30, m31, m32, m33, m34, m35, m36, m37,
                     m38, m39, m40, m41, m42, m43, m44, m45, m46, m47, m48,
                     m49] = object;
        func(m0, names[0]);
        func(m1, names[1]);
        func(m2, names[2]);
        func(m3, names[3]);
        func(m4, names[4]);
        func(m5, names[5]);
        func(m6, names[6]);
        func(m7, names[7]);
        func(m8, names[8]);
        func(m9, names[9]);
        func(m10, names[10]);
        func(m11, names[11]);
        func(m12, names[12]);
        func(m13, names[13]);
        func(m14, names[14]);
        func(m15, names[15]);
        func(m16, names[16]);
        func(m17, names[17]);
        func(m18, names[18]);
        func(m19, names[19]);
        func(m20, names[20]);
        func(m21, names[21]);
        func(m22, names[22]);
        func(m23, names[23]);
        func(m24, names[24]);
        func(m25, names[25]);
        func(m26, names[26]);
        func(m27, names[27]);
        func(m28, names[28]);
        func(m29, names[29]);
        func(m30, names[30]);
        func(m31, names[31]);
        func(m32, names[32]);
        func(m33, names[33]);
        func(m34, names[34]);
        func(m35, names[35]);
        func(m36, names[36]);
        func(m37, names[37]);
        func(m38, names[38]);
        func(m39, names[39]);
        func(m40, names[40]);
        func(m41, names[41]);
        func(m42, names[42]);
        func(m43, names[43]);
        func(m44, names[44]);
        func(m45, names[45]);
        func(m46, names[46]);
        func(m47, names[47]);
        func(m48, names[48]);
        func(m49, names[49]);
    } else if constexpr (cnt == 51) {
        const auto& [m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13,
                     m14, m15, m16, m17, m18, m19, m20, m21, m22, m23, m24, m25,
                     m26, m27, m28, m29, m30, m31, m32, m33, m34, m35, m36, m37,
                     m38, m39, m40, m41, m42, m43, m44, m45, m46, m47, m48, m49,
                     m50] = object;
        func(m0, names[0]);
        func(m1, names[1]);
        func(m2, names[2]);
        func(m3, names[3]);
        func(m4, names[4]);
        func(m5, names[5]);
        func(m6, names[6]);
        func(m7, names[7]);
        func(m8, names[8]);
        func(m9, names[9]);
        func(m10, names[10]);
        func(m11, names[11]);
        func(m12, names[12]);
        func(m13, names[13]);
        func(m14, names[14]);
        func(m15, names[15]);
        func(m16, names[16]);
        func(m17, names[17]);
        func(m18, names[18]);
        func(m19, names[19]);
        func(m20, names[20]);
        func(m21, names[21]);
        func(m22, names[22]);
        func(m23, names[23]);
        func(m24, names[24]);
        func(m25, names[25]);
        func(m26, names[26]);
        func(m27, names[27]);
        func(m28, names[28]);
        func(m29, names[29]);
        func(m30, names[30]);
        func(m31, names[31]);
        func(m32, names[32]);
        func(m33, names[33]);
        func(m34, names[34]);
        func(m35, names[35]);
        func(m36, names[36]);
        func(m37, names[37]);
        func(m38, names[38]);
        func(m39, names[39]);
        func(m40, names[40]);
        func(m41, names[41]);
        func(m42, names[42]);
        func(m43, names[43]);
        func(m44, names[44]);
        func(m45, names[45]);
        func(m46, names[46]);
        func(m47, names[47]);
        func(m48, names[48]);
        func(m49, names[49]);
        func(m50, names[50]);
    } else if constexpr (cnt == 52) {
        const auto& [m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13,
                     m14, m15, m16, m17, m18, m19, m20, m21, m22, m23, m24, m25,
                     m26, m27, m28, m29, m30, m31, m32, m33, m34, m35, m36, m37,
                     m38, m39, m40, m41, m42, m43, m44, m45, m46, m47, m48, m49,
                     m50, m51] = object;
        func(m0, names[0]);
        func(m1, names[1]);
        func(m2, names[2]);
        func(m3, names[3]);
        func(m4, names[4]);
        func(m5, names[5]);
        func(m6, names[6]);
        func(m7, names[7]);
        func(m8, names[8]);
        func(m9, names[9]);
        func(m10, names[10]);
        func(m11, names[11]);
        func(m12, names[12]);
        func(m13, names[13]);
        func(m14, names[14]);
        func(m15, names[15]);
        func(m16, names[16]);
        func(m17, names[17]);
        func(m18, names[18]);
        func(m19, names[19]);
        func(m20, names[20]);
        func(m21, names[21]);
        func(m22, names[22]);
        func(m23, names[23]);
        func(m24, names[24]);
        func(m25, names[25]);
        func(m26, names[26]);
        func(m27, names[27]);
        func(m28, names[28]);
        func(m29, names[29]);
        func(m30, names[30]);
        func(m31, names[31]);
        func(m32, names[32]);
        func(m33, names[33]);
        func(m34, names[34]);
        func(m35, names[35]);
        func(m36, names[36]);
        func(m37, names[37]);
        func(m38, names[38]);
        func(m39, names[39]);
        func(m40, names[40]);
        func(m41, names[41]);
        func(m42, names[42]);
        func(m43, names[43]);
        func(m44, names[44]);
        func(m45, names[45]);
        func(m46, names[46]);
        func(m47, names[47]);
        func(m48, names[48]);
        func(m49, names[49]);
        func(m50, names[50]);
        func(m51, names[51]);
    } else if constexpr (cnt == 53) {
        const auto& [m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13,
                     m14, m15, m16, m17, m18, m19, m20, m21, m22, m23, m24, m25,
                     m26, m27, m28, m29, m30, m31, m32, m33, m34, m35, m36, m37,
                     m38, m39, m40, m41, m42, m43, m44, m45, m46, m47, m48, m49,
                     m50, m51, m52] = object;
        func(m0, names[0]);
        func(m1, names[1]);
        func(m2, names[2]);
        func(m3, names[3]);
        func(m4, names[4]);
        func(m5, names[5]);
        func(m6, names[6]);
        func(m7, names[7]);
        func(m8, names[8]);
        func(m9, names[9]);
        func(m10, names[10]);
        func(m11, names[11]);
        func(m12, names[12]);
        func(m13, names[13]);
        func(m14, names[14]);
        func(m15, names[15]);
        func(m16, names[16]);
        func(m17, names[17]);
        func(m18, names[18]);
        func(m19, names[19]);
        func(m20, names[20]);
        func(m21, names[21]);
        func(m22, names[22]);
        func(m23, names[23]);
        func(m24, names[24]);
        func(m25, names[25]);
        func(m26, names[26]);
        func(m27, names[27]);
        func(m28, names[28]);
        func(m29, names[29]);
        func(m30, names[30]);
        func(m31, names[31]);
        func(m32, names[32]);
        func(m33, names[33]);
        func(m34, names[34]);
        func(m35, names[35]);
        func(m36, names[36]);
        func(m37, names[37]);
        func(m38, names[38]);
        func(m39, names[39]);
        func(m40, names[40]);
        func(m41, names[41]);
        func(m42, names[42]);
        func(m43, names[43]);
        func(m44, names[44]);
        func(m45, names[45]);
        func(m46, names[46]);
        func(m47, names[47]);
        func(m48, names[48]);
        func(m49, names[49]);
        func(m50, names[50]);
        func(m51, names[51]);
        func(m52, names[52]);
    } else if constexpr (cnt == 54) {
        const auto& [m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13,
                     m14, m15, m16, m17, m18, m19, m20, m21, m22, m23, m24, m25,
                     m26, m27, m28, m29, m30, m31, m32, m33, m34, m35, m36, m37,
                     m38, m39, m40, m41, m42, m43, m44, m45, m46, m47, m48, m49,
                     m50, m51, m52, m53] = object;
        func(m0, names[0]);
        func(m1, names[1]);
        func(m2, names[2]);
        func(m3, names[3]);
        func(m4, names[4]);
        func(m5, names[5]);
        func(m6, names[6]);
        func(m7, names[7]);
        func(m8, names[8]);
        func(m9, names[9]);
        func(m10, names[10]);
        func(m11, names[11]);
        func(m12, names[12]);
        func(m13, names[13]);
        func(m14, names[14]);
        func(m15, names[15]);
        func(m16, names[16]);
        func(m17, names[17]);
        func(m18, names[18]);
        func(m19, names[19]);
        func(m20, names[20]);
        func(m21, names[21]);
        func(m22, names[22]);
        func(m23, names[23]);
        func(m24, names[24]);
        func(m25, names[25]);
        func(m26, names[26]);
        func(m27, names[27]);
        func(m28, names[28]);
        func(m29, names[29]);
        func(m30, names[30]);
        func(m31, names[31]);
        func(m32, names[32]);
        func(m33, names[33]);
        func(m34, names[34]);
        func(m35, names[35]);
        func(m36, names[36]);
        func(m37, names[37]);
        func(m38, names[38]);
        func(m39, names[39]);
        func(m40, names[40]);
        func(m41, names[41]);
        func(m42, names[42]);
        func(m43, names[43]);
        func(m44, names[44]);
        func(m45, names[45]);
        func(m46, names[46]);
        func(m47, names[47]);
        func(m48, names[48]);
        func(m49, names[49]);
        func(m50, names[50]);
        func(m51, names[51]);
        func(m52, names[52]);
        func(m53, names[53]);
    } else if constexpr (cnt == 55) {
        const auto& [m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13,
                     m14, m15, m16, m17, m18, m19, m20, m21, m22, m23, m24, m25,
                     m26, m27, m28, m29, m30, m31, m32, m33, m34, m35, m36, m37,
                     m38, m39, m40, m41, m42, m43, m44, m45, m46, m47, m48, m49,
                     m50, m51, m52, m53, m54] = object;
        func(m0, names[0]);
        func(m1, names[1]);
        func(m2, names[2]);
        func(m3, names[3]);
        func(m4, names[4]);
        func(m5, names[5]);
        func(m6, names[6]);
        func(m7, names[7]);
        func(m8, names[8]);
        func(m9, names[9]);
        func(m10, names[10]);
        func(m11, names[11]);
        func(m12, names[12]);
        func(m13, names[13]);
        func(m14, names[14]);
        func(m15, names[15]);
        func(m16, names[16]);
        func(m17, names[17]);
        func(m18, names[18]);
        func(m19, names[19]);
        func(m20, names[20]);
        func(m21, names[21]);
        func(m22, names[22]);
        func(m23, names[23]);
        func(m24, names[24]);
        func(m25, names[25]);
        func(m26, names[26]);
        func(m27, names[27]);
        func(m28, names[28]);
        func(m29, names[29]);
        func(m30, names[30]);
        func(m31, names[31]);
        func(m32, names[32]);
        func(m33, names[33]);
        func(m34, names[34]);
        func(m35, names[35]);
        func(m36, names[36]);
        func(m37, names[37]);
        func(m38, names[38]);
        func(m39, names[39]);
        func(m40, names[40]);
        func(m41, names[41]);
        func(m42, names[42]);
        func(m43, names[43]);
        func(m44, names[44]);
        func(m45, names[45]);
        func(m46, names[46]);
        func(m47, names[47]);
        func(m48, names[48]);
        func(m49, names[49]);
        func(m50, names[50]);
        func(m51, names[51]);
        func(m52, names[52]);
        func(m53, names[53]);
        func(m54, names[54]);
    } else if constexpr (cnt == 56) {
        const auto& [m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13,
                     m14, m15, m16, m17, m18, m19, m20, m21, m22, m23, m24, m25,
                     m26, m27, m28, m29, m30, m31, m32, m33, m34, m35, m36, m37,
                     m38, m39, m40, m41, m42, m43, m44, m45, m46, m47, m48, m49,
                     m50, m51, m52, m53, m54, m55] = object;
        func(m0, names[0]);
        func(m1, names[1]);
        func(m2, names[2]);
        func(m3, names[3]);
        func(m4, names[4]);
        func(m5, names[5]);
        func(m6, names[6]);
        func(m7, names[7]);
        func(m8, names[8]);
        func(m9, names[9]);
        func(m10, names[10]);
        func(m11, names[11]);
        func(m12, names[12]);
        func(m13, names[13]);
        func(m14, names[14]);
        func(m15, names[15]);
        func(m16, names[16]);
        func(m17, names[17]);
        func(m18, names[18]);
        func(m19, names[19]);
        func(m20, names[20]);
        func(m21, names[21]);
        func(m22, names[22]);
        func(m23, names[23]);
        func(m24, names[24]);
        func(m25, names[25]);
        func(m26, names[26]);
        func(m27, names[27]);
        func(m28, names[28]);
        func(m29, names[29]);
        func(m30, names[30]);
        func(m31, names[31]);
        func(m32, names[32]);
        func(m33, names[33]);
        func(m34, names[34]);
        func(m35, names[35]);
        func(m36, names[36]);
        func(m37, names[37]);
        func(m38, names[38]);
        func(m39, names[39]);
        func(m40, names[40]);
        func(m41, names[41]);
        func(m42, names[42]);
        func(m43, names[43]);
        func(m44, names[44]);
        func(m45, names[45]);
        func(m46, names[46]);
        func(m47, names[47]);
        func(m48, names[48]);
        func(m49, names[49]);
        func(m50, names[50]);
        func(m51, names[51]);
        func(m52, names[52]);
        func(m53, names[53]);
        func(m54, names[54]);
        func(m55, names[55]);
    } else if constexpr (cnt == 57) {
        const auto& [m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13,
                     m14, m15, m16, m17, m18, m19, m20, m21, m22, m23, m24, m25,
                     m26, m27, m28, m29, m30, m31, m32, m33, m34, m35, m36, m37,
                     m38, m39, m40, m41, m42, m43, m44, m45, m46, m47, m48, m49,
                     m50, m51, m52, m53, m54, m55, m56] = object;
        func(m0, names[0]);
        func(m1, names[1]);
        func(m2, names[2]);
        func(m3, names[3]);
        func(m4, names[4]);
        func(m5, names[5]);
        func(m6, names[6]);
        func(m7, names[7]);
        func(m8, names[8]);
        func(m9, names[9]);
        func(m10, names[10]);
        func(m11, names[11]);
        func(m12, names[12]);
        func(m13, names[13]);
        func(m14, names[14]);
        func(m15, names[15]);
        func(m16, names[16]);
        func(m17, names[17]);
        func(m18, names[18]);
        func(m19, names[19]);
        func(m20, names[20]);
        func(m21, names[21]);
        func(m22, names[22]);
        func(m23, names[23]);
        func(m24, names[24]);
        func(m25, names[25]);
        func(m26, names[26]);
        func(m27, names[27]);
        func(m28, names[28]);
        func(m29, names[29]);
        func(m30, names[30]);
        func(m31, names[31]);
        func(m32, names[32]);
        func(m33, names[33]);
        func(m34, names[34]);
        func(m35, names[35]);
        func(m36, names[36]);
        func(m37, names[37]);
        func(m38, names[38]);
        func(m39, names[39]);
        func(m40, names[40]);
        func(m41, names[41]);
        func(m42, names[42]);
        func(m43, names[43]);
        func(m44, names[44]);
        func(m45, names[45]);
        func(m46, names[46]);
        func(m47, names[47]);
        func(m48, names[48]);
        func(m49, names[49]);
        func(m50, names[50]);
        func(m51, names[51]);
        func(m52, names[52]);
        func(m53, names[53]);
        func(m54, names[54]);
        func(m55, names[55]);
        func(m56, names[56]);
    } else if constexpr (cnt == 58) {
        const auto& [m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13,
                     m14, m15, m16, m17, m18, m19, m20, m21, m22, m23, m24, m25,
                     m26, m27, m28, m29, m30, m31, m32, m33, m34, m35, m36, m37,
                     m38, m39, m40, m41, m42, m43, m44, m45, m46, m47, m48, m49,
                     m50, m51, m52, m53, m54, m55, m56, m57] = object;
        func(m0, names[0]);
        func(m1, names[1]);
        func(m2, names[2]);
        func(m3, names[3]);
        func(m4, names[4]);
        func(m5, names[5]);
        func(m6, names[6]);
        func(m7, names[7]);
        func(m8, names[8]);
        func(m9, names[9]);
        func(m10, names[10]);
        func(m11, names[11]);
        func(m12, names[12]);
        func(m13, names[13]);
        func(m14, names[14]);
        func(m15, names[15]);
        func(m16, names[16]);
        func(m17, names[17]);
        func(m18, names[18]);
        func(m19, names[19]);
        func(m20, names[20]);
        func(m21, names[21]);
        func(m22, names[22]);
        func(m23, names[23]);
        func(m24, names[24]);
        func(m25, names[25]);
        func(m26, names[26]);
        func(m27, names[27]);
        func(m28, names[28]);
        func(m29, names[29]);
        func(m30, names[30]);
        func(m31, names[31]);
        func(m32, names[32]);
        func(m33, names[33]);
        func(m34, names[34]);
        func(m35, names[35]);
        func(m36, names[36]);
        func(m37, names[37]);
        func(m38, names[38]);
        func(m39, names[39]);
        func(m40, names[40]);
        func(m41, names[41]);
        func(m42, names[42]);
        func(m43, names[43]);
        func(m44, names[44]);
        func(m45, names[45]);
        func(m46, names[46]);
        func(m47, names[47]);
        func(m48, names[48]);
        func(m49, names[49]);
        func(m50, names[50]);
        func(m51, names[51]);
        func(m52, names[52]);
        func(m53, names[53]);
        func(m54, names[54]);
        func(m55, names[55]);
        func(m56, names[56]);
        func(m57, names[57]);
    } else if constexpr (cnt == 59) {
        const auto& [m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13,
                     m14, m15, m16, m17, m18, m19, m20, m21, m22, m23, m24, m25,
                     m26, m27, m28, m29, m30, m31, m32, m33, m34, m35, m36, m37,
                     m38, m39, m40, m41, m42, m43, m44, m45, m46, m47, m48, m49,
                     m50, m51, m52, m53, m54, m55, m56, m57, m58] = object;
        func(m0, names[0]);
        func(m1, names[1]);
        func(m2, names[2]);
        func(m3, names[3]);
        func(m4, names[4]);
        func(m5, names[5]);
        func(m6, names[6]);
        func(m7, names[7]);
        func(m8, names[8]);
        func(m9, names[9]);
        func(m10, names[10]);
        func(m11, names[11]);
        func(m12, names[12]);
        func(m13, names[13]);
        func(m14, names[14]);
        func(m15, names[15]);
        func(m16, names[16]);
        func(m17, names[17]);
        func(m18, names[18]);
        func(m19, names[19]);
        func(m20, names[20]);
        func(m21, names[21]);
        func(m22, names[22]);
        func(m23, names[23]);
        func(m24, names[24]);
        func(m25, names[25]);
        func(m26, names[26]);
        func(m27, names[27]);
        func(m28, names[28]);
        func(m29, names[29]);
        func(m30, names[30]);
        func(m31, names[31]);
        func(m32, names[32]);
        func(m33, names[33]);
        func(m34, names[34]);
        func(m35, names[35]);
        func(m36, names[36]);
        func(m37, names[37]);
        func(m38, names[38]);
        func(m39, names[39]);
        func(m40, names[40]);
        func(m41, names[41]);
        func(m42, names[42]);
        func(m43, names[43]);
        func(m44, names[44]);
        func(m45, names[45]);
        func(m46, names[46]);
        func(m47, names[47]);
        func(m48, names[48]);
        func(m49, names[49]);
        func(m50, names[50]);
        func(m51, names[51]);
        func(m52, names[52]);
        func(m53, names[53]);
        func(m54, names[54]);
        func(m55, names[55]);
        func(m56, names[56]);
        func(m57, names[57]);
        func(m58, names[58]);
    } else if constexpr (cnt == 60) {
        const auto& [m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13,
                     m14, m15, m16, m17, m18, m19, m20, m21, m22, m23, m24, m25,
                     m26, m27, m28, m29, m30, m31, m32, m33, m34, m35, m36, m37,
                     m38, m39, m40, m41, m42, m43, m44, m45, m46, m47, m48, m49,
                     m50, m51, m52, m53, m54, m55, m56, m57, m58, m59] = object;
        func(m0, names[0]);
        func(m1, names[1]);
        func(m2, names[2]);
        func(m3, names[3]);
        func(m4, names[4]);
        func(m5, names[5]);
        func(m6, names[6]);
        func(m7, names[7]);
        func(m8, names[8]);
        func(m9, names[9]);
        func(m10, names[10]);
        func(m11, names[11]);
        func(m12, names[12]);
        func(m13, names[13]);
        func(m14, names[14]);
        func(m15, names[15]);
        func(m16, names[16]);
        func(m17, names[17]);
        func(m18, names[18]);
        func(m19, names[19]);
        func(m20, names[20]);
        func(m21, names[21]);
        func(m22, names[22]);
        func(m23, names[23]);
        func(m24, names[24]);
        func(m25, names[25]);
        func(m26, names[26]);
        func(m27, names[27]);
        func(m28, names[28]);
        func(m29, names[29]);
        func(m30, names[30]);
        func(m31, names[31]);
        func(m32, names[32]);
        func(m33, names[33]);
        func(m34, names[34]);
        func(m35, names[35]);
        func(m36, names[36]);
        func(m37, names[37]);
        func(m38, names[38]);
        func(m39, names[39]);
        func(m40, names[40]);
        func(m41, names[41]);
        func(m42, names[42]);
        func(m43, names[43]);
        func(m44, names[44]);
        func(m45, names[45]);
        func(m46, names[46]);
        func(m47, names[47]);
        func(m48, names[48]);
        func(m49, names[49]);
        func(m50, names[50]);
        func(m51, names[51]);
        func(m52, names[52]);
        func(m53, names[53]);
        func(m54, names[54]);
        func(m55, names[55]);
        func(m56, names[56]);
        func(m57, names[57]);
        func(m58, names[58]);
        func(m59, names[59]);
    } else if constexpr (cnt == 61) {
        const auto& [m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13,
                     m14, m15, m16, m17, m18, m19, m20, m21, m22, m23, m24, m25,
                     m26, m27, m28, m29, m30, m31, m32, m33, m34, m35, m36, m37,
                     m38, m39, m40, m41, m42, m43, m44, m45, m46, m47, m48, m49,
                     m50, m51, m52, m53, m54, m55, m56, m57, m58, m59, m60] =
            object;
        func(m0, names[0]);
        func(m1, names[1]);
        func(m2, names[2]);
        func(m3, names[3]);
        func(m4, names[4]);
        func(m5, names[5]);
        func(m6, names[6]);
        func(m7, names[7]);
        func(m8, names[8]);
        func(m9, names[9]);
        func(m10, names[10]);
        func(m11, names[11]);
        func(m12, names[12]);
        func(m13, names[13]);
        func(m14, names[14]);
        func(m15, names[15]);
        func(m16, names[16]);
        func(m17, names[17]);
        func(m18, names[18]);
        func(m19, names[19]);
        func(m20, names[20]);
        func(m21, names[21]);
        func(m22, names[22]);
        func(m23, names[23]);
        func(m24, names[24]);
        func(m25, names[25]);
        func(m26, names[26]);
        func(m27, names[27]);
        func(m28, names[28]);
        func(m29, names[29]);
        func(m30, names[30]);
        func(m31, names[31]);
        func(m32, names[32]);
        func(m33, names[33]);
        func(m34, names[34]);
        func(m35, names[35]);
        func(m36, names[36]);
        func(m37, names[37]);
        func(m38, names[38]);
        func(m39, names[39]);
        func(m40, names[40]);
        func(m41, names[41]);
        func(m42, names[42]);
        func(m43, names[43]);
        func(m44, names[44]);
        func(m45, names[45]);
        func(m46, names[46]);
        func(m47, names[47]);
        func(m48, names[48]);
        func(m49, names[49]);
        func(m50, names[50]);
        func(m51, names[51]);
        func(m52, names[52]);
        func(m53, names[53]);
        func(m54, names[54]);
        func(m55, names[55]);
        func(m56, names[56]);
        func(m57, names[57]);
        func(m58, names[58]);
        func(m59, names[59]);
        func(m60, names[60]);
    } else if constexpr (cnt == 62) {
        const auto& [m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13,
                     m14, m15, m16, m17, m18, m19, m20, m21, m22, m23, m24, m25,
                     m26, m27, m28, m29, m30, m31, m32, m33, m34, m35, m36, m37,
                     m38, m39, m40, m41, m42, m43, m44, m45, m46, m47, m48, m49,
                     m50, m51, m52, m53, m54, m55, m56, m57, m58, m59, m60,
                     m61] = object;
        func(m0, names[0]);
        func(m1, names[1]);
        func(m2, names[2]);
        func(m3, names[3]);
        func(m4, names[4]);
        func(m5, names[5]);
        func(m6, names[6]);
        func(m7, names[7]);
        func(m8, names[8]);
        func(m9, names[9]);
        func(m10, names[10]);
        func(m11, names[11]);
        func(m12, names[12]);
        func(m13, names[13]);
        func(m14, names[14]);
        func(m15, names[15]);
        func(m16, names[16]);
        func(m17, names[17]);
        func(m18, names[18]);
        func(m19, names[19]);
        func(m20, names[20]);
        func(m21, names[21]);
        func(m22, names[22]);
        func(m23, names[23]);
        func(m24, names[24]);
        func(m25, names[25]);
        func(m26, names[26]);
        func(m27, names[27]);
        func(m28, names[28]);
        func(m29, names[29]);
        func(m30, names[30]);
        func(m31, names[31]);
        func(m32, names[32]);
        func(m33, names[33]);
        func(m34, names[34]);
        func(m35, names[35]);
        func(m36, names[36]);
        func(m37, names[37]);
        func(m38, names[38]);
        func(m39, names[39]);
        func(m40, names[40]);
        func(m41, names[41]);
        func(m42, names[42]);
        func(m43, names[43]);
        func(m44, names[44]);
        func(m45, names[45]);
        func(m46, names[46]);
        func(m47, names[47]);
        func(m48, names[48]);
        func(m49, names[49]);
        func(m50, names[50]);
        func(m51, names[51]);
        func(m52, names[52]);
        func(m53, names[53]);
        func(m54, names[54]);
        func(m55, names[55]);
        func(m56, names[56]);
        func(m57, names[57]);
        func(m58, names[58]);
        func(m59, names[59]);
        func(m60, names[60]);
        func(m61, names[61]);
    } else if constexpr (cnt == 63) {
        const auto& [m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13,
                     m14, m15, m16, m17, m18, m19, m20, m21, m22, m23, m24, m25,
                     m26, m27, m28, m29, m30, m31, m32, m33, m34, m35, m36, m37,
                     m38, m39, m40, m41, m42, m43, m44, m45, m46, m47, m48, m49,
                     m50, m51, m52, m53, m54, m55, m56, m57, m58, m59, m60, m61,
                     m62] = object;
        func(m0, names[0]);
        func(m1, names[1]);
        func(m2, names[2]);
        func(m3, names[3]);
        func(m4, names[4]);
        func(m5, names[5]);
        func(m6, names[6]);
        func(m7, names[7]);
        func(m8, names[8]);
        func(m9, names[9]);
        func(m10, names[10]);
        func(m11, names[11]);
        func(m12, names[12]);
        func(m13, names[13]);
        func(m14, names[14]);
        func(m15, names[15]);
        func(m16, names[16]);
        func(m17, names[17]);
        func(m18, names[18]);
        func(m19, names[19]);
        func(m20, names[20]);
        func(m21, names[21]);
        func(m22, names[22]);
        func(m23, names[23]);
        func(m24, names[24]);
        func(m25, names[25]);
        func(m26, names[26]);
        func(m27, names[27]);
        func(m28, names[28]);
        func(m29, names[29]);
        func(m30, names[30]);
        func(m31, names[31]);
        func(m32, names[32]);
        func(m33, names[33]);
        func(m34, names[34]);
        func(m35, names[35]);
        func(m36, names[36]);
        func(m37, names[37]);
        func(m38, names[38]);
        func(m39, names[39]);
        func(m40, names[40]);
        func(m41, names[41]);
        func(m42, names[42]);
        func(m43, names[43]);
        func(m44, names[44]);
        func(m45, names[45]);
        func(m46, names[46]);
        func(m47, names[47]);
        func(m48, names[48]);
        func(m49, names[49]);
        func(m50, names[50]);
        func(m51, names[51]);
        func(m52, names[52]);
        func(m53, names[53]);
        func(m54, names[54]);
        func(m55, names[55]);
        func(m56, names[56]);
        func(m57, names[57]);
        func(m58, names[58]);
        func(m59, names[59]);
        func(m60, names[60]);
        func(m61, names[61]);
        func(m62, names[62]);
    } else if constexpr (cnt == 64) {
        const auto& [m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13,
                     m14, m15, m16, m17, m18, m19, m20, m21, m22, m23, m24, m25,
                     m26, m27, m28, m29, m30, m31, m32, m33, m34, m35, m36, m37,
                     m38, m39, m40, m41, m42, m43, m44, m45, m46, m47, m48, m49,
                     m50, m51, m52, m53, m54, m55, m56, m57, m58, m59, m60, m61,
                     m62, m63] = object;
        func(m0, names[0]);
        func(m1, names[1]);
        func(m2, names[2]);
        func(m3, names[3]);
        func(m4, names[4]);
        func(m5, names[5]);
        func(m6, names[6]);
        func(m7, names[7]);
        func(m8, names[8]);
        func(m9, names[9]);
        func(m10, names[10]);
        func(m11, names[11]);
        func(m12, names[12]);
        func(m13, names[13]);
        func(m14, names[14]);
        func(m15, names[15]);
        func(m16, names[16]);
        func(m17, names[17]);
        func(m18, names[18]);
        func(m19, names[19]);
        func(m20, names[20]);
        func(m21, names[21]);
        func(m22, names[22]);
        func(m23, names[23]);
        func(m24, names[24]);
        func(m25, names[25]);
        func(m26, names[26]);
        func(m27, names[27]);
        func(m28, names[28]);
        func(m29, names[29]);
        func(m30, names[30]);
        func(m31, names[31]);
        func(m32, names[32]);
        func(m33, names[33]);
        func(m34, names[34]);
        func(m35, names[35]);
        func(m36, names[36]);
        func(m37, names[37]);
        func(m38, names[38]);
        func(m39, names[39]);
        func(m40, names[40]);
        func(m41, names[41]);
        func(m42, names[42]);
        func(m43, names[43]);
        func(m44, names[44]);
        func(m45, names[45]);
        func(m46, names[46]);
        func(m47, names[47]);
        func(m48, names[48]);
        func(m49, names[49]);
        func(m50, names[50]);
        func(m51, names[51]);
        func(m52, names[52]);
        func(m53, names[53]);
        func(m54, names[54]);
        func(m55, names[55]);
        func(m56, names[56]);
        func(m57, names[57]);
        func(m58, names[58]);
        func(m59, names[59]);
        func(m60, names[60]);
        func(m61, names[61]);
        func(m62, names[62]);
        func(m63, names[63]);
    } else {
        static_assert("Too many members! Max Support 64 members.")
    }
}
}  // namespace BL
#endif  //! BL_REFLECT_HPP_FILE