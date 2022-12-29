#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <nodec/entities/registry.hpp>

#include <algorithm>

TEST_CASE("Testing emplace_component.") {
    {
        nodec::entities::Registry registry;
        auto e0 = registry.create_entity();
        
        auto result = registry.emplace_component<int>(e0, 100);
        CHECK(result.first == 100);
        CHECK(result.second);

        result.first = 200;
        CHECK(registry.get_component<int>(e0) == 200);
    }
}

TEST_CASE("Testing remove_component.") {
    using namespace nodec::entities;

    Registry registry;

    
    std::array<Entity, 3> entities;

    std::generate(entities.begin(), entities.end(), [&]() { return registry.create_entity(); });

    registry.emplace_component<int>(entities[0]);
    registry.emplace_component<char>(entities[0]);
    registry.emplace_component<double>(entities[0]);
    
    registry.emplace_component<int>(entities[1]);
    registry.emplace_component<char>(entities[1]);
    
    registry.emplace_component<int>(entities[2]);

    REQUIRE(registry.try_get_component<int>(entities[2]) != nullptr);
    CHECK(registry.remove_component<int>(entities[2]));
    CHECK(registry.try_get_component<int>(entities[2]) == nullptr);
}

TEST_CASE("Testing component_constructed signal.") {
    {
        nodec::entities::Registry registry;
        auto e0 = registry.create_entity();

        bool called = false;

        registry.component_constructed<int>().connect([&](auto& reg, auto entt) {
            CHECK(entt == e0);
            CHECK(&reg == &registry);
            called = true;
            });

        REQUIRE(registry.emplace_component<int>(e0).second);
        CHECK(called);
    }
}

TEST_CASE("Testing all_of, any_of.") {
    using namespace nodec::entities;

    Registry registry;

    const auto e0 = registry.create_entity();
    const auto e1 = registry.create_entity();

    registry.emplace_component<int>(e1);
    registry.emplace_component<char>(e1);

    CHECK(registry.all_of<>(e0));
    CHECK(!registry.any_of<>(e1));

    CHECK(!registry.all_of<int, const char>(e0));
    CHECK(registry.all_of<const int, char>(e1));

    CHECK(!registry.any_of<const int, double>(e0));
    CHECK(registry.any_of<const int, double>(e1));
}


