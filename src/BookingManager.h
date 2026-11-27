#ifndef BOOKING_MANAGER_H
#define BOOKING_MANAGER_H

#include "User.h"
#include "EventCatalog.h"
#include "Pricing.h"
#include "Storage.h"
#include <vector>
#include <stdexcept>

class BookingManager {
private:
    EventCatalog& catalog;
    IStorageRepository& storage;
    INotificationService& notifier;
    std::vector<BookingRecord> activeBookings;

    int countUserBookings(const std::string& userId) const {
        int count = 0;
        for (const auto& b : activeBookings) {
            if (b.userId == userId) count++;
        }
        return count;
    }

public:
    BookingManager(EventCatalog& cat, IStorageRepository& repo, INotificationService& notif)
        : catalog(cat), storage(repo), notifier(notif) {}

    BookingRecord createBooking(const User& user, const std::string& eventId,
                                const IPricingStrategy& pricing, bool addCert, bool addKit) {
        Event* ev = catalog.findEvent(eventId);
        if (!ev) throw std::invalid_argument("Event not found");

        if (countUserBookings(user.getId()) >= user.maxBookingsAllowed()) {
            throw std::runtime_error("User booking limit reached");
        }

        if (!catalog.reserveSeat(eventId)) {
            throw std::runtime_error("Event is full");
        }

        double discountedBase = pricing.calculatePrice(ev->basePrice);
        std::unique_ptr<EventPass> pass = std::make_unique<BasePass>(ev->title, discountedBase);
        if (addCert) pass = std::make_unique<CertificateDecorator>(std::move(pass));
        if (addKit) pass = std::make_unique<WorkshopKitDecorator>(std::move(pass));

        BookingRecord rec{
            "BK-" + std::to_string(activeBookings.size() + 101),
            user.getId(),
            eventId,
            pass->getDescription(),
            pass->getCost()
        };

        if (!storage.saveBooking(rec)) {
            catalog.releaseSeat(eventId);
            throw std::runtime_error("Failed to save booking to storage");
        }

        activeBookings.push_back(rec);
        notifier.sendConfirmation(user.getId(), "Confirmed " + rec.passDetails + " (" + std::to_string((int)rec.totalPaid) + " BDT)");
        return rec;
    }

    bool cancelBooking(const std::string& bookingId) {
        for (auto it = activeBookings.begin(); it != activeBookings.end(); ++it) {
            if (it->bookingId == bookingId) {
                catalog.releaseSeat(it->eventId);
                activeBookings.erase(it);
                return true;
            }
        }
        return false;
    }
};

#endif
