#pragma once
#include <functional>
#include <vector>
template <typename EventT>
class EventHandler
{
public:
    EventHandler() {
        handlers = {};
    }
    ~EventHandler() {}

    void add(std::function<void(EventT)> e) {
        handlers.push_back(e);
    }

    void invoke(EventT e) {
        for (int i = 0; i < handlers.size(); i++) handlers[i](e);
    }

private:
    std::vector<std::function<void(EventT)>> handlers;
};
