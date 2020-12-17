#include <nodec/event.hpp>
#include <nodec/logging.hpp>

using namespace nodec;

class Updater
{
public:
    event::Event<Updater&> on_update;
};

void sub_func(Updater& updater)
{

    logging::info("hey", __FILE__, __LINE__);
}


void func(Updater& updater)
{
    try
    {
        logging::info("hello", __FILE__, __LINE__);

        auto callback = event::StaticCallback<Updater&>::make_shared(&sub_func);

        updater.on_update += callback; // DEADLOCK!!
    }
    catch (const NodecException& e)
    {
        logging::ErrorStream(__FILE__, __LINE__) << e.what() << std::flush;
    }
    catch (const std::exception& e)
    {
        logging::ErrorStream(__FILE__, __LINE__) << e.what() << std::flush;
    }

}

int main()
{
    logging::record_handlers += event::StaticCallback<const logging::LogRecord&>::make_shared(&logging::record_to_stdout_handler);
    logging::info("log start", __FILE__, __LINE__);

    try
    {
        Updater updater;

        auto callback = event::StaticCallback<Updater&>::make_shared(&func);

        updater.on_update += callback;

        updater.on_update.invoke(updater);
    }
    catch (const NodecException& e)
    {
        logging::ErrorStream(__FILE__, __LINE__) << e.what() << std::flush;
    }
    catch (const std::exception& e)
    {
        logging::ErrorStream(__FILE__, __LINE__) << e.what() << std::flush;
    }


}