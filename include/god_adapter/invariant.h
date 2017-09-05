#pragma once

#include "decl.h"

// Dlang-like class invariant checks (https://dlang.org/spec/contracts.html#Invariants)

template<typename F>
struct Finalizer
{
    template<typename U>
    explicit Finalizer(U&& u)
        : finally_(std::forward<U>(u))
    {
    }

    ~Finalizer()
    {
        finally_();
    }

private:
    F finally_;
};

template<typename F>
auto finally(F&& f)
{
    return Finalizer<std::decay_t<F>>{std::forward<F>(f)};
}

template<typename T_base>
struct BaseInvariantChecker : T_base
{
    BaseInvariantChecker(BaseInvariantChecker&& other)
        : T_base(std::move(other))
    {
        this->invariant();
        other.invariant();
    }

    template<typename... V>
    BaseInvariantChecker(V&&... v)
        : T_base(std::forward<V>(v)...)
    {
        this->invariant();
    }

    ~BaseInvariantChecker()
    {
        this->invariant();
    }

protected:
    template<typename F, typename... V>
    auto call(F f, V&&... v)
    {
        this->invariant();
        auto _ = finally([this] {
            this->invariant();
        });
        return f(*this, std::forward<V>(v)...);
    }
};

template<typename T, typename T_base = T>
using AdaptedChecked = Adapter<T, BaseInvariantChecker<T_base>>;
