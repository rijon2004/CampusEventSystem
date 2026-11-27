# Campus Event System - RUET CSE 3206
# Campus Event & Room Booking System

A C++17 event reservation application built for CSE 3206 Software Engineering Sessional.

## Architecture & Design Patterns
- **Factory Method (`src/User.h`)**: Instantiates `StudentUser` and `OrganizerUser` roles with distinct booking quotas.
- **Strategy (`src/Pricing.h`)**: Applies interchangeable pricing rules (`RegularPricing`, `StudentDiscountPricing`).
- **Decorator (`src/Pricing.h`)**: Dynamically adds optional perks (`CertificateDecorator`, `WorkshopKitDecorator`) to event passes.
- **Testing & CI (`tests/system_test.cpp`, `.github/workflows/ci.yml`)**: Unit tested with GoogleTest & GoogleMock and verified via GitHub Actions.
