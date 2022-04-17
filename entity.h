//
// Created by Diego Azulay on 15/04/22.
//

#ifndef INTERACTIVEGRAPHICS_ENTITY_H
#define INTERACTIVEGRAPHICS_ENTITY_H


#include <vector>
#include <typeinfo>
#include "components/icomponent.h"
#include <iostream>
#include <memory>


class Entity
{
public:
    Entity();
    ~Entity();
    void deleteComponents();
    unsigned int id() const;
    template <class T>
    std::shared_ptr<T> getComponent();

    template <class T>
    void addComponent(T& component);

private:
    unsigned int m_id;
    std::vector<std::shared_ptr<IComponent>> m_components;
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

#endif //INTERACTIVEGRAPHICS_ENTITY_H
