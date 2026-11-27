#ifndef USER_H
#define USER_H

#include <string>
#include <memory>
#include <stdexcept>

class User {
protected:
    std::string id;
    std::string name;
public:
    User(std::string id, std::string name) : id(id), name(name) {}
    virtual ~User() = default;
    std::string getId() const { return id; }
    std::string getName() const { return name; }
    virtual std::string getRole() const = 0;
    virtual int maxBookingsAllowed() const = 0;
};

class StudentUser : public User {
public:
    StudentUser(std::string id, std::string name) : User(id, name) {}
    std::string getRole() const override { return "Student"; }
    int maxBookingsAllowed() const override { return 2; }
};

class OrganizerUser : public User {
public:
    OrganizerUser(std::string id, std::string name) : User(id, name) {}
    std::string getRole() const override { return "Organizer"; }
    int maxBookingsAllowed() const override { return 5; }
};

class UserFactory {
public:
    static std::unique_ptr<User> createUser(const std::string& role, const std::string& id, const std::string& name) {
        if (role == "student") return std::make_unique<StudentUser>(id, name);
        if (role == "organizer") return std::make_unique<OrganizerUser>(id, name);
        throw std::invalid_argument("Unknown user role");
    }
};

#endif
