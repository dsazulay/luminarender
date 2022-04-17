//
// Created by Diego Azulay on 15/04/22.
//

#include <iostream>
#include "entity.h"

unsigned int Entity::id() const
{
    return m_id;
}

Entity::Entity()
{

}

Entity::~Entity()
{

}

void Entity::deleteComponents()
{
//    for (auto component : m_components)
    {
//        delete component;
    }
}




