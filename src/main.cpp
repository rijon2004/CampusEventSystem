#include <iostream>
#include "BookingManager.h"

int main() {
    EventCatalog catalog;
    catalog.addEvent({"E1", "AI & ML Bootcamp", "Auditorium", 50, 0, 500.0});
    catalog.addEvent({"E2", "Git & CI/CD Workshop", "Lab 302", 2, 0, 200.0});

    CsvFileStorage storage("bookings.csv");
    ConsoleNotifier notifier;
    BookingManager manager(catalog, storage, notifier);

    auto student = UserFactory::createUser("student", "2203001", "Arif");
    StudentDiscountPricing studentDiscount;

    int choice;
    while (true) {
        std::cout << "\n=== Campus Event & Room Booking ===\n"
                  << "1. View Events\n2. Book Event (as Student)\n3. Cancel Booking\n0. Exit\nChoice: ";
        std::cin >> choice;
        if (choice == 0) break;

        if (choice == 1) {
            for (const auto& e : catalog.getAllEvents()) {
                std::cout << e.id << " | " << e.title << " (" << e.venue << ") | Seats: "
                          << e.bookedSeats << "/" << e.capacity << " | Base: " << e.basePrice << " BDT\n";
            }
        } else if (choice == 2) {
            std::string eid;
            int cert, kit;
            std::cout << "Enter Event ID (E1/E2): "; std::cin >> eid;
            std::cout << "Add Certificate (+50)? (1=Yes, 0=No): "; std::cin >> cert;
            std::cout << "Add Workshop Kit (+150)? (1=Yes, 0=No): "; std::cin >> kit;
            try {
                auto rec = manager.createBooking(*student, eid, studentDiscount, cert == 1, kit == 1);
                std::cout << "Success! ID: " << rec.bookingId << " | Total: " << rec.totalPaid << " BDT\n";
            } catch (const std::exception& ex) {
                std::cout << "Error: " << ex.what() << "\n";
            }
        } else if (choice == 3) {
            std::string bid;
            std::cout << "Enter Booking ID (e.g., BK-101): "; std::cin >> bid;
            std::cout << (manager.cancelBooking(bid) ? "Cancelled.\n" : "Not found.\n");
        }
    }
    return 0;
}
