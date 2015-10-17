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

#include "decl.h"

template<typename T_base>
struct BaseValue : T_base
{
    FWD_CTOR_TBASE(BaseValue)
protected:
    template<typename F, typename... V>
    auto call(F f, V&&... v)
    {
        return f(*this, std::forward<V>(v)...);
    }
};

template<typename T>
struct BaseRef
{
    BaseRef(T& t) : ref_{t} {}
protected:
    template<typename F, typename... V>
    auto call(F f, V&&... v)
    {
        return f(ref_, std::forward<V>(v)...);
    }
private:
    T& ref_;
};
