/*
 * Copyright 2015 Grigory Demchenko (aka gridem)
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

#include <boost/variant.hpp>

#include "decl.h"

template<typename T>
struct BaseLazy
{
    template<typename... V>
    BaseLazy(V&&... v)
        // doesn't compile under MSVC 2015
        //: state_{[v...] {return T{std::move(v)...}; }}
    {
        state_ = [v...] {
            return T{std::move(v)...};
        };
    }

protected:
    using Creator = std::function<T()>;

    template<typename F, typename... V>
    auto call(F f, V&&... v)
    {
        auto* t = boost::get<T>(&state_);
        if (t == nullptr)
        {
            state_ = boost::get<Creator>(state_)();
            t = boost::get<T>(&state_);
        }
        return f(*t, std::forward<V>(v)...);
    }

private:
    // creator must be first because T may not contain default ctor
    boost::variant<Creator, T> state_;
};

template<typename T, typename T_base = T>
using AdaptedLazy = Adapter<T, BaseLazy<T_base>>;

