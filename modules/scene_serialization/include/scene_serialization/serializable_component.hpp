#ifndef SCENE_SERIALIZATION__SERIALIZABLE_COMPONENT_HPP_
#define SCENE_SERIALIZATION__SERIALIZABLE_COMPONENT_HPP_

#include <nodec/type_info.hpp>

#include <cereal/types/polymorphic.hpp>


namespace scene_serialization {

class BaseSerializableComponent {

public:
    template<class Derived>
    BaseSerializableComponent(Derived*)
        : type_id_{ nodec::type_seq<Derived>::value() } {

    }

    virtual ~BaseSerializableComponent() {};

    template<class Archive>
    void serialize(Archive&) {}

public:
    nodec::TypeId type_id() const { return type_id_; }

private:
    const nodec::TypeId type_id_;
};
//
//template<class Archive>
//void serialize(Archive&, BaseSerializableComponent&) {}

}


#endif