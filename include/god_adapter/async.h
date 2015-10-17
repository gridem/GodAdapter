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
#include "detail/rotate.h"

template<typename T_base, typename T_future>
struct BaseCallback2Future : T_base
{
    FWD_CTOR_TBASE(BaseCallback2Future)

protected:
    template<typename F, typename... V>
    auto call(F f, V&&... v)
    {
        typename T_future::Promise promise;
        f(static_cast<T_base&>(*this), std::forward<V>(v)..., [promise](auto&& val) mutable {
            promise.put(std::move(val));
        });
        return promise.future();
    }
};

template<typename T_base>
struct BaseFuture2Callback : T_base
{
    FWD_CTOR_TBASE(BaseFuture2Callback)

protected:
    struct Action
    {
        template<typename T_callback, typename F, typename T, typename... V>
        static void apply(T_callback&& cb, F&& f, T&& t, V&&... v)
        {
            f(std::forward<T>(t), std::forward<V>(v)...).then(std::move(cb));
        }
    };

    template<typename F, typename... V>
    auto call(F&& f, V&&... v)
    {
        detail::Rotate<sizeof...(V) - 1, Action, V..., F, T_base&>::apply(
                    std::forward<V>(v)...,
                    std::forward<F>(f),
                    *this);
    }
};

template<typename T, typename T_future, typename T_base = T>
using AdaptedCallback = Adapter<T, BaseCallback2Future<T_base, T_future>>;

template<typename T, typename T_base = T>
using AdaptedFuture = Adapter<T, BaseFuture2Callback<T_base>>;

