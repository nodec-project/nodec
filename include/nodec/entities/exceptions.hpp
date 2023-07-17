#ifndef NODEC__ENTITIES__EXCEPTIONS_HPP_
#define NODEC__ENTITIES__EXCEPTIONS_HPP_

#include "../formatter.hpp"

namespace nodec {
namespace entities {

template<typename Entity>
inline void throw_invalid_entity_exception(const Entity entity, const char *file, size_t line) {
    throw std::runtime_error(ErrorFormatter<std::runtime_error>(file, line)
                             << "Invalid entity detected. entity: 0x" << std::hex << entity
                             << "(entity: 0x" << to_entity(entity) << "; version: 0x" << to_version(entity) << ")");
}

template<typename Component, typename Entity>
inline void throw_no_component_exception(const Entity entity, const char *file, size_t line) {
    throw std::runtime_error(ErrorFormatter<std::runtime_error>(file, line)
                             << "Entity {0x" << std::hex << entity << "; entity: 0x" << to_entity(entity) << "; version: 0x" << to_version(entity)
                             << "} doesn't have the component {" << typeid(Component).name() << "}.");
}

} // namespace entities
} // namespace nodec
#endif