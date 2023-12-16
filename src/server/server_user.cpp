#include "server_user.hpp"
#include "../utils/protocol.hpp"

int8_t UserManager::isUserLoggedIn(std::string userID) {
  std::string userPath = USER_DIR;
  userPath += "/" + userID;
  if (file_exists(userPath + "/" + userID + "_login.txt") != INVALID) {
    return 0;
  }
  return INVALID;
}

std::string UserManager::getUserPassword(std::string userID) {
  std::string passwordPath = USER_DIR;
  std::string validPassword;
  try {
    passwordPath += "/" + userID + "/" + userID + "_pass.txt";
    read_from_file(passwordPath, validPassword);
  } catch (...) {
    throw std::exception();
  }
  return validPassword;
}

void UserManager::login(std::string userID, std::string password) {
  if (validateUserID(userID) == INVALID ||
      validatePassword(password) == INVALID) {
    throw InvalidPacketException();
  }

  try {
    if (password != getUserPassword(userID)) {
      throw InvalidCredentialsException();
    }
    std::string loginPath = USER_DIR;
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
    std::string userPath = USER_DIR;
    std::string passwordPath;
    userPath += "/" + userID;
    passwordPath = userPath;
    passwordPath += "/" + userID + "_pass.txt";

    create_new_directory(userPath);
    create_new_file(passwordPath);
    write_to_file(passwordPath, password);
    login(userID, password);
  } catch (std::exception &e) {
    throw;
  }
}

int8_t UserManager::userExists(std::string userID) {
  std::string userPath = USER_DIR;
  userPath += "/" + userID;
  userPath += "/" + userID + "_pass.txt";
  return file_exists(userPath);
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
    std::string loginPath = USER_DIR;
    loginPath += "/" + userID;
    loginPath += "/" + userID + "_login.txt";
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
    std::string loginPath, passwordPath;
    std::string userPath = USER_DIR;
    userPath += "/" + userID;
    loginPath = userPath;
    passwordPath = userPath;
    loginPath += "/" + userID + "_login.txt";
    passwordPath += "/" + userID + "_pass.txt";
    delete_file(loginPath);
    delete_file(passwordPath);
  } catch (std::exception &e) {
    throw;
  }
}