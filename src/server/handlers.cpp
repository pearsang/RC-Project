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

  (void)addressFrom;

  ListUserBidsRequest request;
  ListUserBidsResponse response;

  try {
    request.deserialize(buf);
    state.cdebug << "[ListUserBids] User "
                 << " requested to list bids" << std::endl;

    if (state.usersManager.isUserLoggedIn(request.userID) != INVALID) {
      response.auctions =
          state.auctionManager.getAuctionsBiddedByUser(request.userID);

      response.status = ListUserBidsResponse::OK;
      state.cdebug << "[ListUserBids] Bids listed successfully" << std::endl;
    } else {
      response.status = ListUserBidsResponse::NLG;
      state.cdebug << "[ListUserBids] User " << request.userID
                   << " is not logged in" << std::endl;
    }

  } catch (NoOngoingBidsException &e) {
    state.cdebug << "[ListUserBids] User " << request.userID
                 << " has not bidded on any auction" << std::endl;
    response.status = ListUserBidsResponse::NOK;
  } catch (InvalidPacketException &e) {
    state.cdebug << "[ListUserBids] Invalid packet received" << std::endl;
    response.status = ListUserBidsResponse::ERR;
  } catch (std::exception &e) {
    std::cerr << "[ListUserBids] There was an unhandled exception that "
                 "prevented the user from listing bids: "
              << e.what() << std::endl;
    return;
  }

  send_packet(response, addressFrom.socket,
              (struct sockaddr *)&addressFrom.addr, addressFrom.size);
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
    validateOpenAuctionArgs(request.userID, request.password,
                            request.auctionName, request.startValue,
                            request.timeActive, request.assetFilename,
                            request.assetSize);
    state.cdebug << "[OpenAuction] User " << request.userID
                 << " requested to open an auction" << std::endl;

    if (state.usersManager.isUserLoggedIn(request.userID) == 0) {

      uint32_t auctionID = state.auctionManager.openAuction(
          request.userID, request.auctionName, request.startValue,
          request.timeActive, request.assetFilename, request.assetFilePath);

      response.status = OpenAuctionResponse::OK;
      response.auctionID = intToStringWithZeros((int)auctionID);

      state.cdebug << "[OpenAuction] Auction "
                   << " successfully opened" << std::endl;
    } else {
      response.status = OpenAuctionResponse::NLG;

      state.cdebug << "[OpenAuction] User " << request.userID
                   << " is not logged in" << std::endl;
    }
  } catch (InvalidCredentialsException &e) {
    response.status = OpenAuctionResponse::ERR;

    state.cdebug << "User " << request.userID
                 << " tried to open an auction with invalid "
                    "credentials"
                 << std::endl;

  } catch (AuctionsLimitExceededException &e) {
    response.status = OpenAuctionResponse::NOK;

    response.status = OpenAuctionResponse::NOK;
    state.cdebug << "[OpenAuction] Auction "
                 << " failed to open" << std::endl;

  } catch (InvalidPacketException &e) {
    response.status = OpenAuctionResponse::ERR;
    state.cdebug << "[OpenAuction] Invalid packet received" << std::endl;

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

  CloseAuctionRequest request;
  CloseAuctionResponse response;

  try {
    request.receive(fd);
    state.cdebug << "[CloseAuction] User " << request.userID
                 << " requested to close an auction" << std::endl;

    state.auctionManager.closeAuction(request.userID, request.password,
                                      request.auctionID);

    response.status = CloseAuctionResponse::OK;

    state.cdebug << "[CloseAuction] Auction "
                 << " successfully closed" << std::endl;
  } catch (UserNotLoggedInException &e) {
    response.status = CloseAuctionResponse::NLG;

    state.cdebug << "[CloseAuction] User " << request.userID
                 << " is not logged in" << std::endl;

  } catch (InvalidCredentialsException &e) {
    response.status = CloseAuctionResponse::NOK;

    state.cdebug << "User " << request.userID
                 << " tried to close an auction with invalid "
                    "credentials or it does not exist"
                 << std::endl;
  } catch (AuctionNotFoundException &e) {
    response.status = CloseAuctionResponse::EAU;

    state.cdebug << "User " << request.userID
                 << " tried to close an auction "
                    "that does not exist"
                 << std::endl;

  } catch (IncorrectAuctionOwnerException &e) {
    response.status = CloseAuctionResponse::EOW;

    state.cdebug << "User " << request.userID
                 << " tried to close an auction"
                    " that is owned by another user"
                 << std::endl;
  } catch (NonActiveAuctionException &e) {
    response.status = CloseAuctionResponse::END;

    state.cdebug << "User " << request.userID
                 << " tried to close an auction"
                    " that is not active"
                 << std::endl;
  } catch (InvalidPacketException &e) {
    response.status = CloseAuctionResponse::ERR;
    state.cdebug << "[CloseAuction] Invalid packet received" << std::endl;

  } catch (std::exception &e) {
    std::cerr
        << "[CloseAuction] There was an unhandled exception that prevented "
           "the user from closing an auction"
        << e.what() << std::endl;
    return;
  }

  response.send(fd);
}

void handleShowAsset(AuctionServerState &state, int fd) {
  std::cout << "Handling show assets request" << std::endl;

  (void)state;
  (void)fd;
}

void handleBid(AuctionServerState &state, int fd) {
  std::cout << "Handling bid request" << std::endl;

  BidRequest request;
  BidResponse response;

  try {
    request.receive(fd);
    state.cdebug << "[Bid] User " << request.userID
                 << " requested to bid on an auction" << std::endl;

    state.auctionManager.bidOnAuction(request.userID, request.password,
                                      request.auctionID, request.bidValue);

    response.status = BidResponse::ACC;

    state.cdebug << "[Bid] Bid "
                 << "successfully placed" << std::endl;

  } catch (UserNotLoggedInException &e) {
    response.status = BidResponse::NLG;

    state.cdebug << "[Bid] User " << request.userID << " is not logged in"
                 << std::endl;

  } catch (NonActiveAuctionException &e) {
    response.status = BidResponse::NOK;

    state.cdebug << "[Bid] Auction " << request.auctionID
                 << " is no longer active" << std::endl;

  } catch (BidRefusedException &e) {
    response.status = BidResponse::REF;

    state.cdebug << "[Bid] Bid of " << request.bidValue
                 << " was refused as it was not "
                 << "larger enough" << std::endl;

  } catch (IllegalBidException &e) {

    response.status = BidResponse::ILG;

    state.cdebug << "[Bid] User " << request.userID
                 << " is the auction owner, hence it cannot bid" << std::endl;

  } catch (InvalidCredentialsException &e) {
    response.status = BidResponse::ERR;

    state.cdebug << "[Bid] User " << request.userID
                 << " tried to bid with invalid credentials" << std::endl;

  } catch (InvalidPacketException &e) {
    response.status = BidResponse::ERR;
    state.cdebug << "[Bid] Invalid packet received" << std::endl;

  } catch (std::exception &e) {
    std::cerr << "[Bid] There was an unhandled exception that prevented "
                 "the user from bidding on an auction "
              << e.what() << std::endl;
    return;
  }

  response.send(fd);
}