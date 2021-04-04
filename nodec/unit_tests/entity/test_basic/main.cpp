

#include <nodec/logging.hpp>

#include <nodec/entity/entity.hpp>
#include <nodec/entity/registry.hpp>
#include <nodec/entity/sparse_table.hpp>


#include <type_traits>
#include <vector>


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

class Test
{
public:
    ~Test()
    {
        logging::info("called", __FILE__, __LINE__);
    }
};

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


    entity::SparseTable<uint16_t> table;

    for (auto i = 0; i < 10; ++i)
    {
        table[i * 2] = i;
    }

    table.test();
    entity::Entity entity = 0;

    //entity::BasicSparseGroup<size_t, entity::DEFAULT_SPARSE_GROUP_SIZE> group;
    //group[0] = 1;
    //group[3] = 2;
    //group[2] = 9;
    //group.test();
    //logging::InfoStream(__FILE__, __LINE__) << group[2];

    //group.erase(2);
    //group.test();


    //TestComponent comA;
    //TestComponentWithOnDestroy comB;
    //call_on_destroy(comA);
    //call_on_destroy(comB);

    //
    //
    //std::vector<Test> vec;
    //vec.reserve(10);
    //vec.emplace_back();
    //vec.emplace_back();
    //vec.emplace_back();

    //vec.pop_back();
    ////12638232278978672507
    //logging::InfoStream(__FILE__, __LINE__) << vec.capacity();

    //logging::InfoStream(__FILE__, __LINE__) << typeid(int).hash_code();

    logging::info("END", __FILE__, __LINE__);

    return 0;
}