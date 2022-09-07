#ifndef NODEC_RENDERING__RENDERING_HPP_
#define NODEC_RENDERING__RENDERING_HPP_

namespace nodec_rendering {

/**
* @brief Backface culling mode
*/
enum class CullMode {
    //! Disable culling.
    Off,

    //! Cull front-facing geometry.
    Front,

    //! Cull back-facing geometry.
    Back
};

}

#endif