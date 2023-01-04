// Include the polymorphic serialization and registration mechanisms
#include <cereal/types/polymorphic.hpp>

 // A pure virtual base class
struct BaseClass
{
  virtual void sayType() = 0;
};


// Include any archives you plan on using with your type before you register it
// Note that this could be done in any other location so long as it was prior
// to this file being included
#include <cereal/archives/binary.hpp>
#include <cereal/archives/xml.hpp>
#include <cereal/archives/json.hpp>


// A class derived from BaseClass
struct DerivedClassOne : public BaseClass
{
  void sayType();

  int x;

  template<class Archive>
  void serialize( Archive & ar )
  { ar( x ); }
};

// Register DerivedClassOne
CEREAL_REGISTER_TYPE(DerivedClassOne);

CEREAL_REGISTER_POLYMORPHIC_RELATION(BaseClass, DerivedClassOne)

// Another class derived from BaseClass
struct EmbarrassingDerivedClass : public BaseClass
{
  void sayType();

  float y;

  template<class Archive>
  void serialize( Archive & ar )
  { ar( y ); }
};



// Register EmbarassingDerivedClass with a less embarrasing name
CEREAL_REGISTER_TYPE_WITH_NAME(EmbarrassingDerivedClass, "DerivedClassTwo");

// Note that there is no need to register the base class, only derived classes
//  However, since we did not use cereal::base_class, we need to clarify
//  the relationship (more on this later)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BaseClass, EmbarrassingDerivedClass)