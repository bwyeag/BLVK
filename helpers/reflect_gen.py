# 此程序用于生成 reflect 的 visit_members 函数
count = int(input())
outstr = R"""template<typename Visitor>
constexpr void visit_members(const auto&& object, Visitor&& func) {
    using T = std::decay_t<decltype(object)>;
    constexpr size_t cnt = member_count<T>;
    reflect_names_t<cnt>& names = reflect_names<T>;
    if constexpr (cnt==0) {return;
"""
fmtstr = R"""    }} else if constexpr (cnt == {2}) {{
        const auto& [{0}] = object;
{1}"""
mstr = ""
fnstr = ""
fmtfnstr = R"""        func(m{0},names[{0}]);
"""
for i in range(1,count+1):
    mstr += ", m" + str(i-1)
    fnstr += fmtfnstr.format(str(i-1))
    outstr += fmtstr.format(mstr[2::],fnstr,str(i))

outstr += R"""    } else {
        static_assert("Too many members! Max Support {0} members.")
    }
}""".format(str(count))
print(outstr)