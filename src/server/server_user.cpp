#include "server_user.hpp"
#include "../utils/protocol.hpp"

int8_t UserManager::isUserLoggedIn(std::string userID) {
  std::string userPath = ASDIR;
  userPath += "/" + userID;
  if (file_exists(userPath + "/" + userID + "_login.txt")) {
    return 0;
  }
  return INVALID;
}

void UserManager::login(std::string userID, std::string password) {
  if (validateUserID(userID) == INVALID ||
      validatePassword(password) == INVALID) {
    throw InvalidPacketException();
  }

  // check if user already logged in
  if (isUserLoggedIn(userID) != INVALID) {
    throw InvalidCredentialsException();
  }

  try {
    std::string loginPath = ASDIR;
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
    std::string userPath = ASDIR;
    std::string passwordPath;
    userPath += "/" + userID;
    passwordPath = userPath;
    userPath += "/" + userID + "_pass.txt";

    create_new_directory(userPath);
    create_new_file(passwordPath);
    write_to_file(passwordPath, password);
    login(userID, password);
  } catch (std::exception &e) {
    throw;
  }
}

int8_t UserManager::userExists(std::string userID) {
  return file_exists(ASDIR + userID + ".txt");
}
