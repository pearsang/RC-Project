#ifndef SERVER_USER_H
#define SERVER_USER_H

#include <string>

#include "server.hpp"

class UserManager {
public:
  void registerUser(std::string userID, std::string password);
  int8_t userExists(std::string userID);
  void unregisterUser(std::string userID, std::string password);
  void login(std::string userID, std::string password);
  void logout(std::string userID, std::string password);
};

class InvalidCredentialsException : public std::runtime_error {
public:
  InvalidCredentialsException()
      : std::runtime_error("The password does not match that userID's") {}
};
#endif
