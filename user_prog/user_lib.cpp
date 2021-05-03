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
    {
        auto e_ans = env.lookup(ans);
        auto e_des = env.lookup(des);

        std::cout << "Ancestor("
            << ans << " " << (void const*)*e_ans << " " << **e_ans
            << ", "
            << des << " " << (void const*)*e_des << " " << **e_des
            << ") because:\n\n";
        anc.print(std::cout, 0);
        //std::cout << "\nEnvironment (post): " << env << "\n";
        std::cout << "\n\n";
    }
}
