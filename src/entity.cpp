#include "entity.h"

#include "components/transform.h"
#include "log.h"

unsigned int Entity::id() const
{
    return m_id;
}

Entity::Entity() : m_name("Object")
{

}

Entity::~Entity()
{
    LOG_INFO("entity destructor");
}

void Entity::deleteComponents()
{

}

void Entity::addChild(std::unique_ptr<Entity> entity)
{
    entity->m_parent = this;
    auto t = entity->getComponent<Transform>();
    auto pt = getComponent<Transform>();

    t->modelMatrix(glm::inverse(pt->modelMatrix()) * t->modelMatrix());
    t->updateFromModelMatrix();
    t->eulerAngles(t->eulerAngles() - pt->eulerAngles());

    m_children.push_back(std::move(entity));
}

Entity* Entity::getParent()
{
    return m_parent;
}

void Entity::setParent(Entity *entity)
{
    m_parent = entity;
}

void Entity::updateSelfAndChild()
{
    if (m_parent != nullptr)
    {
        auto parentTransform = m_parent->getComponent<Transform>();
        auto transform = getComponent<Transform>();
        transform->updateModelMatrix();
        transform->modelMatrix(parentTransform->modelMatrix() * transform->modelMatrix());
    }

    for (auto& child : m_children)
    {
        child->updateSelfAndChild();
    }
}

std::vector<std::unique_ptr<Entity>>& Entity::getChildren()
{
    return m_children;
}

std::string Entity::name() const
{
    return m_name;
}

void Entity::name(std::string entityName)
{
    m_name = entityName;
}

//Entity::Entity(Entity &e)
//{
//
//}
