#include "server_user.hpp"
#include "../utils/protocol.hpp"

int8_t UserManager::isUserLoggedIn(std::string userID) {
  std::string userPath = USERDIR;
  userPath += "/" + userID;
  if (file_exists(userPath + "/" + userID + "_login.txt") != INVALID) {
    return 0;
  }
  return INVALID;
}

void UserManager::login(std::string userID, std::string password) {
  if (validateUserID(userID) == INVALID ||
      validatePassword(password) == INVALID) {
    throw InvalidPacketException();
  }

  try {
    std::string passwordPath = USERDIR;
    passwordPath += "/" + userID + "/" + userID + "_pass.txt";
    std::string validPassword;
    read_from_file(passwordPath, validPassword);
    if (password != validPassword) {
      throw InvalidCredentialsException();
    }
    std::string loginPath = USERDIR;
    loginPath += "/" + userID + "/" + userID + "_login.txt";
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
    std::string userPath = USERDIR;
    std::string passwordPath;
    userPath += "/" + userID;
    passwordPath = userPath;
    passwordPath += "/" + userID + "_pass.txt";

    create_new_directory(userPath);
    create_new_file(passwordPath);
    write_to_file(passwordPath, password);
    login(userID, password);
  } catch (std::exception &e) {
    printf("Error Zé: %s\n", e.what());
    throw;
  }
}

int8_t UserManager::userExists(std::string userID) {
  std::string userPath = USERDIR;
  userPath += "/" + userID;
  userPath += "/" + userID + "_pass.txt";
  return file_exists(userPath);
}
