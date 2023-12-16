#include "server_user.hpp"
#include "../utils/protocol.hpp"

int8_t UserManager::isUserLoggedIn(std::string userID) {
  std::string loginFile =
      USER_DIR + SLASH + userID + SLASH + userID + LOGIN_FILE;

  return file_exists(loginFile) == INVALID ? INVALID : VALID;
}

std::string UserManager::getUserPassword(std::string userID) {
  try {
    std::string validPassword;
    std::string passwordPath =
        USER_DIR + SLASH + userID + SLASH + userID + PASS_FILE;
    read_from_file(passwordPath, validPassword);
    return validPassword;
  } catch (...) {
    throw std::exception();
  }
}

void UserManager::login(std::string userID, std::string password) {
  if (validateUserID(userID) == INVALID ||
      validatePassword(password) == INVALID) {
    throw InvalidPacketException();
  }

  try {
    if (password != getUserPassword(userID)) { // check if password is valid
      throw InvalidCredentialsException();
    }

    std::string loginPath =
        USER_DIR + SLASH + userID + SLASH + userID + LOGIN_FILE;
    create_new_file(loginPath);
  } catch (std::exception &e) {
    throw;
  }
}

void UserManager::registerUser(std::string userID, std::string password) {
  if (validateUserID(userID) == INVALID ||
      validatePassword(password) == INVALID) {
    throw InvalidPacketException();
  }

  try {
    std::string userPath = USER_DIR + SLASH + userID;
    create_new_directory(userPath);

    std::string passwordPath = userPath + SLASH + userID + PASS_FILE;
    create_new_file(passwordPath);
    write_to_file(passwordPath, password);
    login(userID, password);
  } catch (std::exception &e) {
    throw;
  }
}

int8_t UserManager::userExists(std::string userID) {
  std::string userPath = USER_DIR + SLASH + userID + SLASH + userID + PASS_FILE;
  return file_exists(userPath) == INVALID ? INVALID : VALID;
}

void UserManager::logout(std::string userID, std::string password) {
  if (validateUserID(userID) == INVALID ||
      validatePassword(password) == INVALID) {
    throw InvalidPacketException();
  }

  if (password != getUserPassword(userID)) {
    throw InvalidCredentialsException();
  }

  try {
    std::string loginPath =
        USER_DIR + SLASH + userID + SLASH + userID + LOGIN_FILE;
    delete_file(loginPath);
  } catch (std::exception &e) {
    throw;
  }
}

void UserManager::unregisterUser(std::string userID, std::string password) {
  if (validateUserID(userID) == INVALID ||
      validatePassword(password) == INVALID) {
    throw InvalidPacketException();
  }

  if (password != getUserPassword(userID)) {
    throw InvalidCredentialsException();
  }

  try {
    std::string userPath = USER_DIR + SLASH + userID;
    std::string loginPath = userPath + SLASH + userID + LOGIN_FILE;
    delete_file(loginPath);

    std::string passwordPath = userPath + SLASH + userID + PASS_FILE;
    delete_file(passwordPath);
  } catch (std::exception &e) {
    throw;
  }
}