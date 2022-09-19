#include "test_comp.hpp"

#include <fstream>
#include <iostream>

int main() {
    using namespace nodec_scene_serialization;

    static_assert(std::is_polymorphic<nodec_scene_serialization::BaseSerializableComponent>::value, "AAA");

    try {
        {
            std::ofstream os("polymorphism_test.xml");
            cereal::XMLOutputArchive oarchive(os);

            // Create instances of the derived classes, but only keep base class pointers
            std::shared_ptr<BaseSerializableComponent> ptr1 = std::make_shared<TestComponent>();
            //std::shared_ptr<BaseSerializableComponent> ptr2 = std::make_shared<EmbarrassingDerivedClass>();
            //oarchive(ptr1, ptr2);
            oarchive(ptr1);
        }

        {
            std::ifstream is("polymorphism_test.xml");
            cereal::XMLInputArchive iarchive(is);

            // De-serialize the data as base class pointers, and watch as they are
            // re-instantiated as derived classes
            std::shared_ptr<BaseSerializableComponent> ptr1;
            //std::shared_ptr<BaseSerializableComponent> ptr2;
            //iarchive(ptr1, ptr2);
            iarchive(ptr1);

            // Ta-da! This should output:
            std::cout << ptr1->type_info().seq_index(); // "DerivedClassOne"
            //std::cout << ptr2->type_info(); // "EmbarrassingDerivedClass. Wait.. I mean DerivedClassTwo!"
        }
    } catch (std::exception &e) {
        std::cout << e.what();
    }

    return 0;
}