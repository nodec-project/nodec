#ifndef NODEC_MODULES__RENDERING__INTERFACES__BINDABLE_HPP_
#define NODEC_MODULES__RENDERING__INTERFACES__BINDABLE_HPP_

#include <nodec/nodec_object.hpp>

#include <unordered_map>

namespace nodec_modules
{
namespace rendering
{
namespace interfaces
{

class Rendering;

class BindableResource : public nodec::NodecObject
{
public:
    BindableResource(Rendering* target_rendering, const std::string& name);

protected:
    /**
    * rendering module MUST be alive before destorying this.
    */
    Rendering* target_rendering;
};

}
}
}


#endif