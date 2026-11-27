#ifndef EVENT_CATALOG_H
#define EVENT_CATALOG_H

#include <string>
#include <vector>
#include <stdexcept>

struct Event {
    std::string id;
    std::string title;
    std::string venue;
    int capacity;
    int bookedSeats;
    double basePrice;
};

class EventCatalog {
private:
    std::vector<Event> events;
public:
    void addEvent(const Event& ev) {
        if (ev.capacity <= 0 || ev.basePrice < 0) {
            throw std::invalid_argument("Invalid capacity or price");
        }
        events.push_back(ev);
    }

    Event* findEvent(const std::string& eventId) {
        for (auto& ev : events) {
            if (ev.id == eventId) return &ev;
        }
        return nullptr;
    }

    bool reserveSeat(const std::string& eventId) {
        Event* ev = findEvent(eventId);
        if (!ev || ev->bookedSeats >= ev->capacity) return false;
        ev->bookedSeats++;
        return true;
    }

    bool releaseSeat(const std::string& eventId) {
        Event* ev = findEvent(eventId);
        if (!ev || ev->bookedSeats <= 0) return false;
        ev->bookedSeats--;
        return true;
    }

    const std::vector<Event>& getAllEvents() const { return events; }
};

#endif
