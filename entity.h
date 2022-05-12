#pragma once

#include "pch.h"
#include "components/icomponent.h"


class Entity
{
public:
    Entity();
    ~Entity();
    void deleteComponents();
    unsigned int id() const;
    std::string name() const;
    void name(std::string entityName);
    template <class T>
    std::shared_ptr<T> getComponent();

    template <class T>
    void addComponent(T& component);

    void addChild(Entity entity);
    std::list<Entity>& getChildren();
    void setParent(Entity* entity);

    void updateSelfAndChild();

private:
    unsigned int m_id;
    std::string m_name;
    std::vector<std::shared_ptr<IComponent>> m_components;

    Entity* m_parent = nullptr;
    std::list<Entity> m_children;
};

template<class T>
void Entity::addComponent(T& component)
{
    m_components.push_back(std::make_shared<T>(component));
}

template<class T>
std::shared_ptr<T> Entity::getComponent()
{
    for (const auto& component : m_components)
    {
        auto& p = *component;
        if (typeid(p) == typeid(T))
        {
            return std::static_pointer_cast<T>(component);
        }
    }

    return nullptr;
}
