

#include <nodec/logging.hpp>
#include <nodec/entity/entity.hpp>


#include <type_traits>


using namespace nodec;


/**
* @note the way to "template template class, call a function if it exists"
*   <https://stackoverflow.com/questions/43317175/template-template-class-call-a-function-if-it-exists/43317995>
*/

template <typename T, typename = int>
struct has_on_destroy : std::false_type {};

template <typename T>
struct has_on_destroy <T, decltype(&T::on_destroy, 0)> : std::true_type {};


template<typename T>
typename std::enable_if<has_on_destroy<T>::value>::type
call_on_destroy(T& com)
{
    com.on_destroy();

    logging::info("call on_destroy()", __FILE__, __LINE__);

}

template<typename T>
typename std::enable_if<!has_on_destroy<T>::value>::type
call_on_destroy(T&)
{

    logging::info("no on_destroy()", __FILE__, __LINE__);
}



class TestComponent
{
public:
    int field;

};


class TestComponentWithOnDestroy
{
public:
    int field;

    void on_destroy()
    {
        logging::info("hello!", __FILE__, __LINE__);
    }
};

template<typename Component>
void test(Component& com)
{
    com.field;

    com.on_destroy();
}


int main()
{
    logging::record_handlers += logging::StaticRecordHandler::make_shared(&logging::record_to_stdout_handler);

    TestComponent comA;
    TestComponentWithOnDestroy comB;
    call_on_destroy(comA);
    call_on_destroy(comB);

    return 0;
}