#include "handlers.hpp"

#include <fstream>
#include <iomanip>
#include <iostream>

#include "../utils/protocol.hpp"

void handleLogin(AuctionServerState &state, std::stringstream &buf,
                 SocketAddress &addressFrom) {
  LoginRequest request;
  LoginResponse response;

  try {
    request.deserialize(buf);
    state.cdebug << "[Login] User " << request.userID << " requested to login"
                 << std::endl;

    if (state.usersManager.userExists(request.userID) == 0) {
      state.usersManager.login(request.userID, request.password);
      response.status = LoginResponse::OK;
    } else {
      state.usersManager.registerUser(request.userID, request.password);
      response.status = LoginResponse::REG;
    }

    state.cdebug << "[Login] User " << request.userID
                 << " successfully logged in" << std::endl;
  } catch (InvalidCredentialsException &e) {
    state.cdebug << "User " << request.userID
                 << " tried to login with invalid "
                    "credentials"
                 << std::endl;
    response.status = LoginResponse::NOK;
  } catch (InvalidPacketException &e) {
    state.cdebug << "[Login] Invalid packet received" << std::endl;
    response.status = LoginResponse::ERR;
  } catch (std::exception &e) {
    std::cerr << "[Login] There was an unhandled exception that prevented "
                 "the user from logging in"
              << e.what() << std::endl;

    return;
  }

  send_packet(response, addressFrom.socket,
              (struct sockaddr *)&addressFrom.addr, addressFrom.size);
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