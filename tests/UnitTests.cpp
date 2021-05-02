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
        std::string name_data = "Bob";
        env.set_var(name, &name_data);

        lvar<int> age = { 2, "data" };
        int age_data = 42;
        env.set_var(age, &age_data);

        auto p_name = env.lookup(name);
        BOOST_REQUIRE(p_name != std::nullopt);
        BOOST_REQUIRE(*p_name != nullptr);
        BOOST_TEST( *p_name == &name_data );
        BOOST_TEST( **p_name == name_data );

        auto p_age = env.lookup(age);
        BOOST_REQUIRE(p_age != std::nullopt);
        BOOST_REQUIRE(*p_age != nullptr);
        BOOST_TEST( *p_age == &age_data );
        BOOST_TEST( **p_age == age_data );
    }

    BOOST_AUTO_TEST_CASE(revert_checkpoint)
    {
        lvar<int> v1 = { 1, "v1" };
        lvar<int> v2 = { 2, "v2" };

        Environment env;

        int data1 = 11;
        env.set_var(v1, &data1);

        auto chkpt = env.save_checkpoint();

        int data2a = 22;
        env.set_var(v2, &data2a);
        BOOST_CHECK(env.lookup(v2) == &data2a);

        env.revert_to_checkpoint(chkpt);
        BOOST_CHECK(env.lookup(v2) == std::nullopt);
        BOOST_CHECK(env.lookup(v1) != std::nullopt);

        int data2b = 24;
        env.set_var(v2, &data2b);
        BOOST_CHECK(env.lookup(v2) == &data2b);
    }

    BOOST_AUTO_TEST_CASE(unify_val)
    {
        Environment env;

        lvar<std::string> name = { 1, "name" };

        std::string name_data1 = "Adam";
        env.set_var(name, &name_data1);

        std::string name_data2 = "Adam";
        BOOST_TEST( env.eq(name, &name_data2) );

        std::string name_data3 = "Bob";
        BOOST_TEST( !env.eq(name, &name_data3) );
    }

    BOOST_AUTO_TEST_CASE(unify_var)
    {
        Environment env;

        lvar<std::string> name1 = { 11, "name1" };
        lvar<std::string> name2 = { 22, "name2" };

        // Unify ungrounded variables should succeed and extend env.
        BOOST_TEST( env.eq(name1, name2) );
        BOOST_TEST( env.save_checkpoint() == 1 );

        // Unify same variables again should succeed but not change env.
        BOOST_TEST( env.eq(name2, name1) );
        BOOST_TEST( env.save_checkpoint() == 1 );
    }

    BOOST_AUTO_TEST_CASE(triangular_unify)
    {
        Environment env;

        lvar<std::string> name1 = { 11, "name1" };
        lvar<std::string> name2 = { 22, "name2" };

        BOOST_TEST( env.eq(name1, name2) );
        //BOOST_TEST( env.lookup(name1) == name2 );

        std::string name_data = "Zelda";
        BOOST_TEST( env.eq(name1, &name_data) );

        BOOST_TEST( *env.lookup(name1) == &name_data );
        BOOST_TEST( *env.lookup(name2) == &name_data );
    }

BOOST_AUTO_TEST_SUITE_END()