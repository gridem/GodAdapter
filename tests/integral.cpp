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

const char* const c_ctor = "ctor";
const char* const c_1 = "1";
const char* const c_2 = "2";
const char* const c_lock = "lock";
const char* const c_unlock = "unlock";

using Buffer = std::string;

struct Disk
{
    Disk()
    {
        op(c_ctor);
    }

    void write(const Buffer& buffer)
    {
        op(buffer.c_str());
    }
};

DECL_ADAPTER(Disk, write)

struct DiskExecute;
DECL_ADAPTER(DiskExecute, write, execute)

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

BOOST_FIXTURE_TEST_SUITE(integral, OpsFixture)

BOOST_AUTO_TEST_CASE(Batch)
{
    AdaptedBatch<Disk> disk;
    CHECK_OPS(c_ctor);
    disk.write(c_1);
    disk.write(c_2);
    CHECK_OPS(c_ctor);
    disk.execute();
    CHECK_OPS(c_ctor, c_1, c_2);
}

BOOST_AUTO_TEST_CASE(Integral)
{
    AdaptedLazy<DiskExecute, AdaptedShared<DiskExecute, AdaptedLocked<DiskExecute, AdaptedBatch<Disk>, TestMutex>>> disk;
    CHECK_OPS();
    disk.write(c_1);
    CHECK_OPS(c_ctor, c_lock, c_unlock);
    disk.write(c_2);
    CHECK_OPS(c_ctor, c_lock, c_unlock, c_lock, c_unlock);
    disk.execute();
    CHECK_OPS(c_ctor, c_lock, c_unlock, c_lock, c_unlock, c_lock, c_1, c_2, c_unlock);
}

BOOST_AUTO_TEST_SUITE_END()
