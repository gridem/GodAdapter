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

#include <boost/test/unit_test.hpp>
#include <god_adapter/god_adapter.h>

#define CHECK_OPS(...) \
    BOOST_CHECK_EQUAL(ops(), (Operations{__VA_ARGS__}));

template<typename T>
T& single()
{
    static T t;
    return t;
}

using Operations = std::vector<const char*>;

inline Operations& ops()
{
    return single<Operations>();
}

inline void op(const char* o)
{
    ops().push_back(o);
}

struct OpsFixture
{
    ~OpsFixture()
    {
        ops().clear();
    }
};

namespace std {

template<typename T>
ostream& operator<<(ostream& o, const vector<T>& t)
{
    bool first = true;
    for (auto&& v: t)
    {
        if (first)
            first = false;
        else
            o << ", ";
        o << v;
    }
    return o;
}

}
