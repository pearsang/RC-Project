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
      state.cdebug << "[Login] User " << request.userID
                   << " successfully logged in" << std::endl;

    } else {
      state.usersManager.registerUser(request.userID, request.password);
      response.status = LoginResponse::REG;
      state.cdebug << "[Login] User " << request.userID
                   << " successfully registered" << std::endl;
    }
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
  LogoutRequest request;
  LogoutResponse response;

  try {
    request.deserialize(buf);
    state.cdebug << "[Logout] User " << request.userID
                 << " requested to logout with password " << request.password
                 << std::endl;
    if (state.usersManager.userExists(request.userID) == 0) {
      if (state.usersManager.isUserLoggedIn(request.userID) == 0) {
        state.usersManager.logout(request.userID, request.password);
        response.status = LogoutResponse::OK;
        state.cdebug << "[Logout] User " << request.userID
                     << " successfully logged out" << std::endl;
      } else {
        response.status = LogoutResponse::NOK;
        state.cdebug << "[Logout] User " << request.userID
                     << " is not logged in" << std::endl;
      }
    } else {
      response.status = LogoutResponse::UNR;
      state.cdebug << "[Logout] User " << request.userID << " is not registered"
                   << std::endl;
    }
  } catch (InvalidCredentialsException &e) {
    state.cdebug << "User " << request.userID
                 << " tried to logout with invalid "
                    "credentials"
                 << std::endl;
    response.status = LogoutResponse::ERR;
  } catch (InvalidPacketException &e) {
    state.cdebug << "[Logout] Invalid packet received" << std::endl;
    response.status = LogoutResponse::ERR;
  } catch (std::exception &e) {
    std::cerr << "[Logout] There was an unhandled exception that prevented "
                 "the user from logging out"
              << e.what() << std::endl;
    return;
  }

  send_packet(response, addressFrom.socket,
              (struct sockaddr *)&addressFrom.addr, addressFrom.size);
}

void handleUnregister(AuctionServerState &state, std::stringstream &buf,
                      SocketAddress &addressFrom) {
  UnregisterRequest request;
  UnregisterResponse response;

  try {
    request.deserialize(buf);
    state.cdebug << "[Unregister] User " << request.userID
                 << " requested to unregister with password "
                 << request.password << std::endl;
    if (state.usersManager.userExists(request.userID) == 0) {
      if (state.usersManager.isUserLoggedIn(request.userID) == 0) {
        state.usersManager.logout(request.userID, request.password);
        state.cdebug << "[Unregister] User " << request.userID
                     << " successfully logged out" << std::endl;
        state.usersManager.unregisterUser(request.userID, request.password);
        response.status = UnregisterResponse::OK;
        state.cdebug << "[Unregister] User " << request.userID
                     << " successfully unregistered" << std::endl;
      } else {
        response.status = UnregisterResponse::NOK;
        state.cdebug << "[Unregister] User " << request.userID
                     << " is not logged in" << std::endl;
      }
    } else {
      response.status = UnregisterResponse::UNR;
      state.cdebug << "[Unregister] User " << request.userID
                   << " is not registered" << std::endl;
    }
  } catch (InvalidCredentialsException &e) {
    state.cdebug << "User " << request.userID
                 << " tried to unregister with invalid "
                    "credentials"
                 << std::endl;
    response.status = UnregisterResponse::ERR;
  } catch (InvalidPacketException &e) {
    state.cdebug << "[Unregister] Invalid packet received" << std::endl;
    response.status = UnregisterResponse::ERR;
  } catch (std::exception &e) {
    std::cerr << "[Unregister] There was an unhandled exception that prevented "
                 "the user from unregistering"
              << e.what() << std::endl;
    return;
  }

  send_packet(response, addressFrom.socket,
              (struct sockaddr *)&addressFrom.addr, addressFrom.size);
}

