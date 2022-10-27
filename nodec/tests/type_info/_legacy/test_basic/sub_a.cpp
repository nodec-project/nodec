#include "sub_a.hpp"


nodec::type_seq_index_type getA()
{
    return nodec::type_seq_index<TestA>::value();
}


nodec::type_seq_index_type getB() {
    return nodec::type_seq_index<TestB>::value();
}

void test()
{
    //nodec::entity::internal::type_seq::test();
}