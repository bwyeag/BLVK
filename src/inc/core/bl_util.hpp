#ifndef _BL_CORE_BL_UTIL_HPP_
#define _BL_CORE_BL_UTIL_HPP_
#include <functional>
#include <list>
namespace BL {
template <uint32_t Tag, typename... Args>
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
            fn(std::forward(call)...);
        }
    }
    void erase(Handle& handle) { items.erase(handle.it); }
};
}  // namespace BL
#endif  //!_BL_CORE_BL_UTIL_HPP_