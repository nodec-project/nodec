#ifndef APP_HPP_
#define APP_HPP_

#define EDITOR_MODE

#include <nodec_animations/animation_curve.hpp>
#include <nodec_engine/nodec_engine.hpp>
#include <nodec_input/input_devices.hpp>
#include <nodec_input/keyboard/keyboard.hpp>
#include <nodec_input/mouse/mouse.hpp>
#include <nodec_physics/components/physics_shape.hpp>
#include <nodec_physics/components/rigid_body.hpp>
#include <nodec_rendering/components/camera.hpp>
#include <nodec_rendering/components/directional_light.hpp>
#include <nodec_rendering/components/image_renderer.hpp>
#include <nodec_rendering/components/mesh_renderer.hpp>
#include <nodec_rendering/components/point_light.hpp>
#include <nodec_rendering/resources/mesh.hpp>
#include <nodec_resources/resources.hpp>
#include <nodec_scene/components/hierarchy.hpp>
#include <nodec_scene/components/name.hpp>
#include <nodec_scene/components/transform.hpp>
#include <nodec_scene/scene.hpp>
#include <nodec_scene_audio/components/audio_listener.hpp>
#include <nodec_scene_audio/components/audio_source.hpp>
#include <nodec_scene_audio/resources/audio_clip.hpp>
#include <nodec_scene_serialization/components/non_serialized.hpp>
#include <nodec_scene_serialization/scene_entity_emplacer.hpp>
#include <nodec_scene_serialization/scene_loader.hpp>
#include <nodec_scene_serialization/scene_serialization.hpp>
#include <nodec_scene_serialization/serializable_scene_graph.hpp>
#include <nodec_screen/screen.hpp>
#include <nodec_world/world.hpp>

#include <nodec/algorithm.hpp>
#include <nodec/logging.hpp>
#include <nodec/math/gfx.hpp>
#include <nodec/optional.hpp>

#include <cereal/types/polymorphic.hpp>

#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/archives/portable_binary.hpp>
#include <cereal/archives/xml.hpp>

#ifdef EDITOR_MODE
#    include <imessentials/list.hpp>
#    include <imessentials/text_buffer.hpp>
#    include <imgui.h>
#    include <nodec_scene_editor/scene_editor.hpp>
#endif

#endif
