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

namespace detail {

template<int N, typename... V>
struct Rotate;

template<int N, typename T_action, typename T, typename... V>
struct Rotate<N, T_action, T, V...>
{
    static void apply(T&& t, V&&... v)
    {
        Rotate<N-1, T_action, V..., T>::apply(
                    std::forward<V>(v)...,
                    std::forward<T>(t));
    }
};

template<typename T_action, typename T, typename... V>
struct Rotate<0, T_action, T, V...>
{
    static void apply(T&& t, V&&... v)
    {
        T_action::apply(
                    std::forward<T>(t),
                    std::forward<V>(v)...);
    }
};

}
