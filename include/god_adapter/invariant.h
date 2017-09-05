#pragma once

#include <boost/scope_exit.hpp>

#include "decl.h"

// Dlang-like class invariant checks (https://dlang.org/spec/contracts.html#Invariants)

template<typename T>
struct BaseInvariantChecker : T
{
    BaseInvariantChecker (BaseInvariantChecker&& other):T (std::move (other))
    {
        this->invariant();
        other.invariant();
    }

    template<typename... V>
    BaseInvariantChecker (V&&... v):T(std::forward<V>(v)...)
    {
        this->invariant();
    }

    ~BaseInvariantChecker()
    {
        this->invariant();
    }

protected:
    template<typename F, typename... V>
    auto call (F f, V&&... v)
    {
        this->invariant();
        BOOST_SCOPE_EXIT_TPL (this_)
        {
            this_->invariant();
        } BOOST_SCOPE_EXIT_END

        return f(*this, std::forward<V>(v)...);
    }
};

template<typename T, typename T_base = T>
using AdaptedChecked = Adapter<T, BaseInvariantChecker<T_base>>;

