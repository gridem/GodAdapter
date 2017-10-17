/*
 * Copyright 2017 Grigory Demchenko (aka gridem)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

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
        T_base::invariant();
        other.invariant();
    }

    template<typename... V>
    BaseInvariantChecker(V&&... v)
        : T_base(std::forward<V>(v)...)
    {
        T_base::invariant();
    }

    ~BaseInvariantChecker()
    {
        T_base::invariant();
    }

protected:
    template<typename F, typename... V>
    auto call(F f, V&&... v)
    {
        T_base::invariant();
        auto _ = finally([this] {
            T_base::invariant();
        });
        return f(*this, std::forward<V>(v)...);
    }
};

template<typename T, typename T_base = T>
using AdaptedChecked = Adapter<T, BaseInvariantChecker<T_base>>;
