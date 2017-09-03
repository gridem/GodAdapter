#include "ut.h"

#include <god_adapter/invariant.h>

const char* const c_assert = "assert";

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
		op (c_assert);
	}
};

DECL_ADAPTER (Logic, method)

BOOST_FIXTURE_TEST_SUITE(invariant, OpsFixture)

BOOST_AUTO_TEST_CASE(Method)
{
	{
		AdaptedChecked<Logic> logic;
		CHECK_OPS (c_ctor, c_assert);

		logic.method();
		CHECK_OPS (c_ctor, c_assert, c_assert, c_method, c_assert);
	}

	CHECK_OPS (c_ctor, c_assert, c_assert, c_method, c_assert, c_assert, c_dtor);
}

BOOST_AUTO_TEST_CASE(MoveCtor)
{
	{
		AdaptedChecked<Logic> logic1;
		CHECK_OPS (c_ctor, c_assert);

		AdaptedChecked<Logic> logic2 = std::move (logic1);
		CHECK_OPS (c_ctor, c_assert, c_mtor, c_assert, c_assert);
	}

	CHECK_OPS (c_ctor, c_assert, c_mtor, c_assert, c_assert, c_assert, c_dtor, c_assert, c_dtor);
}

BOOST_AUTO_TEST_SUITE_END()
