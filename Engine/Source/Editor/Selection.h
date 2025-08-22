#pragma once

namespace Nova
{
    class Object;
    class Asset;
    class Entity;
    class Scene;

    class Selection
    {
    public:
        static inline Object* SelectedObject = nullptr;
        static inline Entity* SelectedEntity = nullptr;
        static inline Asset* SelectedAsset = nullptr;
        static inline Scene* SelectedScene = nullptr;
    };
}
