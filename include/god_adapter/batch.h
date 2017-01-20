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

#include <boost/any.hpp>
#include <vector>
#include <functional>
#include <type_traits>

#include "decl.h"

template<typename T_base>
struct BaseBatch : T_base
{
    FWD_CTOR_TBASE(BaseBatch)

    void execute()
    {
        for (auto&& action: actions_)
        {
            action();
        }
        actions_.clear();
    }

protected:
    using Action = std::function<void()>;

    template<typename F, typename... V>
    auto call(F f, V&&... v)
    {
        actions_.push_back([this, f, v...] {
            f(*this, std::move(v)...);
        });
    }

private:
    std::vector<Action> actions_;
};

template<typename T, typename T_base = T>
using AdaptedBatch = Adapter<T, BaseBatch<T_base>>;

template<typename F>
auto invokeWrappedAny(F&& f) -> typename std::enable_if<
    std::is_same<void, decltype(f())>::value, boost::any>::type
{
    f();
    return {};
}

template<typename F>
auto invokeWrappedAny(F&& f) -> typename std::enable_if<
    !std::is_same<void, decltype(f())>::value, boost::any>::type
{
    return f();
}

template<typename T_base>
struct BaseBatchWithResult : T_base
{
    FWD_CTOR_TBASE(BaseBatchWithResult)

    std::vector<boost::any> execute()
    {
        std::vector<boost::any> results;
        for (auto&& action: actions_)
        {
            results.push_back(action());
        }
        actions_.clear();
        return results;
    }

protected:
    using Action = std::function<boost::any()>;

    template<typename F, typename... V>
    auto call(F f, V&&... v)
    {
        actions_.push_back([this, f, v...]() -> boost::any {
            try
            {
                return invokeWrappedAny([this, f, v...] {
                    return f(*this, std::move(v)...);
                });
            }
            catch (...)
            {
                return std::current_exception();
            }
        });
    }

private:
    std::vector<Action> actions_;
};

template<typename T, typename T_base = T>
using AdaptedBatchWithResult = Adapter<T, BaseBatchWithResult<T_base>>;

