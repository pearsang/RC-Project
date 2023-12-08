#include "handlers.hpp"

#include <fstream>
#include <iomanip>
#include <iostream>

#include "../utils/protocol.hpp"
#include "../utils/utils.hpp"
#include "server_state.hpp"

void handleLogin(AuctionServerState &state, std::stringstream &buf,
                 SocketAddress &addressFrom) {
  std::cout << "Handling login request" << std::endl;

  LoginRequest request;
  LoginResponse response;
  std::string buffer;

  (void)state;
  (void)addressFrom;

  if (directory_exists(ASDIR) == INVALID ||
      directory_exists(USERDIR) == INVALID) {
    std::cout << "Directory does not exist" << std::endl;
    response.status = LoginResponse::ERR;
    return;
  }

  request.deserialize(buf);

  // validate password and userID
  if (validateUserID(request.userID) == INVALID ||
      validatePassword(request.password) == INVALID) {
    std::cout << "Invalid userID or password" << std::endl;
    response.status = LoginResponse::ERR;
  }

  // check if user is registered and create directory if not
  std::string userPath = USERDIR;
  std::string passwordPath;
  userPath += "/" + request.userID;
  passwordPath = userPath + "/" + request.userID + "_pass.txt";

  if (directory_exists(userPath) == INVALID) {
    std::cout << "User is not registered" << std::endl;
    create_new_directory(userPath);
    create_new_file(passwordPath);
    write_to_file(passwordPath, request.password);
    // set response status to REG
    response.status = LoginResponse::REG;
  }

  // check if user is already logged in
  if (file_exists(userPath + "/" + request.userID + "login") != INVALID) {
    std::cout << "User is already logged in" << std::endl;
    response.status = LoginResponse::ERR;
    return;
  }

  read_from_file(passwordPath, buffer);
  // check if password is correct
  if (request.password != buffer) {
    std::cout << "Password is incorrect" << std::endl;
    response.status = LoginResponse::NOK;
  }
}

void handleLogout(AuctionServerState &state, std::stringstream &buf,
                  SocketAddress &addressFrom) {
  std::cout << "Handling logout request" << std::endl;

  (void)state;
  (void)buf;
  (void)addressFrom;
}

void handleUnregister(AuctionServerState &state, std::stringstream &buf,
                      SocketAddress &addressFrom) {
  std::cout << "Handling unregister request" << std::endl;

  (void)state;
  (void)buf;
  (void)addressFrom;
}

void handleListUserAuctions(AuctionServerState &state, std::stringstream &buf,
                            SocketAddress &addressFrom) {
  std::cout << "Handling list user auctions request" << std::endl;

  (void)state;
  (void)buf;
  (void)addressFrom;
}

void handleListUserBids(AuctionServerState &state, std::stringstream &buf,
                        SocketAddress &addressFrom) {
  std::cout << "Handling list user bids request" << std::endl;

  (void)state;
  (void)buf;
  (void)addressFrom;
}

void handleListAuctions(AuctionServerState &state, std::stringstream &buf,
                        SocketAddress &addressFrom) {
  std::cout << "Handling list auctions request" << std::endl;

  (void)state;
  (void)buf;
  (void)addressFrom;
}

void handleShowRecord(AuctionServerState &state, std::stringstream &buf,
                      SocketAddress &addressFrom) {
  std::cout << "Handling show record request" << std::endl;

  (void)state;
  (void)buf;
  (void)addressFrom;
}

void handleOpenAuction(AuctionServerState &state, int fd) {
  std::cout << "Handling open auction request" << std::endl;

  (void)state;
  (void)fd;
}

void handleCloseAuction(AuctionServerState &state, int fd) {
  std::cout << "Handling close auction request" << std::endl;

  (void)state;
  (void)fd;
}

void handleShowAsset(AuctionServerState &state, int fd) {
  std::cout << "Handling show assets request" << std::endl;

  (void)state;
  (void)fd;
}

void handleBid(AuctionServerState &state, int fd) {
  std::cout << "Handling bid request" << std::endl;

  (void)state;
  (void)fd;
}