//
// Created by Diego Azulay on 21/04/22.
//

#ifndef INTERACTIVEGRAPHICS_DISPATCHER_H
#define INTERACTIVEGRAPHICS_DISPATCHER_H


#include <functional>
#include <unordered_map>
#include <string>
#include <vector>
#include "event.h"

class Dispatcher
{
public:
    Dispatcher(const Dispatcher& other) = delete;
    void operator=(const Dispatcher& other) = delete;

     void subscribe(const char* descriptor, std::function<void(const Event&)>&& func);
     void post(const Event& event);

     static Dispatcher& instance();

private:
    Dispatcher() = default;

    std::unordered_map<std::string, std::vector<std::function<void(const Event&)>>> m_observers;
};



#endif //INTERACTIVEGRAPHICS_DISPATCHER_H
