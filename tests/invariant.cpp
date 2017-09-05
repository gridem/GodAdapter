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

#include "ut.h"

const char* const c_invariant = "invariant";

const char* const c_ctor = "ctor";
const char* const c_copyCtor = "copy ctor";
const char* const c_mtor = "mtor";
const char* const c_dtor = "dtor";

const char* const c_method = "method";

struct Logic
{
    Logic()
    {
        op(c_ctor);
    }

    Logic(Logic&&)
    {
        op(c_mtor);
    }

    Logic(const Logic&)
    {
        op(c_copyCtor);
    }

    ~Logic()
    {
        op(c_dtor);
    }

public:
    void method()
    {
        op(c_method);
    }

protected:
    void invariant()
    {
        op(c_invariant);
    }
};

DECL_ADAPTER(Logic, method)

BOOST_FIXTURE_TEST_SUITE(invariant, OpsFixture)

BOOST_AUTO_TEST_CASE(Method)
{
    {
        AdaptedChecked<Logic> logic;
        CHECK_OPS(c_ctor, c_invariant);

        logic.method();
        CHECK_OPS(c_ctor, c_invariant, c_invariant, c_method, c_invariant);
    }

    CHECK_OPS(c_ctor, c_invariant, c_invariant, c_method, c_invariant, c_invariant, c_dtor);
}

BOOST_AUTO_TEST_CASE(MoveCtor)
{
    {
        AdaptedChecked<Logic> logic1;
        CHECK_OPS(c_ctor, c_invariant);

        AdaptedChecked<Logic> logic2 = std::move(logic1);
        CHECK_OPS(c_ctor, c_invariant, c_mtor, c_invariant, c_invariant);
    }

    CHECK_OPS(c_ctor, c_invariant, c_mtor, c_invariant, c_invariant, c_invariant, c_dtor, c_invariant, c_dtor);
}

BOOST_AUTO_TEST_CASE(CopyCtor)
{
    {
        AdaptedChecked<Logic> logic1;
        CHECK_OPS(c_ctor, c_invariant);

        AdaptedChecked<Logic> logic2 = logic1;
        CHECK_OPS(c_ctor, c_invariant, c_copyCtor, c_invariant);
    }

    CHECK_OPS(c_ctor, c_invariant, c_copyCtor, c_invariant, c_invariant, c_dtor, c_invariant, c_dtor);
}

BOOST_AUTO_TEST_SUITE_END()
