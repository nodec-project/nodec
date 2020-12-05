#ifndef NODEC__MATRIX4X4_HPP_
#define NODEC__MATRIX4x4_HPP_

namespace nodec
{
    template<typename T>
    class Matrix4x4
    {
    public:
        union
        {
            T m[16];
        };
    };
}

#endif