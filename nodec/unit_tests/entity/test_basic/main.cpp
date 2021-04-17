

#include <nodec/logging.hpp>

#include <nodec/ecs/entity.hpp>
#include <nodec/ecs/registry.hpp>
#include <nodec/ecs/sparse_table.hpp>


#include <type_traits>
#include <vector>

#include <cstdlib>

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

class ComponentA
{
public:
    ComponentA(int field)
        :field(field)
    {

        logging::InfoStream(__FILE__, __LINE__) << "constructed.";
    }

    ComponentA(const ComponentA& other)
        :field(other.field)
    {
        logging::InfoStream(__FILE__, __LINE__) << "copied.";
    }


    ComponentA(ComponentA&& other) noexcept
        :field(std::move(other.field))
    {
        logging::InfoStream(__FILE__, __LINE__) << "moved.";
    }

    ~ComponentA()
    {
        logging::InfoStream(__FILE__, __LINE__) << "destroyed.";
    }

public:
    int field;



};

struct ComponentB
{
    std::string field;
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

template<typename Table>
void print(Table& table)
{
    logging::InfoStream info_stream(__FILE__, __LINE__);
    for (auto iter = table.begin(); iter != table.end(); ++iter)
    {
        //info_stream << "( " << iter->first << ", " << iter->second << "), ";
        info_stream << "( " << *iter << "), ";
    }

    info_stream << std::endl;

    for (auto& v : table)
    {
        //info_stream << "( " << v.first << ", " << v.second << "), ";
        info_stream << "( " << v << "), ";
    }
}

int main()
{
    try
    {


        logging::record_handlers += logging::StaticRecordHandler::make_shared(&logging::record_to_stdout_handler);

        ecs::Registry registry;

        auto entity = registry.create_entity();

        
        logging::InfoStream(__FILE__, __LINE__) << registry.is_valid(entity);

        registry.add_component<ComponentA>(entity, 1);
        registry.add_component<ComponentB>(entity, "HOGE");
        //registry.add_component<TestComponent>(entity, 0);
        {
            auto& componentA = registry.get_component<ComponentA>(entity);
            logging::InfoStream(__FILE__, __LINE__) << componentA.field;

            auto& componentB = registry.get_component<ComponentB>(entity);
            logging::InfoStream(__FILE__, __LINE__) << componentB.field;
        }

        {
            auto components = registry.get_components<ComponentA, ComponentB>(entity);

            logging::InfoStream(__FILE__, __LINE__) << std::get<0>(components).field;
            logging::InfoStream(__FILE__, __LINE__) << std::get<1>(components).field;
            //logging::InfoStream(__FILE__, __LINE__) << std::get<2>(components).field;
        }
        registry.destroy_entity(entity);

        //{
        //    auto& componentA = registry.get_component<ComponentA>(entity);
        //    logging::InfoStream(__FILE__, __LINE__) << componentA.field;
        //}

        logging::InfoStream(__FILE__, __LINE__) << registry.is_valid(entity);

        entity = registry.create_entity();
        //throw ecs::InvalidEntityException(entity, __FILE__, __LINE__);


        //std::unordered_map<int, int> map;
        //map.emplace(0, 1);
        //for (auto iter = map.begin(); iter != map.end(); ++iter)
        //{
        //    *iter = std::pair<const int, int>(0, 1);
        //}

        //entity::SparseTable<uint16_t> table;
        //for (auto i = 0; i < 50; ++i)
        //{
        //    table[i * 2] = i;
        //}

        //print(table);

        //table.erase(6);
        //logging::InfoStream(__FILE__, __LINE__) << table.group_count();

        //print(table);
        //auto back = table.end();
        //--back;
        ////logging::InfoStream(__FILE__, __LINE__) << back->first << ", " << back->second;
        //logging::InfoStream(__FILE__, __LINE__) << *back;
        //

        //entity::Entity entity = 0;

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
    }
    catch (std::exception& e)
    {
        logging::ErrorStream(__FILE__, __LINE__) << e.what();
    }

    logging::info("END", __FILE__, __LINE__);

    return 0;
}