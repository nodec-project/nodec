#ifndef NODEC_RENDERING__SAMPLER_HPP_
#define NODEC_RENDERING__SAMPLER_HPP_

namespace nodec_rendering {

struct Sampler {
    enum class FilterMode {
        Point,
        Bilinear,
        Anisotropic
    };

    enum class WrapMode {
        Wrap,
        Clamp
    };

    FilterMode filter_mode{FilterMode::Bilinear};
    WrapMode wrap_mode{WrapMode::Wrap};

    bool operator==(const Sampler &other) const noexcept {
        return wrap_mode == other.wrap_mode && filter_mode == other.filter_mode;
    }

    bool operator!=(const Sampler &other) const noexcept {
        return !(*this == other);
    }
};
} // namespace nodec_rendering

template<>
struct std::hash<nodec_rendering::Sampler> {
    std::size_t operator()(const nodec_rendering::Sampler &sampler) const noexcept {
        return static_cast<std::uint8_t>(sampler.filter_mode) << 8 | static_cast<std::uint8_t>(sampler.wrap_mode);
    }
};

#endif