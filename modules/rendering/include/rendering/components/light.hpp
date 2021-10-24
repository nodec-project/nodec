#ifndef NODEC_MODULES__RENDERING__INTERFACES__CAMERA_HPP_
#define NODEC_MODULES__RENDERING__INTERFACES__CAMERA_HPP_

#include <nodec/scene_set/component.hpp>

namespace nodec_modules
{
namespace rendering
{
namespace interfaces
{

class Camera : public nodec::scene_set::Component
{
public:
    using Component::Component;


};

}
}
}


#endif