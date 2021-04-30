#include "boost/test/unit_test.hpp"
#include "Environment.hpp"

namespace std
{
    ostream& operator <<(ostream& os, nullopt_t const&)
    {
        return os << "empty";
    }

    template <typename T>
    ostream& operator <<(ostream& os, optional<T> value)
    {
        if (value)
            return os << value;
        else
            return os << "empty";
    }
}

BOOST_AUTO_TEST_SUITE(EnvironmentTests)

    BOOST_AUTO_TEST_CASE(empty_no_var)
    {
        Environment env;
        lvar<int> dummy;
        BOOST_TEST( env.lookup(dummy) == std::nullopt );
    }

    BOOST_AUTO_TEST_CASE(empty_checkpoint_good)
    {
        Environment env;
        BOOST_TEST( env.save_checkpoint() == 0 );
        BOOST_REQUIRE_NO_THROW( env.revert_to_checkpoint(0) );
    }

    BOOST_AUTO_TEST_CASE(empty_checkpoint_bad)
    {
        Environment env;
        BOOST_CHECK_THROW( env.revert_to_checkpoint(1), std::logic_error );
    }

    BOOST_AUTO_TEST_CASE(lookup_var)
    {
        Environment env;

        lvar<std::string> name = { 1, "name" };
        std::string value = "Bob";
        env.set_var(name, &value);

        lvar<int> data = { 2, "data" };
        int data_value = 42;
        env.set_var(data, &data_value);

        //BOOST_TEST( env.lookup(dummy) == std::nullopt );
    }


BOOST_AUTO_TEST_SUITE_END()