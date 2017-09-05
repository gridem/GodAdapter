#include "ut.h"

#include <god_adapter/invariant.h>

const char* const c_invariant = "invariant";

const char* const c_ctor = "ctor";
const char* const c_mtor = "mtor";
const char* const c_dtor = "dtor";

const char* const c_method = "meth";

struct Logic
{
    Logic()
    {
        op (c_ctor);
    }

    Logic(Logic&&)
    {
        op (c_mtor);
    }

    ~Logic()
    {
        op (c_dtor);
    }

public:
    void method()
    {
        op (c_method);
    }

protected:
    void invariant()
    {
        op (c_invariant);
    }
};

DECL_ADAPTER (Logic, method)

BOOST_FIXTURE_TEST_SUITE(invariant, OpsFixture)

BOOST_AUTO_TEST_CASE(Method)
{
    {
        AdaptedChecked<Logic> logic;
        CHECK_OPS (c_ctor, c_invariant);

        logic.method();
        CHECK_OPS (c_ctor, c_invariant, c_invariant, c_method, c_invariant);
    }

    CHECK_OPS (c_ctor, c_invariant, c_invariant, c_method, c_invariant, c_invariant, c_dtor);
}

BOOST_AUTO_TEST_CASE(MoveCtor)
{
    {
        AdaptedChecked<Logic> logic1;
        CHECK_OPS (c_ctor, c_invariant);

        AdaptedChecked<Logic> logic2 = std::move (logic1);
        CHECK_OPS (c_ctor, c_invariant, c_mtor, c_invariant, c_invariant);
    }

    CHECK_OPS (c_ctor, c_invariant, c_mtor, c_invariant, c_invariant, c_invariant, c_dtor, c_invariant, c_dtor);
}

BOOST_AUTO_TEST_SUITE_END()
