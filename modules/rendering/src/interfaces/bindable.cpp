#include <nodec_modules/rendering/interfaces/bindable.hpp>
#include <nodec_modules/rendering/interfaces/rendering.hpp>


namespace nodec_modules
{
    namespace rendering
    {
        namespace interfaces
        {
            void IBindable::bind(Rendering* rendering)
            {
                binded_renderings.emplace(rendering->id(), rendering);

                
                bind_impl(rendering);
            }

            void IBindable::unbind()
            {
                for (auto iter = binded_renderings.begin(); iter != binded_renderings.end(); ++iter)
                {
                    unbind_impl(iter->second);
                }
            }

        }
    }
}