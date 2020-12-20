#include <nodec/event.hpp>
#include <nodec/logging.hpp>

#include <iostream>

using namespace nodec;

class Updater
{
public:
    event::Event<Updater&> on_update;
};

void sub_func(Updater& updater)
{
    std::cout << "hey" << std::endl;
}


void func(Updater& updater)
{
    try
    {
        std::cout << "hello" << std::endl;

        auto callback = event::StaticCallback<Updater&>::make_shared(&sub_func);

        updater.on_update += callback; // DEADLOCK!!
        //updater.on_update.unhook_all(); // CRASH!
    }
    catch (const NodecException& e)
    {
        logging::ErrorStream(__FILE__, __LINE__) << e.what() << std::endl;
    }
    catch (const std::exception& e)
    {
        logging::ErrorStream(__FILE__, __LINE__) << e.what() << std::endl;
    }

}

int main()
{
    std::cout << "log start" << std::endl;

    try
    {

        std::cout << "--- 1 ---" << std::endl;
        Updater updater;

        auto callback = event::StaticCallback<Updater&>::make_shared(&func);

        updater.on_update += callback;

        updater.on_update.invoke(updater);

        std::cout << "--- 2 ---" << std::endl;
        updater.on_update.invoke(updater);
    }
    catch (const NodecException& e)
    {

        std::cout << e.what() << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }


}