#include "user_lib.hpp"
#include "user_prog.hpp"
//#include "ExampleRelation.hpp"
#include "ExampleRelation2.hpp"

#include <iostream>

USER_LIB_EXPORT void run_user_lib()
{
    //user_prog p;
    //p.run();

    //ExampleRelation rel;

    //while (rel.step())
    //    std::cout << "Step\n";

    int var_counter = 0;
    lvar<Person> ans = { ++var_counter, "out: ans"};
    lvar<Person> des = { ++var_counter, "out: des"};
    Environment env = {};

    Ancestor anc(ans, des);

    while (anc.step(env, var_counter))
        std::cout << "Step\n";
}
