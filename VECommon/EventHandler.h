#pragma once
template <typename EventT>
class EventHandler
{
public:
    EventHandler() {
        handlers = {};
    }
    ~EventHandler() {}

    void add(function<void(EventT)> e) {
        handlers.push_back(e);
    }

    void invoke(EventT e) {
        for (int i = 0; i < handlers.size(); i++) handlers[i](e);
    }

private:
    vector<function<void(EventT)>> handlers;
};
