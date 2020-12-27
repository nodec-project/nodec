#include "resources.hpp"

using namespace nodec;

NodecObject::Reference<Resources> Resources::instance;



Resources* Resources::get()
{
    if (auto locked = instance.lock())
    {
        return locked.get();
    }

    throw NodecException("Resources Component is not instanciated.", __FILE__, __LINE__);
}

void Resources::on_awake()
{
    instance = shared_from(this);
}
