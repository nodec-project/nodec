#include <nodec_modules/rendering/interfaces/bindable_resource.hpp>
#include <nodec_modules/rendering/interfaces/rendering.hpp>


namespace nodec_modules
{
namespace rendering
{
namespace interfaces
{

BindableResource::BindableResource(Rendering* target_rendering, const std::string& name) :
    NodecObject(name),
    target_rendering(target_rendering)
{

}

}
}
}