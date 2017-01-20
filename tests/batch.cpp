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

#include <boost/variant.hpp>
#include <functional>

#include <iostream>

const char* const c_get = "get";
const char* const c_set = "set";

struct Data
{
    int get() const
    {
        op(c_get);
        return value_;
    }

    void set(int value)
    {
        op(c_set);
        value_ = value;
    }

private:
    int value_ = 0;
};

DECL_ADAPTER(Data, get, set)

struct DataExc
{
    int get() const
    {
        op(c_get);
        throw 1;
    }

    void set(int value)
    {
        op(c_set);
        throw 2;
    }

private:
    int value_ = 0;
};

DECL_ADAPTER(DataExc, get, set)

BOOST_FIXTURE_TEST_SUITE(batch, OpsFixture)

BOOST_AUTO_TEST_CASE(Normal)
{
    Data d;
    CHECK_OPS();
    d.get();
    CHECK_OPS(c_get);
    d.set(1);
    d.set(2);
    CHECK_OPS(c_get, c_set, c_set);
}

BOOST_AUTO_TEST_CASE(Batch)
{
    AdaptedBatch<Data> d;
    CHECK_OPS();
    d.get();
    d.set(1);
    d.set(2);
    CHECK_OPS();
    d.execute();
    CHECK_OPS(c_get, c_set, c_set);
}

BOOST_AUTO_TEST_CASE(BatchWithResult)
{
    AdaptedBatchWithResult<Data> d;
    CHECK_OPS();
    d.set(1);
    d.get();
    d.set(2);
    d.get();
    CHECK_OPS();
    auto results = d.execute();
    CHECK_OPS(c_set, c_get, c_set, c_get);
    BOOST_CHECK_EQUAL(results.size(), 4);
    BOOST_CHECK(results[0].empty());
    BOOST_CHECK_EQUAL(boost::any_cast<int>(results[1]), 1);
    BOOST_CHECK(results[2].empty());
    BOOST_CHECK_EQUAL(boost::any_cast<int>(results[3]), 2);
}

BOOST_AUTO_TEST_CASE(BatchWithExceptions)
{
    AdaptedBatchWithResult<DataExc> d;
    CHECK_OPS();
    d.set(1);
    d.get();
    CHECK_OPS();
    auto results = d.execute();
    CHECK_OPS(c_set, c_get);
    BOOST_CHECK_EQUAL(results.size(), 2);
    auto checkException = [](auto&& val) {
        return [val = std::move(val)](auto&& exVal) {
            return val == exVal;
        };
    };

    BOOST_CHECK_EXCEPTION(std::rethrow_exception(boost::any_cast<std::exception_ptr>(
        results[0])), int, checkException(2));
    BOOST_CHECK_EXCEPTION(std::rethrow_exception(boost::any_cast<std::exception_ptr>(
        results[1])), int, checkException(1));
}

BOOST_AUTO_TEST_SUITE_END()

