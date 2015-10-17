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

#include "ut.h"

#include <boost/variant.hpp>
#include <functional>

const char* const c_action = "action";
const char* const c_ctor = "ctor";
const char* const c_ctorInt = "ctor int";
const char* const c_ctorString = "ctor string";

struct Obj
{
    Obj()
    {
        op(c_ctor);
    }

    Obj(int v)
    {
        BOOST_CHECK_EQUAL(v, 2);
        op(c_ctorInt);
    }

    Obj(const std::string& s)
    {
        BOOST_CHECK_EQUAL(s, "hello");
        op(c_ctorString);
    }

    void action()
    {
        op(c_action);
    }
};

DECL_ADAPTER(Obj, action)

BOOST_FIXTURE_TEST_SUITE(lazy, OpsFixture)

BOOST_AUTO_TEST_CASE(Normal)
{
    Obj o;
    CHECK_OPS(c_ctor);
    o.action();
    CHECK_OPS(c_ctor, c_action);
    o.action();
    CHECK_OPS(c_ctor, c_action, c_action);
}

BOOST_AUTO_TEST_CASE(Lazy)
{
    AdaptedLazy<Obj> o;
    CHECK_OPS();
    o.action();
    CHECK_OPS(c_ctor, c_action);
    o.action();
    CHECK_OPS(c_ctor, c_action, c_action);
}

BOOST_AUTO_TEST_CASE(LazyWithInt)
{
    AdaptedLazy<Obj> o{2};
    CHECK_OPS();
    o.action();
    CHECK_OPS(c_ctorInt, c_action);
    o.action();
    CHECK_OPS(c_ctorInt, c_action, c_action);
}

BOOST_AUTO_TEST_CASE(LazyWithString)
{
    const char* const hello = "hello";
    AdaptedLazy<Obj> o{hello};
    CHECK_OPS();
    o.action();
    CHECK_OPS(c_ctorString, c_action);
    o.action();
    CHECK_OPS(c_ctorString, c_action, c_action);
}

BOOST_AUTO_TEST_SUITE_END()

