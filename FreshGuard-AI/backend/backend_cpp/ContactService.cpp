#include "ContactService.h"
#include "Database.h"

ContactService::ContactService(Database& database) : db(database) {}
ContactService::~ContactService() {}

bool ContactService::saveMessage(const std::string& name, const std::string& email,
                                 const std::string& subject, const std::string& message) {
    if (name.empty() || email.empty() || subject.empty() || message.empty()) return false;
    return db.saveContactMessage(name, email, subject, message);
}
