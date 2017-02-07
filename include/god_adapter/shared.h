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

#include <utility>
#include <mutex>

#include "decl.h"

template<typename T_base, typename T_locker>
struct BaseLocker : T_base
{
    FWD_CTOR_TBASE(BaseLocker)
protected:
    template<typename F, typename... V>
    auto call(F f, V&&... v)
    {
        using Lock = std::lock_guard<T_locker>;
        Lock _{lock_};
        return f(static_cast<T_base&>(*this), std::forward<V>(v)...);
    }

private:
    T_locker lock_;
};

template<typename T>
struct BaseShared
{
    template<typename U>
    friend struct BaseShared;

    template<typename U, typename T_base>
    BaseShared(Adapter<U, T_base>&& a) : shared_{std::move(a.shared_)} {}

    template<typename U, typename T_base>
    BaseShared(const Adapter<U, T_base>& a) : shared_{a.shared_} {}

    template<typename U, typename T_base>
    BaseShared(Adapter<U, T_base>& a) : shared_{a.shared_} {}

    template<typename... V>
    BaseShared(V&&... v) : shared_{std::make_shared<T>(std::forward<V>(v)...)} {}

protected:
    template<typename F, typename... V>
    auto call(F f, V&&... v)
    {
        return f(*shared_, std::forward<V>(v)...);
    }
private:
    std::shared_ptr<T> shared_;
};

template<typename T, typename T_base = T, typename T_locker = std::mutex>
using AdaptedLocked = Adapter<T, BaseLocker<T_base, T_locker>>;

template<typename T, typename T_base = T>
using AdaptedShared = Adapter<T, BaseShared<T_base>>;

template<typename T, typename T_base = T, typename T_locker = std::mutex>
using AdaptedSharedLocked = AdaptedShared<T, AdaptedLocked<T, T_base, T_locker>>;
