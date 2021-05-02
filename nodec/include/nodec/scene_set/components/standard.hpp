#ifndef NODEC__SCENE_SET__COMPONENTS__STANDARD_HPP_
#define NODEC__SCENE_SET__COMPONENTS__STANDARD_HPP_

#include <nodec/scene_set/scene_registry.hpp>
#include <nodec/vector3.hpp>
#include <nodec/quaternion.hpp>
#include <nodec/matrix4x4.hpp>
#include <nodec/exception.hpp>

#include <memory>
#include <string>
#include <unordered_map>


namespace nodec {
namespace scene_set {
namespace components {

struct Name {
    std::string name;
};

struct Hierarchy {
    class HierarchyException : public Exception {
    public:
        using Exception::Exception;
    };

    const std::vector<SceneEntity>& children() { return children_; }
    const SceneEntity& parent() { return parent_; }

    static void append_child(SceneRegistry& registry, const SceneEntity parent, const SceneEntity child) {
        auto& parent_hierarchy = registry.get_component<Hierarchy>(parent);
        auto& child_hierarchy = registry.get_component<Hierarchy>(child);

        if (child_hierarchy.parent_ != entities::null_entity) {
            std::ostringstream oss;
            oss << "The child (entity: " << child << ") already has the parent (entity: " << child_hierarchy.parent_ << ").";
            throw HierarchyException(oss.str(), __FILE__, __LINE__);
        }

        auto grand = parent;
        while (grand != entities::null_entity) {
            if (grand == child) {
                std::ostringstream oss;
                oss << "Circular reference detected. The child (entity: " << child
                    << ") to append was detected in the parents of parent (entity: " << parent << ").";
                throw HierarchyException(oss.str(), __FILE__, __LINE__);
            }
            auto& hier = registry.get_component<Hierarchy>(grand);
            grand = hier.parent_;
        }

        auto pos = std::lower_bound(parent_hierarchy.children_.begin(),
                                    parent_hierarchy.children_.end(),
                                    child);

        if (pos != parent_hierarchy.children_.end() && *pos == child) {
            std::ostringstream oss;
            oss << "The parent (entity: " << parent << ") already has the same child (entity: " << child << ").";
            throw HierarchyException(oss.str(), __FILE__, __LINE__);
        }

        parent_hierarchy.children_.insert(pos, child);
        child_hierarchy.parent_ = parent;
    }

    static void remove_child(SceneRegistry& registry, const SceneEntity& parent, const SceneEntity& child) {
        auto& parent_hierarchy = registry.get_component<Hierarchy>(parent);
        auto& child_hierarchy = registry.get_component<Hierarchy>(child);

        auto pos = std::lower_bound(parent_hierarchy.children_.begin(),
                                    parent_hierarchy.children_.end(),
                                    child);
        if (pos == parent_hierarchy.children_.end() || *pos != child) {
            std::ostringstream oss;
            oss << "The child(entity: " << child << ") is not the child of parent(entity: " << parent<< ")";
            throw HierarchyException(oss.str(), __FILE__, __LINE__);
        }

        parent_hierarchy.children_.erase(pos);
        child_hierarchy.parent_ = entities::null_entity;
    }

private:
    std::vector<SceneEntity> children_;
    SceneEntity parent_{ entities::null_entity };
};

struct Transform {


private:
    Vector3f local_position_{ Vector3f::zero };
    Vector3f local_scale_{ Vector3f::ones };
    Quaternionf local_rotation_{ Quaternionf::identity };

    Matrix4x4f local2world;

    bool dirty_{ true };
};

//
//class SceneObject {
//public:
//    NODEC_SHARED_PTR_DEFINITIONS(SceneObject);
//    NODEC_WEAK_PTR_DEFINITIONS(SceneObject);
//
//
//public:
//    SceneObject(SceneRegistry& registry, const std::string& name);
//
//    ~SceneObject();
//
//public:
//    std::string name;
//
//public:
//    WeakPtr append_child(SharedPtr child);
//    SharedPtr remove_child(SceneObject& child);
//    
//private:
//    WeakPtr parent_;
//    std::vector<SharedPtr> children_;
//    SceneEntity entity_;
//    const SceneRegistry* registry;
//
//    NODEC_DISABLE_COPY(SceneObject);
//};

}
}
}

#endif