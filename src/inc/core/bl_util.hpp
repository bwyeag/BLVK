#ifndef _BL_CORE_BL_UTIL_HPP_
#define _BL_CORE_BL_UTIL_HPP_
#include <functional>
#include <list>
namespace BL {
namespace _detail {
template <typename Tag>
static constexpr bool has_callback_set = false;
template <typename Tag, size_t Series>
void callback_set() {}
}  // namespace _detail

template <typename Tag, typename... Args>
class Callback {
   public:
    using Func = std::function<void(Args...)>;
    using List = typename std::list<Func>;
    using Iterator = typename List::iterator;

   private:
    List items;

   public:
    struct Handle {
        Iterator it;
    };
    size_t size() const { return items.size(); }
    Handle insert(Func&& fn) {
        items.push_back(std::forward(fn));
        return items.end();
    }
    void iterate(Args&&... call) {
        for (Func& fn : items) {
            fn(call...);
        }
    }
    void erase(Handle& handle) { items.erase(handle.it); }
    void clear() { items.clear(); }
};

template <typename Tag, size_t Series, typename... Args>
class Callback2 {
   public:
    using Func = std::function<void(Args...)>;
    using List = typename std::list<Func>;
    using Iterator = typename List::iterator;

   private:
    List items;

   public:
    struct Handle {
        Iterator it;
    };
    size_t size() const { return items.size(); }
    Handle insert(Func&& fn) {
        if constexpr (_detail::has_callback_set<Tag>)
            _detail::callback_set<Tag, Series>();
        items.push_back(std::forward(fn));
        return items.end();
    }
    void iterate(Args&&... call) {
        for (Func& fn : items) {
            fn(std::forward(call)...);
        }
    }
    void erase(Handle& handle) { items.erase(handle.it); }
    void clear() { items.clear(); }
};
}  // namespace BL
#endif  //!_BL_CORE_BL_UTIL_HPP_