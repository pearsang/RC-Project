#ifndef SERVER_USER_H
#define SERVER_USER_H

#include "../utils/constants.hpp"
#include "../utils/protocol.hpp"
#include "../utils/utils.hpp"
#include <stdexcept>
#include <string>

class UserManager {
public:
  /**
   * @brief Registers an user.
   *
   * @param userID the user ID to register
   * @param password  the password of the user
   */
  void registerUser(std::string userID, std::string password);
  /**
   * @brief Checks if an user exists.
   *
   * @param userID the user ID to check
   * @return VALID if the user exists, INVALID otherwise
   */
  int8_t userExists(std::string userID);

  /**
   * @brief Unregisters an user.
   *
   * @param userID the user ID to unregister
   * @param password the password of the user
   */
  void unregisterUser(std::string userID, std::string password);

  /**
   * @brief Logs in an user.
   *
   * @param userID the user ID to login
   * @param password the password of the user
   */
  void login(std::string userID, std::string password);

  /**
   * @brief Logs out an user.
   *
   * @param userID the user ID to logout
   * @param password the password of the user
   */
  void logout(std::string userID, std::string password);

  /**
   * @brief Checks if an user is logged in.
   *
   * @param userID the user ID to check
   * @return VALID if the user is logged in, INVALID otherwise
   */
  int8_t isUserLoggedIn(std::string userID);

  /**
   * @brief Gets the user password.
   *
   * @param userID the user ID to get the password
   * @return the user password
   */
  std::string getUserPassword(std::string userID);

  /**
   * @brief Constructs a new User Manager object.
   */
  UserManager() = default;

  /**
   * @brief Destroys the User Manager object.
   */
  ~UserManager() = default;
};

/**
 * @brief Exception thrown when invalid credentials are used.
 */
class InvalidCredentialsException : public std::runtime_error {
public:
  InvalidCredentialsException()
      : std::runtime_error("The password does not match that userID's") {}
};

/**
 * @brief Exception thrown when the user is not logged in.
 */
class UserNotLoggedInException : public std::runtime_error {
public:
  UserNotLoggedInException()
      : std::runtime_error("The user is not logged in") {}
};

#endif
