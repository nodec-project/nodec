#include "sub_a.hpp"


#include <nodec/logging.hpp>

using namespace nodec;


class TestC
{

};


int main()
{
    logging::record_handlers += logging::StaticRecordHandler::make_shared(&logging::record_to_stdout_handler);

    logging::InfoStream(__FILE__, __LINE__) << entity::type_seq<TestB>::value();
    logging::InfoStream(__FILE__, __LINE__) << entity::type_seq<TestA>::value();
    
    logging::InfoStream(__FILE__, __LINE__) << getA();
    logging::InfoStream(__FILE__, __LINE__) << getB();


    logging::InfoStream(__FILE__, __LINE__) << entity::type_seq<TestC>::value();

    //entity::internal::type_seq::test();
    test();


    logging::InfoStream(__FILE__, __LINE__) << "End";

    return 0;
}