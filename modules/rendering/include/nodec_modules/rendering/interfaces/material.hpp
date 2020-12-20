#ifndef NODEC_MODULES__RENDERING__INTERFACES__MATERIAL_HPP_
#define NODEC_MODULES__RENDERING__INTERFACES__MATERIAL_HPP_

#include "shader.hpp"

#include <nodec/nodec_object.hpp>
#include <nodec/vector3.hpp>

#include <map>

namespace nodec_modules
{
namespace rendering
{
namespace interfaces
{

class Material : public BindableResource
{
public:
    Material(Rendering* target_rendering,
             nodec::NodecObject::Holder<Shader> shader);

    ~Material();

public:
    const Shader& shader() const noexcept;

    const std::map<std::string, float>& float_properties();
    bool set_float(const std::string& name, const float& value);
    bool get_float(const std::string& name, float& out);

    const std::map<std::string, nodec::Vector3f>& vector3_properties();
    bool set_vector3(const std::string& name, const nodec::Vector3f& value);
    bool get_vector3(const std::string& name, nodec::Vector3f& out);


protected:
    nodec::NodecObject::Holder<Shader> shader_;

    std::map<std::string, float> float_properties_;
    std::map<std::string, nodec::Vector3f> vector3_properties_;

private:
    template<typename T>
    static bool get_value_generic(std::map<std::string, T>& properties,
                                  const std::string& name,
                                  T& out);

    template<typename T>
    static bool set_value_generic(std::map<std::string, T>& properties,
                                  const std::string& name,
                                  const T& value);


};
}
}
}

#endif