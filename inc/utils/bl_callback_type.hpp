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
#ifndef BL_UTIL_RESULT_TYPE_FILE
#include <functional>
#include <list>
namespace blt {
//*****************************************************************************
// 回调函数类
//*****************************************************************************
namespace _detail {
template <typename Tag> static constexpr bool has_callback_set = false;
template <typename Tag, size_t Series> void callback_set() {}
} // namespace _detail
template <typename Tag, size_t Series, typename... Args> class Callback2 {
public:
  using Func = std::function<void(Args...)>;
  using List = typename std::list<Func>;
  using Iterator = typename List::iterator;

private:
  List m_Items;

public:
  struct Handle {
    Iterator m_it;
  };
  size_t size() const { return m_Items.size(); }
  Handle insert(Func &&fn) {
    if constexpr (_detail::has_callback_set<Tag>)
      _detail::callback_set<Tag, Series>();
    m_Items.push_back(std::forward<Func>(fn));
    return {m_Items.end()};
  }
  void iterate(Args &&...call) {
    for (Func &fn : m_Items) {
      fn(std::forward<Args>(call)...);
    }
  }
  void erase(Handle &handle) { m_Items.erase(handle.m_it); }
  void clear() { m_Items.clear(); }
};
template <typename Tag, typename... Args>
using Callback = Callback2<Tag, 0u, Args...>;
} // namespace BLT
#endif // !BL_UTIL_RESULT_TYPE_FILE
