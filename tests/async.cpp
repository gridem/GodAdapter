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

const char* const c_async = "async";
const char* const c_complete = "complete";
const char* const c_callback = "callback";
const char* const c_then = "then";
const char* const c_put = "put";
const char* const c_async1 = "async 1";
const char* const c_async2 = "async 2";

using Handler = std::function<void()>;

template<typename T>
struct FutureTest
{
    using F = std::function<void(const T&)>;

    FutureTest(const std::shared_ptr<F>& f) : f_{f} {}

    void then(F f)
    {
        op(c_then);
        *f_ = f;
    }

    struct Promise
    {
        Promise() : f_{std::make_shared<F>()} {}

        FutureTest future()
        {
            return f_;
        }

        void put(const T& v)
        {
            op(c_put);
            (*f_)(v);
        }

    private:
        std::shared_ptr<F> f_;
    };

private:
    std::shared_ptr<F> f_;
};

struct ImplCallback
{
    using F = std::function<void(int)>;

    void async(F cb)
    {
        op(c_async);
        cb_ = cb;
    }

    void complete(int v)
    {
        op(c_complete);
        cb_(v);
    }

private:
    F cb_;
};

struct Impl;
DECL_ADAPTER(Impl, async)

struct ImplFuture
{
    FutureTest<int> async(int v)
    {
        op(c_async);
        v_ = v;
        return p.future();
    }

    FutureTest<int> async(int v1, int v2)
    {
        op(c_async);
        v_ = v1 + v2;
        return p.future();
    }

    void complete()
    {
        op(c_complete);
        p.put(v_);
    }

private:
    int v_ = 0;
    FutureTest<int>::Promise p;
};

struct ManualFutureAdapter
{
    ImplCallback impl_;

    FutureTest<int> async()
    {
        FutureTest<int>::Promise p;
        impl_.async([p](int v) mutable {
            p.put(v);
        });
        return p.future();
    }
};

struct ManualCallbackAdapter
{
    ImplFuture impl_;

    void async(int v, std::function<void(int)> cb)
    {
        impl_.async(v).then(cb);
    }
};

struct ComplexCallbacks
{
    using F1 = std::function<void(int)>;
    using F2 = std::function<void(const std::string&)>;

    void async1(int v, F1 cb)
    {
        op(c_async1);
        f1_ = [v, cb] { cb(v); };
    }

    void async2(const std::string& s, F2 cb)
    {
        op(c_async2);
        f2_ = [s, cb] { cb(s); };
    }

    void complete1()
    {
        op(c_complete);
        f1_();
    }

    void complete2()
    {
        op(c_complete);
        f2_();
    }

private:
    Handler f1_;
    Handler f2_;
};

struct Complex1 {};
DECL_ADAPTER(Complex1, async1)
struct Complex2 {};
DECL_ADAPTER(Complex2, async2)

BOOST_FIXTURE_TEST_SUITE(async, OpsFixture)

BOOST_AUTO_TEST_CASE(Async)
{
    ImplCallback i;
    i.async([](int v) {
        BOOST_CHECK_EQUAL(v, 3);
        op(c_callback);
    });
    i.complete(3);
    CHECK_OPS(c_async, c_complete, c_callback);
}

BOOST_AUTO_TEST_CASE(ManualFuture)
{
    ManualFutureAdapter f;
    f.async().then([](int v) {
        BOOST_CHECK_EQUAL(v, 2);
        op(c_callback);
    });
    CHECK_OPS(c_async, c_then);
    f.impl_.complete(2);
    CHECK_OPS(c_async, c_then, c_complete, c_put, c_callback);
}

BOOST_AUTO_TEST_CASE(ManualCallback)
{
    ManualCallbackAdapter f;
    f.async(5, [](int v) {
        BOOST_CHECK_EQUAL(v, 5);
        op(c_callback);
    });
    CHECK_OPS(c_async, c_then);
    f.impl_.complete();
    CHECK_OPS(c_async, c_then, c_complete, c_put, c_callback);
}

BOOST_AUTO_TEST_CASE(AdapterFuture)
{
    AdaptedCallback<Impl, FutureTest<int>, ImplCallback> f;
    f.async().then([](int v) {
        BOOST_CHECK_EQUAL(v, 2);
        op(c_callback);
    });
    CHECK_OPS(c_async, c_then);
    f.complete(2);
    CHECK_OPS(c_async, c_then, c_complete, c_put, c_callback);
}

BOOST_AUTO_TEST_CASE(AdapterCallback)
{
    AdaptedFuture<Impl, ImplFuture> f;
    f.async(3, [](int v) {
        BOOST_CHECK_EQUAL(v, 3);
        op(c_callback);
    });
    CHECK_OPS(c_async, c_then);
    f.complete();
    CHECK_OPS(c_async, c_then, c_complete, c_put, c_callback);
}

BOOST_AUTO_TEST_CASE(AdapterCallback2)
{
    AdaptedFuture<Impl, ImplFuture> f;
    f.async(4, 5, [](int v) {
        BOOST_CHECK_EQUAL(v, 9);
        op(c_callback);
    });
    CHECK_OPS(c_async, c_then);
    f.complete();
    CHECK_OPS(c_async, c_then, c_complete, c_put, c_callback);
}

BOOST_AUTO_TEST_CASE(Rotate)
{
    struct A
    {
        static void apply(int t1, int t2, int t3, int t4, int t5, int t6)
        {
            BOOST_CHECK_EQUAL(t1, 5);
            BOOST_CHECK_EQUAL(t2, 6);
            BOOST_CHECK_EQUAL(t3, 1);
            BOOST_CHECK_EQUAL(t4, 2);
            BOOST_CHECK_EQUAL(t5, 3);
            BOOST_CHECK_EQUAL(t6, 4);
        }
    };

    detail::Rotate<4, A, int, int, int, int, int, int>::apply(1, 2, 3, 4, 5, 6);
}

BOOST_AUTO_TEST_CASE(ComplexAdapter)
{
    static const char* const hello = "hello";

    AdaptedCallback<Complex1, FutureTest<int>, AdaptedCallback<Complex2, FutureTest<std::string>, ComplexCallbacks>> c;
    auto fut1 = c.async1(42);
    CHECK_OPS(c_async1);
    fut1.then([](int v) {
        BOOST_CHECK_EQUAL(v, 42);
        op(c_callback);
    });
    CHECK_OPS(c_async1, c_then);
    auto fut2 = c.async2(hello);
    CHECK_OPS(c_async1, c_then, c_async2);
    fut2.then([](const std::string& s) {
        BOOST_CHECK_EQUAL(s, hello);
        op(c_callback);
    });
    CHECK_OPS(c_async1, c_then, c_async2, c_then);
    c.complete1();
    CHECK_OPS(c_async1, c_then, c_async2, c_then, c_complete, c_put, c_callback);
    c.complete2();
    CHECK_OPS(c_async1, c_then, c_async2, c_then, c_complete, c_put, c_callback, c_complete, c_put, c_callback);
}

BOOST_AUTO_TEST_SUITE_END()
