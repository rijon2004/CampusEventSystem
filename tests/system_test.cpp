#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "BookingManager.h"

using ::testing::_;
using ::testing::Return;

class MockStorage : public IStorageRepository {
public:
    MOCK_METHOD(bool, saveBooking, (const BookingRecord& record), (override));
};

class MockNotifier : public INotificationService {
public:
    MOCK_METHOD(void, sendConfirmation, (const std::string& userId, const std::string& message), (override));
};

TEST(UserFactoryTest, CreatesCorrectRolesAndLimits) {
    auto s = UserFactory::createUser("student", "S1", "Nadia");
    auto o = UserFactory::createUser("organizer", "O1", "Dr. Hasan");
    EXPECT_EQ(s->getRole(), "Student");
    EXPECT_EQ(s->maxBookingsAllowed(), 2);
    EXPECT_EQ(o->getRole(), "Organizer");
    EXPECT_EQ(o->maxBookingsAllowed(), 5);
    EXPECT_THROW(UserFactory::createUser("guest", "G1", "Unknown"), std::invalid_argument);
}

TEST(PricingAndDecoratorTest, AppliesDiscountAndPerks) {
    StudentDiscountPricing discount;
    double price = discount.calculatePrice(500.0);
    std::unique_ptr<EventPass> pass = std::make_unique<BasePass>("AI Bootcamp", price);
    pass = std::make_unique<CertificateDecorator>(std::move(pass));
    pass = std::make_unique<WorkshopKitDecorator>(std::move(pass));

    EXPECT_DOUBLE_EQ(pass->getCost(), 600.0);
    EXPECT_EQ(pass->getDescription(), "AI Bootcamp Pass + Printed Certificate + Workshop Kit");
}

TEST(BookingManagerTest, SuccessfulBookingCallsStorageAndNotifier) {
    EventCatalog catalog;
    catalog.addEvent({"E1", "Robotics", "Hall A", 10, 0, 200.0});
    MockStorage mockStorage;
    MockNotifier mockNotifier;

    EXPECT_CALL(mockStorage, saveBooking(_)).WillOnce(Return(true));
    EXPECT_CALL(mockNotifier, sendConfirmation("S1", _)).Times(1);

    BookingManager manager(catalog, mockStorage, mockNotifier);
    auto student = UserFactory::createUser("student", "S1", "Tanvir");
    RegularPricing regular;

    auto rec = manager.createBooking(*student, "E1", regular, true, false);
    EXPECT_EQ(rec.bookingId, "BK-101");
    EXPECT_DOUBLE_EQ(rec.totalPaid, 250.0);
    EXPECT_EQ(catalog.findEvent("E1")->bookedSeats, 1);
}

TEST(BookingManagerTest, RollsBackSeatWhenStorageFails) {
    EventCatalog catalog;
    catalog.addEvent({"E1", "Robotics", "Hall A", 10, 0, 200.0});
    MockStorage mockStorage;
    MockNotifier mockNotifier;

    EXPECT_CALL(mockStorage, saveBooking(_)).WillOnce(Return(false));
    EXPECT_CALL(mockNotifier, sendConfirmation(_, _)).Times(0);

    BookingManager manager(catalog, mockStorage, mockNotifier);
    auto student = UserFactory::createUser("student", "S1", "Tanvir");
    RegularPricing regular;

    EXPECT_THROW(manager.createBooking(*student, "E1", regular, false, false), std::runtime_error);
    EXPECT_EQ(catalog.findEvent("E1")->bookedSeats, 0);
}

TEST(BookingManagerTest, PreventsOverbookingWhenFull) {
    EventCatalog catalog;
    catalog.addEvent({"E1", "CyberSec", "Lab 1", 1, 0, 100.0});
    MockStorage mockStorage;
    MockNotifier mockNotifier;
    EXPECT_CALL(mockStorage, saveBooking(_)).WillOnce(Return(true));
    EXPECT_CALL(mockNotifier, sendConfirmation(_, _)).Times(1);

    BookingManager manager(catalog, mockStorage, mockNotifier);
    auto s1 = UserFactory::createUser("student", "S1", "A");
    auto s2 = UserFactory::createUser("student", "S2", "B");
    RegularPricing regular;

    manager.createBooking(*s1, "E1", regular, false, false);
    EXPECT_THROW(manager.createBooking(*s2, "E1", regular, false, false), std::runtime_error);
}

TEST(BookingManagerTest, CancelBookingFreesSeat) {
    EventCatalog catalog;
    catalog.addEvent({"E1", "CyberSec", "Lab 1", 1, 0, 100.0});
    MockStorage mockStorage;
    MockNotifier mockNotifier;
    EXPECT_CALL(mockStorage, saveBooking(_)).WillOnce(Return(true));
    EXPECT_CALL(mockNotifier, sendConfirmation(_, _)).Times(1);

    BookingManager manager(catalog, mockStorage, mockNotifier);
    auto s1 = UserFactory::createUser("student", "S1", "A");
    RegularPricing regular;

    auto rec = manager.createBooking(*s1, "E1", regular, false, false);
    EXPECT_TRUE(manager.cancelBooking(rec.bookingId));
    EXPECT_EQ(catalog.findEvent("E1")->bookedSeats, 0);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
