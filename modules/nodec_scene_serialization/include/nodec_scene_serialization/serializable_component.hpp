#ifndef NODEC_SCENE_SERIALIZATION__SERIALIZABLE_COMPONENT_HPP_
#define NODEC_SCENE_SERIALIZATION__SERIALIZABLE_COMPONENT_HPP_

#include <nodec/type_info.hpp>

#include <cereal/types/polymorphic.hpp>

namespace nodec_scene_serialization {

class BaseSerializableComponent {
public:
    template<class Derived>
    BaseSerializableComponent(Derived *)
        : type_id_{nodec::type_seq<Derived>::value()} {
    }

    BaseSerializableComponent()
        : type_id_{nodec::type_seq<BaseSerializableComponent>::value()} {
    }

    virtual ~BaseSerializableComponent(){};

    template<class Archive>
    void serialize(Archive &) {}

public:
    nodec::TypeId type_id() const {
        return type_id_;
    }

private:
    const nodec::TypeId type_id_;
};
//
// template<class Archive>
// void serialize(Archive&, BaseSerializableComponent&) {}

} // namespace nodec_scene_serialization

#endif