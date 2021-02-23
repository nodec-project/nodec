/**
* 
*/

#include <nodec/application.hpp>

namespace nodec
{

int Application::run()
{
    try
    {
        return main();
    }
    catch (...)
    {
        return on_error_exit();
    }
}

}