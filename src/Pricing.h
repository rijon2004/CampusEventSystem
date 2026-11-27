#ifndef PRICING_H
#define PRICING_H

#include <string>
#include <memory>

class IPricingStrategy {
public:
    virtual ~IPricingStrategy() = default;
    virtual double calculatePrice(double basePrice) const = 0;
};

class RegularPricing : public IPricingStrategy {
public:
    double calculatePrice(double basePrice) const override { return basePrice; }
};

class StudentDiscountPricing : public IPricingStrategy {
public:
    double calculatePrice(double basePrice) const override { return basePrice * 0.80; }
};

class EventPass {
public:
    virtual ~EventPass() = default;
    virtual double getCost() const = 0;
    virtual std::string getDescription() const = 0;
};

class BasePass : public EventPass {
private:
    double price;
    std::string title;
public:
    BasePass(std::string title, double price) : title(title), price(price) {}
    double getCost() const override { return price; }
    std::string getDescription() const override { return title + " Pass"; }
};

class PassDecorator : public EventPass {
protected:
    std::unique_ptr<EventPass> wrappedPass;
public:
    PassDecorator(std::unique_ptr<EventPass> pass) : wrappedPass(std::move(pass)) {}
};

class CertificateDecorator : public PassDecorator {
public:
    CertificateDecorator(std::unique_ptr<EventPass> pass) : PassDecorator(std::move(pass)) {}
    double getCost() const override { return wrappedPass->getCost() + 50.0; }
    std::string getDescription() const override { return wrappedPass->getDescription() + " + Printed Certificate"; }
};

class WorkshopKitDecorator : public PassDecorator {
public:
    WorkshopKitDecorator(std::unique_ptr<EventPass> pass) : PassDecorator(std::move(pass)) {}
    double getCost() const override { return wrappedPass->getCost() + 150.0; }
    std::string getDescription() const override { return wrappedPass->getDescription() + " + Workshop Kit"; }
};

#endif
