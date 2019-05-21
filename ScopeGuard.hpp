#ifndef SCOPEGUARD_HEADER_INCLUDED_A6546W4A6F46SA46G4ASF
#define SCOPEGUARD_HEADER_INCLUDED_A6546W4A6F46SA46G4ASF

#include <utility> // move

template<class Fun>
class ScopeGuard {
    Fun f_;
    bool active_;
public:
    ScopeGuard(Fun f) : f_(std::move(f)), active_(true) {}
    ScopeGuard(ScopeGuard&& rhs)
      : f_(std::move(rhs.f_)),
        active_(rhs.active_)
    {
        rhs.dismiss();
    }
    ScopeGuard() = delete;
    ScopeGuard(ScopeGuard const&) = delete;
    ~ScopeGuard() { if (active_) f_(); }
    void dismiss() { active_ = false; }
};

template<class Fun>
ScopeGuard<Fun> scopeGuard(Fun f) {
    return ScopeGuard<Fun>(std::move(f));
}

namespace detail {
    enum class ScopeGuardOnExit {};
    template<typename Fun>
    ScopeGuard<Fun> operator+(ScopeGuardOnExit, Fun&& fn) {
        return ScopeGuard<Fun>(std::forward<Fun>(fn));
    }
} // end namespace detail

#define CONCATENATE_IMPL(s1, s2) s1##s2
#define CONCATENATE(s1, s2) CONCATENATE_IMPL(s1, s2)

#ifdef __COUNTER__
#define ANONYMOUS_VARIABLE(str) \
    CONCATENATE(str, __COUNTER__)
#else
#define ANONYMOUS_VARIABLE(str) \
    CONCATENATE(str, __LINE__)
#endif

#define SCOPE_EXIT \
    auto ANONYMOUS_VARIABLE(SCOPE_EXIT_STATE) \
    = ::detail::ScopeGuardOnExit() + [&]()

#endif // SCOPEGUARD_HEADER_INCLUDED_A6546W4A6F46SA46G4ASF
