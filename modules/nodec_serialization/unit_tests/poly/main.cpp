// any archives included prior to 'myclasses.hpp'
// would also apply to the registration
#include "myclasses.hpp"

#include <cereal/archives/xml.hpp>
#include <cereal/types/polymorphic.hpp>

#include <fstream>
#include <iostream>

int main() {
    try {
        {
            std::ofstream os("polymorphism_test.xml");
            cereal::XMLOutputArchive oarchive(os);

            // Create instances of the derived classes, but only keep base class pointers
            std::shared_ptr<BaseClass> ptr1 = std::make_shared<DerivedClassOne>();
            std::shared_ptr<BaseClass> ptr2 = std::make_shared<EmbarrassingDerivedClass>();
            oarchive(ptr1, ptr2);
        }

        {
            std::ifstream is("polymorphism_test.xml");
            cereal::XMLInputArchive iarchive(is);

            // De-serialize the data as base class pointers, and watch as they are
            // re-instantiated as derived classes
            std::shared_ptr<BaseClass> ptr1;
            std::shared_ptr<BaseClass> ptr2;
            iarchive(ptr1, ptr2);

            // Ta-da! This should output:
            ptr1->sayType(); // "DerivedClassOne"
            ptr2->sayType(); // "EmbarrassingDerivedClass. Wait.. I mean DerivedClassTwo!"
        }
    }
    catch (std::exception& e) {
        std::cout << e.what();
    }

    return 0;
}