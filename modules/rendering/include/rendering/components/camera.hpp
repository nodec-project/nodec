#ifndef RENDERING__COMPONENTS__CAMERA_HPP_
#define RENDERING__COMPONENTS__CAMERA_HPP_


namespace rendering {
namespace components {

class Camera {

public:

    float farClipPlane{ 100.0f };
    float nearClipPlane{ 0.01f };
    float fovAngle{ 45.0f };

};

}
}


#endif