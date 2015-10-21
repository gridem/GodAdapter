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

const char* const c_lock = "lock";
const char* const c_unlock = "unlock";
const char* const c_action = "action";
const char* const c_copyCtor = "copy ctor";
const char* const c_dtorDerived = "dtor derived";

struct TestMutex
{
    void lock()
    {
        op(c_lock);
    }

    void unlock()
    {
        op(c_unlock);
    }
};

struct Counter
{
    Counter() = default;

    Counter(const Counter&)
    {
        op(c_copyCtor);
    }

    void inc()
    {
        op(c_action);
        ++ counter;
    }

    int counter = 0;
};

struct CounterDerived : Counter
{
    ~CounterDerived()
    {
        op(c_dtorDerived);
    }
};

DECL_ADAPTER(Counter, inc)

BOOST_FIXTURE_TEST_SUITE(shared, OpsFixture)

BOOST_AUTO_TEST_CASE(AdapterLock)
{
    using MutexCounter = AdaptedLocked<Counter, TestMutex>;

    MutexCounter counter;
    BOOST_CHECK_EQUAL(counter.counter, 0);
    counter.inc();
    CHECK_OPS(c_lock, c_action, c_unlock);
    BOOST_CHECK_EQUAL(counter.counter, 1);
    auto counter2 = counter;
    CHECK_OPS(c_lock, c_action, c_unlock, c_copyCtor);
}

BOOST_AUTO_TEST_CASE(AdapterShared)
{
    using SharedCounter = AdaptedShared<Counter>;
    SharedCounter counter;
    counter.inc();
    CHECK_OPS(c_action);
    auto counter2 = counter;
    CHECK_OPS(c_action);
}

BOOST_AUTO_TEST_CASE(AdapterSharedMutex)
{
    using MutexSharedCounter = AdaptedSharedLocked<Counter, TestMutex>;

    MutexSharedCounter counter;
    CHECK_OPS();
    counter.inc();
    CHECK_OPS(c_lock, c_action, c_unlock);
}

BOOST_AUTO_TEST_CASE(AdapterSharedDerived)
{
    {
        AdaptedShared<Counter> c;
        AdaptedShared<Counter> c2 = c;
        c2 = c;
    }
    CHECK_OPS();
    {
        AdaptedShared<Counter> c = AdaptedShared<Counter>{};
    }
    CHECK_OPS();
    {
        AdaptedShared<Counter> c = AdaptedShared<CounterDerived>{};
        CHECK_OPS();
    }
    CHECK_OPS(c_dtorDerived);
}

BOOST_AUTO_TEST_SUITE_END()
