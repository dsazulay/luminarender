#pragma once

#include "../entity.h"

class Event
{
public:
    Event() = default;
    virtual const char* type() const = 0;
};

class WindowResizeEvent : public Event
{
public:
    WindowResizeEvent(int width, int height) : m_width(width), m_height(height) {}
    const char* type() const override { return descriptor; }
    int width() const { return m_width; }
    int height() const { return m_height; }

    static constexpr char* descriptor = (char*) "WindowResizeEvent";
private:
    int m_width;
    int m_height;
};

class MouseScrollEvent : public Event
{
public:
    MouseScrollEvent(double xOffset, double yOffset) : m_xOffset(xOffset),
        m_yOffset(yOffset) {}
    const char* type() const override { return descriptor; }
    double xOffset() const { return m_xOffset; }
    double yOffset() const { return m_yOffset; }

    static constexpr char* descriptor = (char*) "MouseScrollEvent";
private:
    double m_xOffset;
    double m_yOffset;
};

class MouseMoveEvent : public Event
{
public:
    MouseMoveEvent(float xPos, float yPos, float lastX, float lastY ) :
        m_xPos(xPos), m_yPos(yPos), m_lastX(lastX), m_lastY(lastY) {}
    const char* type() const override { return descriptor; }
    float xPos()  const { return m_xPos;  }
    float yPos()  const { return m_yPos;  }
    float lastX() const { return m_lastX; }
    float lastY() const { return m_lastY; }

    static constexpr char* descriptor = (char*) "MouseMoveEvent";
private:
    float m_xPos;
    float m_yPos;
    float m_lastX;
    float m_lastY;
};

class ViewportResizeEvent : public Event
{
public:
    ViewportResizeEvent(float width, float height) : m_width(width),
        m_height(height) {}
    const char* type() const override { return descriptor; }
    float width() const { return m_width; }
    float height() const { return m_height; }

    static constexpr char* descriptor = (char*) "ViewportResizeEvent";
private:
    float m_width;
    float m_height;
};

class KeyPressEvent : public Event
{
public:
    KeyPressEvent(int keyCode, int modifier) : m_keyCode(keyCode),
        m_modifier(modifier) {}
    const char* type() const override { return descriptor; }
    int keyCode() const { return m_keyCode; }
    int modifier() const { return m_modifier; }

    static constexpr char* descriptor = (char*) "KeyPressEvent";
private:
    int m_keyCode;
    int m_modifier;
};

class KeySinglePressEvent : public Event
{
public:
    KeySinglePressEvent(int keyCode, int modifier) : m_keyCode(keyCode),
        m_modifier(modifier) {}
    const char* type() const override { return descriptor; }
    int keyCode() const { return m_keyCode; }
    int modifier() const { return m_modifier; }

    static constexpr char* descriptor = (char*) "KeySinglePressEvent";
private:
    int m_keyCode;
    int m_modifier;
};

class UiCreateEmptyEvent : public Event
{
public:
    UiCreateEmptyEvent(Entity* entity) : m_entity(entity) {}
    const char* type() const override { return descriptor; }
    Entity* entity() const { return m_entity; }

    static constexpr char* descriptor =  (char*) "UiCreateEmptyEvent";
private:
    Entity* m_entity;
};

class UiToggleSSAOEvent : public Event
{
public:
    UiToggleSSAOEvent(bool enabled) : m_enabled(enabled) {}
    const char* type() const override { return descriptor; }
    bool enabled() const { return m_enabled; }

    static constexpr char* descriptor =  (char*) "UiToggleSSAOEvent";
private:
    bool m_enabled;
};
