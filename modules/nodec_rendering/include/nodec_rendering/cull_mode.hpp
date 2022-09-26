#ifndef NODEC_RENDERING__CULL_MODE_HPP_
#define NODEC_RENDERING__CULL_MODE_HPP_

namespace nodec_rendering {

/**
* @brief Back face culling mode
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