#ifndef STORAGE_H
#define STORAGE_H

#include <string>
#include <fstream>
#include <iostream>

struct BookingRecord {
    std::string bookingId;
    std::string userId;
    std::string eventId;
    std::string passDetails;
    double totalPaid;
};

class IStorageRepository {
public:
    virtual ~IStorageRepository() = default;
    virtual bool saveBooking(const BookingRecord& record) = 0;
};

class INotificationService {
public:
    virtual ~INotificationService() = default;
    virtual void sendConfirmation(const std::string& userId, const std::string& message) = 0;
};

// Appends booking records to CSV
class CsvFileStorage : public IStorageRepository {
private:
    std::string filename;
public:
    CsvFileStorage(std::string file = "bookings.csv") : filename(file) {}
    bool saveBooking(const BookingRecord& record) override {
        std::ofstream out(filename, std::ios::app);
        if (!out.is_open()) return false;
        out << record.bookingId << "," << record.userId << ","
            << record.eventId << "," << record.passDetails << ","
            << record.totalPaid << "\n";
        return true;
    }
};

class ConsoleNotifier : public INotificationService {
public:
    void sendConfirmation(const std::string& userId, const std::string& message) override {
        std::cout << "[Notify -> " << userId << "]: " << message << "\n";
    }
};

#endif
