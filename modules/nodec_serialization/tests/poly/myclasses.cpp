#include "myclasses.hpp"
#include <iostream>

void DerivedClassOne::sayType()
{
  std::cout << "DerivedClassOne" << std::endl; 
}

void EmbarrassingDerivedClass::sayType()
{
  std::cout << "EmbarrassingDerivedClass. Wait.. I mean DerivedClassTwo!" << std::endl; 
}