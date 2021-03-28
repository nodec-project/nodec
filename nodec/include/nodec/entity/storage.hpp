#ifndef NODEC__ENTITY__STORAGE_HPP_
#define NODEC__ENTITY__STORAGE_HPP_

#include <vector>

namespace nodec
{
namespace entity
{

class BaseStorage
{

};

template<typename Entity, typename Value>
class BasicStorage : public BaseStorage
{
public:

private:

    std::vector<Value> instances;
};


}
}



#endif