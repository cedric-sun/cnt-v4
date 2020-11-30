// cesun, 11/25/20 4:44 PM.

#ifndef CNT5106_V4_UPTR_CAST_HPP
#define CNT5106_V4_UPTR_CAST_HPP

#include <memory>

template<typename Derived, typename DerivedDel = std::default_delete<Derived>, typename Base, typename BaseDel>
std::unique_ptr<Derived, DerivedDel>
static_unique_ptr_cast(std::unique_ptr<Base, BaseDel> &&p) {
    auto d = static_cast<Derived *>(p.release());
    // TODO: we are casting BaseDel to DerivedDel here... does that work?
    return std::unique_ptr<Derived, DerivedDel>{d, std::move(p.get_deleter())};
}

#endif //CNT5106_V4_UPTR_CAST_HPP
