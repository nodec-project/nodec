#include <nodec_modules/game_engine/interfaces/game_engine.hpp>
#include <nodec_modules/game_engine/interfaces/behavior.hpp>

#include <nodec/scene_set/scene_object.hpp>
#include <nodec/logging.hpp>

using namespace nodec;
using namespace nodec_modules::rendering::interfaces;


void HandleException(const std::string& this_name,
                     const std::string& scene_object_name,
                     const char* event_name)
{
    try
    {
        throw; // re-throw current exception.
    }
    catch (const NodecException& e)
    {
        logging::ErrorStream(__FILE__, __LINE__) << "A NodecException has been occured while " << (event_name) << ".\n"
            << "this.name: " << this_name << "\n"
            << "scene_object.name: " << scene_object_name << "\n"
            << "error_message: " << e.what() << std::flush;
    }
    catch (const std::exception& e)
    {

        logging::ErrorStream(__FILE__, __LINE__) << "A StandardException has been occured while " << (event_name) << ".\n"
            << "this.name: " << this_name << "\n"
            << "scene_object.name: " << scene_object_name << "\n"
            << "error_message: " << e.what() << std::flush;
    }
    catch (...)
    {
        logging::ErrorStream(__FILE__, __LINE__) << "A UnknownException has been occured while " << (event_name) << ".\n"
            << "this.name: " << this_name << "\n"
            << "scene_object.name: " << scene_object_name << "\n"
            << "error_message: Unavailable." << std::flush;
    }
}


namespace nodec_modules
{
namespace game_engine
{
namespace interfaces
{

void Behavior::on_frame_start(Rendering& rendering) {}
void Behavior::on_frame_update(Rendering& rendering) {}

void Behavior::on_frame_update_(Rendering& rendering)
{
    if (!is_called_frame_start)
    {
        is_called_frame_start = true;
        try
        {
            on_frame_start(rendering);
        }
        catch (...)
        {
            HandleException(name, scene_object().name, "on_frame_start");
        }
        return;
    }
    try
    {
        on_frame_update(rendering);
    }
    catch (...)
    {
        HandleException(name, scene_object().name, "on_frame_update");
    }
}

void Behavior::enable_frame_update()
{
    if (on_frame_update_callback)
    {
        return;
    }

    on_frame_update_callback = event::MemberCallback<Behavior, Rendering&>::make_shared(
        shared_from(this), &Behavior::on_frame_update_
    );

    get_engine()->rendering().on_frame_update += on_frame_update_callback;

}


} // namespace interfaces
}
}