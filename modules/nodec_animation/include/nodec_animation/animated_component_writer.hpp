#ifndef NODEC_ANIMATION__ANIMATED_COMPONENT_WRITER_HPP_
#define NODEC_ANIMATION__ANIMATED_COMPONENT_WRITER_HPP_

#include <nodec_animation/resources/animation_clip.hpp>
#include <nodec_scene_serialization/scene_serialization.hpp>

namespace nodec_animation {

/**
 * @brief The AnimatedComponentWriter class provides the functionality to write animated component properties to a serializable component.
 *
 */
class AnimatedComponentWriter {
    struct InternalTag {};

public:
    class ComponentTypeInspector : public cereal::OutputArchive<ComponentTypeInspector> {
    public:
        ComponentTypeInspector(AnimatedComponentWriter &writer, InternalTag)
            : OutputArchive(this), writer_(writer) {}

        void set_next_name(const char *name) {
            next_name_ = name;
        }

        void save_value(const std::string &value) {
            auto next_name = next_name_;
            next_name_ = nullptr;

            if (!next_name || std::string(next_name) != "polymorphic_name") return;

            writer_.set_polymorphic_name(value);

            continue_flags_ &= ~0x01;
        }

        template<class T, cereal::traits::EnableIf<std::is_arithmetic<T>::value> = cereal::traits::sfinae>
        void save_value(T value) {
            auto next_name = next_name_;
            next_name_ = nullptr;

            if (!next_name || std::string(next_name) != "polymorphic_id") return;

            writer_.set_polymorphic_id(static_cast<std::uint32_t>(value));

            continue_flags_ &= ~0x02;
        }

        bool need_to_continue() const {
            return continue_flags_ != 0x00;
        }

    private:
        AnimatedComponentWriter &writer_;
        const char *next_name_;
        std::uint8_t continue_flags_{0x03};
    };

    class PropertyWriter : public cereal::InputArchive<PropertyWriter> {
    public:
        PropertyWriter(const nodec_animation::resources::AnimatedComponent &source,
                       AnimatedComponentWriter &owner, InternalTag)
            : InputArchive(this), source_(source), owner_(owner) {}

        void load_value(std::string &value) {
            // reset next_name.
            auto name = name_stack_[name_stack_.size() - 2];

            if (name_stack_.size() == 3 && std::string(name) == "polymorphic_name") {
                value = owner_.polymorphic_name_;
                return;
            }

            // source_.properties.find();
        }

        template<class T, cereal::traits::EnableIf<std::is_arithmetic<T>::value> = cereal::traits::sfinae>
        void load_value(T &value) {
            // reset next_name.
            auto name = name_stack_[name_stack_.size() - 2];

            if (name_stack_.size() == 3 && std::string(name) == "polymorphic_id") {
                value = owner_.polymorphic_id_;
                return;
            }

            if (name_stack_.size() == 5 && std::string(name) == "valid") {
                value = 1;
                return;
            }
        }

        void start_node(const char *name) {
            name_stack_.emplace_back(name);
        }

        void end_node() {
            name_stack_.pop_back();
        }

    private:
        const nodec_animation::resources::AnimatedComponent &source_;
        AnimatedComponentWriter &owner_;
        std::vector<const char *> name_stack_;
    };

    AnimatedComponentWriter(const nodec::type_info &component_type,
                            nodec_scene_serialization::SceneSerialization &serialization) {
        auto prototype = serialization.make_serializable_component(component_type);

        ComponentTypeInspector inspector(*this, InternalTag{});

        inspector(prototype);
    }

    void sample_animation(const nodec_animation::resources::AnimatedComponent &source,
                          std::unique_ptr<nodec_scene_serialization::BaseSerializableComponent> &dest) {
        // curveのhintを使いたいので, contextなどを引数で取りたい.

        PropertyWriter writer(source, *this, InternalTag{});

        writer(dest);
    }

private:
    void set_polymorphic_id(std::uint32_t id) {
        polymorphic_id_ = id;
    }

    void set_polymorphic_name(const std::string &name) {
        polymorphic_name_ = name;
    }

private:
    std::string polymorphic_name_;
    std::uint32_t polymorphic_id_;
};

template<class T>
inline void CEREAL_SAVE_FUNCTION_NAME(AnimatedComponentWriter::ComponentTypeInspector &ar, const cereal::NameValuePair<T> &pair) {
    if (!ar.need_to_continue()) return;
    // std::cout << t.name << ": " << std::endl;
    ar.set_next_name(pair.name);
    ar(pair.value);
}

template<class T, cereal::traits::EnableIf<std::is_arithmetic<T>::value> = cereal::traits::sfinae>
inline void CEREAL_SAVE_FUNCTION_NAME(AnimatedComponentWriter::ComponentTypeInspector &ar, const T &value) {
    if (!ar.need_to_continue()) return;
    // std::cout << "<arithmetic> " << value << std::endl;
    ar.save_value(value);
}

template<class CharT, class Traits, class Alloc>
inline void CEREAL_SAVE_FUNCTION_NAME(AnimatedComponentWriter::ComponentTypeInspector &ar, const std::basic_string<CharT, Traits, Alloc> &value) {
    if (!ar.need_to_continue()) return;
    // std::cout << "<string> " << value << std::endl;
    ar.save_value(value);
}

template<class T>
inline void prologue(AnimatedComponentWriter::PropertyWriter &ar,
                     const cereal::NameValuePair<T> &pair) {
    ar.start_node(pair.name);
}

template<class T>
inline void prologue(AnimatedComponentWriter::PropertyWriter &ar,
                     const T &value) {
    ar.start_node(nullptr);
}

template<class T>
inline void epilogue(AnimatedComponentWriter::PropertyWriter &ar,
                     const T &value) {
    ar.end_node();
}

template<class T>
inline void CEREAL_LOAD_FUNCTION_NAME(AnimatedComponentWriter::PropertyWriter &ar,
                                      cereal::NameValuePair<T> &pair) {
    std::cout << pair.name << ": " << std::endl;
    ar(pair.value);
}

template<class T, cereal::traits::EnableIf<std::is_arithmetic<T>::value> = cereal::traits::sfinae>
inline void CEREAL_LOAD_FUNCTION_NAME(AnimatedComponentWriter::PropertyWriter &ar,
                                      T &value) {
    std::cout << "<arithmetic> " << value << std::endl;
    ar.load_value(value);
}

template<class CharT, class Traits, class Alloc>
inline void CEREAL_LOAD_FUNCTION_NAME(AnimatedComponentWriter::PropertyWriter &ar,
                                      std::basic_string<CharT, Traits, Alloc> &value) {
    std::cout << "<string> " << value << std::endl;
    ar.load_value(value);
}


} // namespace nodec_animation

CEREAL_REGISTER_ARCHIVE(nodec_animation::AnimatedComponentWriter::ComponentTypeInspector)

CEREAL_REGISTER_ARCHIVE(nodec_animation::AnimatedComponentWriter::PropertyWriter)

#endif