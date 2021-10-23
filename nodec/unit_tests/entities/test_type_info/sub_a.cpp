#include "sub_a.hpp"


nodec::entity::TypeId getA()
{
    return nodec::entity::type_seq<TestA>::value();
}


nodec::entity::TypeId getB()
{
    return nodec::entity::type_seq<TestB>::value();
}

void test()
{
    //nodec::entity::internal::type_seq::test();
}