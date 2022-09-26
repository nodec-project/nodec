#include <nodec_rendering/sampler.hpp>

#include <cereal/cereal.hpp>

namespace nodec_rendering {

template<class Archive>
void serialize(Archive &archive, Sampler &sampler) {
    archive(
        cereal::make_nvp("filter_mode", sampler.filter_mode),
        cereal::make_nvp("wrap_mode", sampler.wrap_mode));
}

} // namespace nodec_rendering