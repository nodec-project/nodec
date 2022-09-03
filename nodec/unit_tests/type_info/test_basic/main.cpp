#include "sub_a.hpp"


#include <nodec/logging.hpp>
#include <nodec/type_info.hpp>



class TestC
{

};


int main()
{
    using namespace nodec;
    logging::record_handlers().connect(logging::record_to_stdout_handler);


    logging::InfoStream(__FILE__, __LINE__) << type_seq_index<TestB>::value();
    logging::InfoStream(__FILE__, __LINE__) << type_seq_index<TestA>::value();
    
    logging::InfoStream(__FILE__, __LINE__) << getA();
    logging::InfoStream(__FILE__, __LINE__) << getB();

    //nodec::type_info infoA{nodec::in_place_type_t<TestA>{}};
    nodec::type_info infoA{nodec::in_place_type<TestA>};
    nodec::type_info infoB{nodec::in_place_type<TestB>};

    auto infoA1 = nodec::type_id<TestA>();
    auto infoA2 = nodec::type_id<const TestA>();

    logging::InfoStream(__FILE__, __LINE__) << (infoA == infoB);
    logging::InfoStream(__FILE__, __LINE__) << (infoA == infoA1);


    logging::InfoStream(__FILE__, __LINE__) << typeid(TestA).name() << "\n"
                                            << typeid(TestA&).name();

    //logging::InfoStream(__FILE__, __LINE__) << entity::type_seq<TestC>::value();

    ////entity::internal::type_seq::test();
    //test();


    //logging::InfoStream(__FILE__, __LINE__) << "End";

    return 0;
}