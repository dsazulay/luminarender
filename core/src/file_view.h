#pragma once

#include <string_view>


template<typename T>
concept Reloadable = requires(T a)
{
    { a.onFileChanged(std::string_view()) };
};

class FileView
{
public:
    template<Reloadable HotReloadable>
    explicit FileView(HotReloadable &hotReloadable)
        : object{ &hotReloadable },
          onFileChangedImpl{
            [](void *obj, std::string_view path)
            {
                return static_cast<HotReloadable *>(obj)->onFileChanged(path);
            }
        }
    {}

    void onFileChanged(std::string_view path) const
    {
        onFileChangedImpl(object, path);
    }

private:
    void *object;
    void (*onFileChangedImpl)(void *, std::string_view);
};