void handleListUserAuctions(AuctionServerState &state, std::stringstream &buf,
                            SocketAddress &addressFrom) {
  std::cout << "Handling list user auctions request" << std::endl;

  (void)state;
  (void)buf;
  (void)addressFrom;

  ListUserAuctionsRequest request;
  ListUserAuctionsResponse response;

  try {
    request.deserialize(buf);
    state.cdebug << "[ListUserAuctions] User "
                 << " requested to list auctions" << std::endl;

    if (state.usersManager.isUserLoggedIn(request.userID) != INVALID) {
      response.auctions = state.auctionManager.listUserAuctions(request.userID);
      response.status = ListUserAuctionsResponse::OK;
      state.cdebug << "[ListUserAuctions] Auctions listed successfully"
                   << std::endl;
    } else {
      response.status = ListUserAuctionsResponse::NLG;
      state.cdebug << "[ListUserAuctions] User " << request.userID
                   << " is not logged in" << std::endl;
    }

  } catch (NoAuctionsException &e) {
    state.cdebug << "[ListUserAuctions] No auctions to list" << std::endl;
    response.status = ListUserAuctionsResponse::NOK;
  } catch (InvalidPacketException &e) {
    state.cdebug << "[ListUserAuctions] Invalid packet received" << std::endl;
    response.status = ListUserAuctionsResponse::ERR;
  } catch (std::exception &e) {
    std::cerr << "[ListUserAuctions] There was an unhandled exception that "
                 "prevented the user from listing auctions"
              << e.what() << std::endl;
    return;
  }

  send_packet(response, addressFrom.socket,
              (struct sockaddr *)&addressFrom.addr, addressFrom.size);
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

  ListAuctionsRequest request;
  ListAuctionsResponse response;

  try {
    request.deserialize(buf);
    state.cdebug << "[ListAuctions] User "
                 << " requested to list auctions" << std::endl;

    response.auctions = state.auctionManager.listAuctions();
    response.status = ListAuctionsResponse::OK;
    state.cdebug << "[ListAuctions] Auctions listed successfully" << std::endl;

  } catch (NoAuctionsException &e) {
    state.cdebug << "[ListAuctions] No auctions to list" << std::endl;
    response.status = ListAuctionsResponse::NOK;
  } catch (InvalidPacketException &e) {
    state.cdebug << "[ListAuctions] Invalid packet received" << std::endl;
    response.status = ListAuctionsResponse::ERR;
  } catch (std::exception &e) {
    std::cerr << "[ListAuctions] There was an unhandled exception that "
                 "prevented the user from listing auctions"
              << e.what() << std::endl;
    return;
  }

  send_packet(response, addressFrom.socket,
              (struct sockaddr *)&addressFrom.addr, addressFrom.size);
}

void handleShowRecord(AuctionServerState &state, std::stringstream &buf,
                      SocketAddress &addressFrom) {
  std::cout << "Handling show record request" << std::endl;

  (void)state;
  (void)buf;
  (void)addressFrom;
}

void handleOpenAuction(AuctionServerState &state, int fd) {
  OpenAuctionRequest request;
  OpenAuctionResponse response;
  try {
    request.receive(fd);
    state.cdebug << "[OpenAuction] User " << request.userID
                 << " requested to open an auction" << std::endl;

    if (state.usersManager.isUserLoggedIn(request.userID) == 0) {
      uint32_t auctionID = state.auctionManager.openAuction(
          request.userID, request.auctionName, request.startValue,
          request.timeActive, request.assetFilename);
      if (auctionID == (uint32_t)INVALID) {
        response.status = OpenAuctionResponse::NOK;
        state.cdebug << "[OpenAuction] Auction "
                     << " failed to open" << std::endl;
      } else {
        response.status = OpenAuctionResponse::OK;
        state.cdebug << "[OpenAuction] Auction "
                     << " successfully opened" << std::endl;
      }
    } else {
      response.status = OpenAuctionResponse::NLG;
      state.cdebug << "[OpenAuction] User " << request.userID
                   << " is not logged in" << std::endl;
    }
  } catch (InvalidCredentialsException &e) {
    state.cdebug << "User " << request.userID
                 << " tried to open an auction with invalid "
                    "credentials"
                 << std::endl;
    response.status = OpenAuctionResponse::ERR;
  } catch (InvalidPacketException &e) {
    state.cdebug << "[OpenAuction] Invalid packet received" << std::endl;
    response.status = OpenAuctionResponse::ERR;
  } catch (std::exception &e) {
    std::cerr
        << "[OpenAuction] There was an unhandled exception that prevented "
           "the user from opening an auction"
        << e.what() << std::endl;
    return;
  }

  response.send(fd);
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