#include <cstdint>
#include <bitset>
#include <array>
#include <unordered_map>
#include <memory>
#include <cassert>
#include <set>

using Entity = uint32_t;
const Entity MAX_ENTITIES = 1000;

using ComponentType = std::uint8_t;
const ComponentType MAX_COMPONENTS = 8;

using Mask = std::bitset<MAX_COMPONENTS>;

class EntityManager
{
public:
    Entity createEntity()
    {
        assert(m_entityCount < MAX_ENTITIES && "Too many entities in existence");

        Entity id = m_entityCount;
        ++m_entityCount;
        return id;
    }

    void setMask(Entity entity, Mask mask)
    {
        m_componentMasks[entity] = mask;
    }

    Mask getMask(Entity entity)
    {
        return m_componentMasks[entity];
    }

private:
    uint32_t m_entityCount{0};
    std::array<Mask, MAX_ENTITIES> m_componentMasks;
};

class IComponentArray
{
public:
    virtual ~IComponentArray() = default;
};

template<typename T>
class ComponentArray : IComponentArray
{
public:
    void insertData(Entity entity, T component)
    {
        assert(m_entityToIndex.find(entity) == m_entityToIndex.end() && "Component of the same type already added");
        size_t newIndex = m_size;
        m_entityToIndex[entity] = newIndex;
        m_indexToEntity[newIndex] = entity;
        m_size++;
    }

    T& getData(Entity entity)
    {
        assert(m_entityToIndex.find(entity) != m_entityToIndex.end() && "Retrieving non-existing component");
        return m_componentArray[entity];
    }

private:
    std::array<T, MAX_ENTITIES> m_componentArray;
    std::unordered_map<Entity, size_t> m_entityToIndex;
    std::unordered_map<size_t, Entity> m_indexToEntity;
    size_t m_size{0};
};

class ComponentManager
{
public:
    template<typename T>
    void registerComponent()
    {
        const char* typeName = typeid(T).name();
        assert(m_componentTypes.find(typeName) == m_componentTypes.end() && "Component already registered");
        
        m_componentTypes.insert({typeName, m_nextType});
        m_componentArrays.insert({typeName, std::make_shared<ComponentArray<T>>()});
        ++m_nextType;
    }

    template<typename T>
    ComponentType getComponentType()
    {
        const char* typeName = typeid(T).name();
        assert(m_componentTypes.find(typeName) != m_componentTypes.end() && "Component not registered");
        return m_componentTypes[typeName];
    }

    template<typename T>
    void addComponent(Entity entity, T component)
    {
        getComponentArray<T>()->insertData(entity, component);
    }

    template<typename T>
    T& getComponent(Entity entity)
    {
        return getComponentArray<T>()->GetData(entity);
    }

private:
    std::unordered_map<const char*, ComponentType> m_componentTypes;
    std::unordered_map<const char*, std::shared_ptr<IComponentArray>> m_componentArrays;
    ComponentType m_nextType{0};

    template<typename T>
    std::shared_ptr<ComponentArray<T>> getComponentArray()
    {
        const char* typeName = typeid(T).name();
        assert(m_componentTypes.find(typeName) != m_componentTypes.end() && "Component not registered");
        return std::static_pointer_cast<ComponentArray<T>>(m_componentArrays[typeName]);
    }
};

class System
{
public:
    std::set<Entity> m_entities;
};

class SystemManager : System
{
public:
    template<typename T>
    std::shared_ptr<T> registerSystem()
    {
        const char* typeName = typeid(T).name();

        assert(m_systems.find(typeName) == m_systems.end() && "System already registered");

        auto system = std::make_shared<T>();
        m_systems.insert({typeName, system});
        return system;
    }

    template<typename T>
    void setMask(Mask mask)
    {
        const char* typeName = typeid(T).name();
        
        assert(m_systems.find(typeName) != m_systems.end() && "System not registered");

        m_masks.insert({typeName, mask});
    }

    void entityMaskChanged(Entity entity, Mask entityMask)
    {
        for (auto const& pair : m_systems)
        {
            auto const& type = pair.first;
            auto const& system = pair.second;
            auto const& systemMask = m_masks[type];

            if ((entityMask & systemMask) == systemMask)
                system->m_entities.insert(entity);
            else
                system->m_entities.erase(entity);
        }
    }

private:
    std::unordered_map<const char*, Mask> m_masks;
    std::unordered_map<const char*, std::shared_ptr<System>> m_systems;
};

class Coordinator
{
public:
    void init()
    {
        m_enityManager = std::make_unique<EntityManager>();
        m_componentManager = std::make_unique<ComponentManager>();
        m_systemManager = std::make_unique<SystemManager>();
    }

    Entity createEntity()
    {
        return m_enityManager->createEntity();
    }

    template<typename T>
    void registerComponent()
    {
        m_componentManager->registerComponent<T>();
    }

    template<typename T>
    void addComponent(Entity entity, T component)
    {
        m_componentManager->addComponent<T>(entity, component);

        auto mask = m_enityManager->getMask(entity);
        mask.set(m_componentManager->getComponentType<T>(), true);
        m_enityManager->setMask(entity, mask);

        m_systemManager->entityMaskChanged(entity, mask);
    }

    template<typename T>
    T& getComponent(Entity entity)
    {
        return m_componentManager->getComponent<T>(entity);
    }

    template<typename T>
    ComponentType getComponentType()
    {
        return m_componentManager->getComponentType<T>();
    }

    template<typename T>
    std::shared_ptr<T> registerSystem()
    {
        return m_systemManager->registerSystem<T>();
    }

    template<typename T>
    void setSystemMask(Mask mask)
    {
        m_systemManager->setMask<T>(mask);
    }

private:
    std::unique_ptr<EntityManager> m_enityManager;
    std::unique_ptr<ComponentManager> m_componentManager;
    std::unique_ptr<SystemManager> m_systemManager;
};