//
//
//#include <nodec/logging.hpp>
//#include <nodec/stopwatch.hpp>
//
//#include <nodec/entities/entity.hpp>
//#include <nodec/entities/registry.hpp>
//#include <nodec/entities/sparse_table.hpp>
//
//#include <type_traits>
//#include <vector>
//
//#include <cstdlib>
//
//using namespace nodec;
//using namespace nodec::entities;
//
///**
// * @note the way to "template template class, call a function if it exists"
// *   <https://stackoverflow.com/questions/43317175/template-template-class-call-a-function-if-it-exists/43317995>
// */
//
//template<typename T, typename = int>
//struct has_on_destroy : std::false_type {};
//
//template<typename T>
//struct has_on_destroy<T, decltype(&T::on_destroy, 0)> : std::true_type {};
//
//template<typename T>
//typename std::enable_if<has_on_destroy<T>::value>::type
//call_on_destroy(T &com) {
//    com.on_destroy();
//
//    logging::info("call on_destroy()", __FILE__, __LINE__);
//}
//
//template<typename T>
//typename std::enable_if<!has_on_destroy<T>::value>::type
//call_on_destroy(T &) {
//    logging::info("no on_destroy()", __FILE__, __LINE__);
//}
//
//class Test {
//public:
//    ~Test() {
//        logging::info("called", __FILE__, __LINE__);
//    }
//};
//
//class ComponentA {
//public:
//    ComponentA(int field)
//        : field(field) {
//        //logging::InfoStream(__FILE__, __LINE__) << "constructed.";
//    }
//
//    ComponentA(const ComponentA &other)
//        : field(other.field) {
//        //logging::InfoStream(__FILE__, __LINE__) << "copy-contructed.";
//    }
//
//    ComponentA(ComponentA &&other) noexcept
//        : field(std::move(other.field)) {
//        //logging::InfoStream(__FILE__, __LINE__) << "move-constructed.";
//    }
//
//    ComponentA &operator=(ComponentA &&other) noexcept {
//        if (this != &other) {
//            field = std::move(other.field);
//        }
//        return *this;
//    }
//
//    ~ComponentA() {
//        //logging::InfoStream(__FILE__, __LINE__) << "destroyed.";
//    }
//
//public:
//    int field;
//};
//
//struct ComponentB {
//    std::string field;
//};
//
//struct BaseComponent {
//    int bfield;
//};
//
//struct DerivedComponentA : public BaseComponent {
//    int dfield;
//};
//
//struct DerivedComponentB : public BaseComponent {
//    int dfield;
//};
//
//class TestComponentWithOnDestroy {
//public:
//    int field;
//
//    void on_destroy() {
//        logging::info("hello!", __FILE__, __LINE__);
//    }
//};
//
//template<typename Component>
//void test(Component &com) {
//    com.field;
//
//    com.on_destroy();
//}
//
//template<typename Table>
//void print(Table &table) {
//    logging::InfoStream info_stream(__FILE__, __LINE__);
//    for (auto iter = table.begin(); iter != table.end(); ++iter) {
//        // info_stream << "( " << iter->first << ", " << iter->second << "), ";
//        info_stream << "( " << *iter << "), ";
//    }
//
//    info_stream << std::endl;
//
//    for (auto &v : table) {
//        // info_stream << "( " << v.first << ", " << v.second << "), ";
//        info_stream << "( " << v << "), ";
//    }
//}
//
//int main() {
//    try {
//        logging::record_handlers().connect(logging::record_to_stdout_handler);
//
//
//
//        Stopwatch<std::chrono::steady_clock> sw;
//
//        {
//            Registry registry;
//
//            for (auto i = 0; i < 1000000; ++i) {
//                const auto entity = registry.create_entity();
//                auto pair = registry.emplace_component<ComponentA>(entity, i);
//
//                if (i % 2) {
//                    auto pair = registry.emplace_component<ComponentB>(entity, Formatter() << i);
//                }
//            }
//
//            // auto view = registry.view<const ComponentA, ComponentB>(type_list<ComponentB>());
//
//            using namespace std::chrono;
//           
//            Stopwatch<high_resolution_clock> sw;
//
//            sw.restart();
//            registry.view<ComponentA, ComponentB>().each([](auto entt, ComponentA &compA, ComponentB &compB) {
//                //auto &compA = std::get<0>(comps);
//                //auto &compB = std::get<1>(comps);
//
//                compA.field = 2;
//                compB.field = "B";
//            });
//
//            logging::InfoStream(__FILE__, __LINE__) << duration_cast<milliseconds>(sw.elapsed()).count();
//
//            
//            sw.restart();
//            auto view = registry.view<ComponentA, ComponentB>();
//            for (const auto entt : view) {
//                // auto& compA = view.get<ComponentA>(entity);
//                // auto &comps = view.get<0>(entity);
//                // auto comps = view.get(entity);
//                auto comps = view.get(entt);
//                auto &compA = std::get<0>(comps);
//                auto &compB = std::get<1>(comps);
//
//
//                //auto &compA = view.get<0>(entt);
//                //auto &compB = view.get<1>(entt);
//
//                compA.field = 1;
//                compB.field = "A";
//
//                // auto& compA = view.get<0>(entity);
//                // auto &compA = view.get<ComponentA>(entity);
//
//                // logging::InfoStream(__FILE__, __LINE__) << entt << ":" << compA.field;
//                // logging::InfoStream(__FILE__, __LINE__) << entity << ":" << std::get<0>(comps).field << ","
//                //     << std::get<1>(comps).field;
//            }
//            logging::InfoStream(__FILE__, __LINE__) << duration_cast<milliseconds>(sw.elapsed()).count();
//
//            //registry.view<ComponentA>().each([](auto entt, ComponentA & compA) {
//            //    logging::InfoStream(__FILE__, __LINE__) << entt << "#" << compA.field;
//            //});
//            //{
//            //    const auto &cregistry = registry;
//            //    auto view = cregistry.view<ComponentA>();
//
//            //}
//            // auto it = view.begin();
//            // logging::InfoStream(__FILE__, __LINE__) << *it;
//
//            // using Type  = const ComponentB;
//            // constness_as<ComponentA, Type>::type;
//        }
//
//        //{
//        //    logging::InfoStream info(__FILE__, __LINE__);
//
//        //    Registry registry;
//
//        //    auto entt_a = registry.create_entity();
//        //    registry.emplace_component<ComponentB>(entt_a);
//        //    auto results = registry.remove_components<ComponentB, BaseComponent>(entt_a);
//        //
//        //    info << std::get<0>(results) << ", " << std::get<1>(results);
//        //}
//
//        // ecs::Registry registry;
//
//        //{
//        //    ecs::BasicStorage<ecs::Entity, DerivedComponentA> derivedAStorage;
//        //    auto pair = derivedAStorage.emplace(10);
//        //    pair.first.bfield = 1;
//        //    pair.first.dfield = 2;
//
//        //    auto* baseStorage = static_cast<ecs::BaseStorage<ecs::Entity>*>(&derivedAStorage);
//
//        //    {
//        //        auto* storage = dynamic_cast<ecs::BasicStorage<ecs::Entity, DerivedComponentA>*>(baseStorage);
//        //        logging::InfoStream(__FILE__, __LINE__) << storage;
//        //    }
//
//        //    {
//        //        auto* storage = dynamic_cast<ecs::BasicStorage<ecs::Entity, BaseComponent>*>(baseStorage);
//        //        logging::InfoStream(__FILE__, __LINE__) << storage;
//        //    }
//        //    {
//        //        auto iter = baseStorage->begin();
//        //        logging::InfoStream(__FILE__, __LINE__) << *iter;
//
//        //        auto* com_void = baseStorage->try_get_opaque(*iter);
//        //        auto* com_base = static_cast<BaseComponent*>(com_void);
//        //        auto* com_derive_a = static_cast<DerivedComponentA*>(com_void);
//        //        {
//        //            //auto* com_derive_a = dynamic_cast<DerivedComponentA*>(com_base);
//
//        //        }
//        //        logging::InfoStream(__FILE__, __LINE__) << com_base->bfield;
//        //        logging::InfoStream(__FILE__, __LINE__) << com_derive_a->bfield << ", " << com_derive_a->dfield;
//
//        //    }
//        //}
//        // for (auto i = 0; i < 10; ++i) {
//        //    const auto entity = registry.create_entity();
//        //    auto& comA = registry.add_component<DerivedComponentA>(entity);
//        //    logging::InfoStream info(__FILE__, __LINE__);
//        //    info << entity << ": " << comA.dfield;
//        //    if (i % 2 == 0) {
//        //        auto& comB = registry.add_component<DerivedComponentB>(entity);
//        //        info << ", " << comB.dfield;
//        //    }
//        //}
//
//        //{
//        //    auto view = registry.view<BaseComponent>();
//        //    for (auto entity : view) {
//
//        //        logging::InfoStream(__FILE__, __LINE__) << entity << ": ";
//        //    }
//        //}
//
//        // for (auto i = 0; i < 10; ++i) {
//        //     const auto entity = registry.create_entity();
//        //     auto& comB = registry.add_component<ComponentB>(entity, "HOGE");
//        //     logging::InfoStream info(__FILE__, __LINE__);
//        //     info << entity << ": " << comB.field;
//
//        //    if (i % 2 == 0) {
//        //        auto& comA = registry.add_component<ComponentA>(entity, i);
//        //        info << ", " << comA.field;
//        //    }
//        //}
//
//        ////{
//        ////    auto view = registry.view<ComponentA>();
//
//        ////    for (auto iter = view.begin(); iter != view.end(); ++iter) {
//        ////        auto entity = *iter;
//
//        ////        logging::InfoStream(__FILE__, __LINE__) << entity;
//        ////    }
//        ////}
//        //{
//        //    auto view = registry.view<ComponentA, const ComponentB>();
//        //    for (auto entity : view) {
//        //        auto coms = registry.get_components<ComponentA, ComponentB>(entity);
//
//        //        //auto& comA = registry.get_component<ComponentA>(entity);
//        //        //auto& comB = registry.get_component<const ComponentB>(entity);
//        //        auto& comA = std::get<0>(coms);
//        //        auto& comB = std::get<1>(coms);
//        //
//        //        comA.field *= 2;
//        //        logging::InfoStream(__FILE__, __LINE__) << entity << ": " << comA.field << ", " << comB.field;
//        //    }
//        //}
//
//        //{
//        //    auto view = registry.view<ComponentA>();
//        //    for (auto entity : view) {
//        //        auto& comA = registry.get_component<ComponentA>(entity);
//
//        //        logging::InfoStream(__FILE__, __LINE__) << entity << ": " << comA.field;
//        //    }
//        //}
//
//        // auto entity = registry.create_entity();
//
//        //
//        // logging::InfoStream(__FILE__, __LINE__) << registry.is_valid(entity);
//
//        // registry.add_component<ComponentA>(entity, 1);
//        // registry.add_component<ComponentB>(entity, "HOGE");
//        ////registry.add_component<TestComponent>(entity, 0);
//        //{
//        //    auto& componentA = registry.get_component<const ComponentA>(entity);
//        //    logging::InfoStream(__FILE__, __LINE__) << componentA.field;
//
//        //    //auto& componentB = registry.get_component<ComponentB>(entity);
//        //    //logging::InfoStream(__FILE__, __LINE__) << componentB.field;
//        //}
//        //{
//        //    ////sw.restart();
//        //    //auto is_removed = registry.remove_components<ComponentB, ComponentA, ComponentA, ComponentB>(entity);
//        //    ////sw.stop();
//        //    ////logging::InfoStream(__FILE__, __LINE__) << std::chrono::duration<float>(sw.elapsed()).count();
//        //    //logging::InfoStream(__FILE__, __LINE__) << std::boolalpha << std::get<0>(is_removed);
//        //    //logging::InfoStream(__FILE__, __LINE__) << std::boolalpha << std::get<1>(is_removed);
//        //    //logging::InfoStream(__FILE__, __LINE__) << std::boolalpha << std::get<2>(is_removed);
//        //    //logging::InfoStream(__FILE__, __LINE__) << std::boolalpha << std::get<3>(is_removed);
//        //    ////logging::InfoStream(__FILE__, __LINE__) << std::get<0>(is_removed);
//        //    ////logging::InfoStream(__FILE__, __LINE__) << std::get<1>(is_removed);
//        //}
//        //{
//        //    auto components = registry.get_components<ComponentA, ComponentB>(entity);
//
//        //    logging::InfoStream(__FILE__, __LINE__) << std::get<0>(components).field;
//        //    logging::InfoStream(__FILE__, __LINE__) << std::get<1>(components).field;
//        //    //logging::InfoStream(__FILE__, __LINE__) << std::get<2>(components).field;
//        //}
//
//        //{
//        //    auto view = registry.view<ComponentA, ComponentB>();
//        //    view.test();
//        //}
//
//        // registry.destroy_entity(entity);
//
//        ////{
//        ////    auto& componentA = registry.get_component<ComponentA>(entity);
//        ////    logging::InfoStream(__FILE__, __LINE__) << componentA.field;
//        ////}
//        //{
//        //    auto& component = registry.get_component<ComponentA>(entity);
//        //}
//
//        // logging::InfoStream(__FILE__, __LINE__) << registry.is_valid(entity);
//
//        // entity = registry.create_entity();
//        // throw ecs::InvalidEntityException(entity, __FILE__, __LINE__);
//
//        // std::unordered_map<int, int> map;
//        // map.emplace(0, 1);
//        // for (auto iter = map.begin(); iter != map.end(); ++iter)
//        //{
//        //     *iter = std::pair<const int, int>(0, 1);
//        // }
//
//        // ecs::SparseTable<uint16_t> table;
//        // for (auto i = 0; i < 50; ++i)
//        //{
//        //     table[i * 2] = i;
//        // }
//
//        // print(table);
//
//        // table.erase(6);
//        // logging::InfoStream(__FILE__, __LINE__) << table.group_count();
//
//        // print(table);
//        // auto back = table.end();
//        //--back;
//        ////logging::InfoStream(__FILE__, __LINE__) << back->first << ", " << back->second;
//        // logging::InfoStream(__FILE__, __LINE__) << *back;
//        //
//
//        // entity::Entity entity = 0;
//
//        // entity::BasicSparseGroup<size_t, entity::DEFAULT_SPARSE_GROUP_SIZE> group;
//        // group[0] = 1;
//        // group[3] = 2;
//        // group[2] = 9;
//        // group.test();
//        // logging::InfoStream(__FILE__, __LINE__) << group[2];
//
//        // group.erase(2);
//        // group.test();
//
//        // TestComponent comA;
//        // TestComponentWithOnDestroy comB;
//        // call_on_destroy(comA);
//        // call_on_destroy(comB);
//
//        //
//        //
//        // std::vector<Test> vec;
//        // vec.reserve(10);
//        // vec.emplace_back();
//        // vec.emplace_back();
//        // vec.emplace_back();
//
//        // vec.pop_back();
//        ////12638232278978672507
//        // logging::InfoStream(__FILE__, __LINE__) << vec.capacity();
//
//        // logging::InfoStream(__FILE__, __LINE__) << typeid(int).hash_code();
//    } catch (std::exception &e) {
//        logging::ErrorStream(__FILE__, __LINE__) << e.what();
//    }
//
//    logging::info("END", __FILE__, __LINE__);
//
//    return 0;
//}