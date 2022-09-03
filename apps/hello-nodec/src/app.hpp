#ifndef APP_HPP_
#define APP_HPP_

#define CEREAL_THREAD_SAFE 1

#define EDITOR_MODE

#include <nodec_engine/nodec_engine.hpp>
#include <nodec_input/input.hpp>
#include <nodec_rendering/components/camera.hpp>
#include <nodec_rendering/components/image_renderer.hpp>
#include <nodec_rendering/components/directional_light.hpp>
#include <nodec_rendering/components/mesh_renderer.hpp>
#include <nodec_rendering/resources/mesh.hpp>
#include <nodec_resources/resources.hpp>
#include <nodec_scene/components/basic.hpp>
#include <nodec_scene/scene.hpp>
#include <nodec_scene_audio/components/audio_listener.hpp>
#include <nodec_scene_audio/components/audio_source.hpp>
#include <nodec_scene_audio/resources/audio_clip.hpp>
#include <nodec_scene_serialization/scene_serialization.hpp>
#include <nodec_scene_serialization/serializable_scene_graph.hpp>
#include <nodec_scene_serialization/scene_entity_emplacer.hpp>
#include <nodec_screen/screen.hpp>

#include <nodec/logging.hpp>
#include <nodec/math/gfx.hpp>

#include <cereal/types/polymorphic.hpp>

#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/archives/portable_binary.hpp>
#include <cereal/archives/xml.hpp>

#ifdef EDITOR_MODE
#    include <imgui.h>
#    include <nodec_scene_editor/scene_editor.hpp>
#endif

#endif
