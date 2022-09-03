#ifndef NODEC_SCENE_SERIALIZATION__SERIALIZABLE_COMPONENT_HPP_
#define NODEC_SCENE_SERIALIZATION__SERIALIZABLE_COMPONENT_HPP_

#include <nodec/type_info.hpp>

#include <cereal/types/polymorphic.hpp>

namespace nodec_scene_serialization {

class BaseSerializableComponent {
public:
    template<class Derived>
    BaseSerializableComponent(Derived *)
        : type_info_{&nodec::type_id<Derived>()} {
    }

    BaseSerializableComponent()
        : type_info_{&nodec::type_id<BaseSerializableComponent>()} {
    }

    virtual ~BaseSerializableComponent(){};

    template<class Archive>
    void serialize(Archive &) {}

public:
    const nodec::type_info& type_info() const noexcept {
        return *type_info_;
    }

private:
    const nodec::type_info *type_info_;
};
//
// template<class Archive>
// void serialize(Archive&, BaseSerializableComponent&) {}

} // namespace nodec_scene_serialization

// NOTE: We pre-include cereal archivers.
//  SerializableComponent will be inherited from BaseSerializableComponent class,
//  and should be registered in cereal by using CEREAL_REGISTER_TYPE, CEREAL_REGISTER_POLYMORPHIC_RELATION.
//  Before registering the class in cereal, we need to include the archivers we plan to use.
//  So, we pre-include these headers.
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/archives/portable_binary.hpp>
#include <cereal/archives/xml.hpp>


#endif