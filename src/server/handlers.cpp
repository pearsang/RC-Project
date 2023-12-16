#include "handlers.hpp"

#include <fstream>
#include <iomanip>
#include <iostream>

#include "../utils/protocol.hpp"

void handleLogin(AuctionServerState &serverState, std::stringstream &buf,
                 SocketAddress &addressFrom) {
  LoginRequest request;
  LoginResponse response;
  try {
    request.deserialize(buf);
    serverState.verbose << "[Login] User " << request.userID
                        << " requested to login" << std::endl;

    if (serverState.usersManager.userExists(request.userID) == 0) {
      serverState.usersManager.login(request.userID, request.password);
      response.status = LoginResponse::OK;
      serverState.verbose << "[Login] User " << request.userID
                          << " successfully logged in" << std::endl;

    } else {
      serverState.usersManager.registerUser(request.userID, request.password);
      response.status = LoginResponse::REG;
      serverState.verbose << "[Login] User " << request.userID
                          << " successfully registered" << std::endl;
    }
  } catch (InvalidCredentialsException &e) {
    serverState.verbose << "User " << request.userID
                        << " tried to login with invalid "
                           "credentials"
                        << std::endl;
    response.status = LoginResponse::NOK;
  } catch (InvalidPacketException &e) {
    serverState.verbose << "[Login] Invalid packet received" << std::endl;
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

void handleLogout(AuctionServerState &serverState, std::stringstream &buf,
                  SocketAddress &addressFrom) {
  LogoutRequest request;
  LogoutResponse response;

  try {
    request.deserialize(buf);
    serverState.verbose << "[Logout] User " << request.userID
                        << " requested to logout with password "
                        << request.password << std::endl;
    if (serverState.usersManager.userExists(request.userID) == 0) {
      if (serverState.usersManager.isUserLoggedIn(request.userID) == 0) {
        serverState.usersManager.logout(request.userID, request.password);
        response.status = LogoutResponse::OK;
        serverState.verbose << "[Logout] User " << request.userID
                            << " successfully logged out" << std::endl;
      } else {
        response.status = LogoutResponse::NOK;
        serverState.verbose << "[Logout] User " << request.userID
                            << " is not logged in" << std::endl;
      }
    } else {
      response.status = LogoutResponse::UNR;
      serverState.verbose << "[Logout] User " << request.userID
                          << " is not registered" << std::endl;
    }
  } catch (InvalidCredentialsException &e) {
    serverState.verbose << "User " << request.userID
                        << " tried to logout with invalid "
                           "credentials"
                        << std::endl;
    response.status = LogoutResponse::ERR;
  } catch (InvalidPacketException &e) {
    serverState.verbose << "[Logout] Invalid packet received" << std::endl;
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

void handleUnregister(AuctionServerState &serverState, std::stringstream &buf,
                      SocketAddress &addressFrom) {
  UnregisterRequest request;
  UnregisterResponse response;

  try {
    request.deserialize(buf);
    serverState.verbose << "[Unregister] User " << request.userID
                        << " requested to unregister with password "
                        << request.password << std::endl;
    if (serverState.usersManager.userExists(request.userID) == 0) {
      if (serverState.usersManager.isUserLoggedIn(request.userID) == 0) {
        serverState.usersManager.logout(request.userID, request.password);
        serverState.verbose << "[Unregister] User " << request.userID
                            << " successfully logged out" << std::endl;
        serverState.usersManager.unregisterUser(request.userID,
                                                request.password);
        response.status = UnregisterResponse::OK;
        serverState.verbose << "[Unregister] User " << request.userID
                            << " successfully unregistered" << std::endl;
      } else {
        response.status = UnregisterResponse::NOK;
        serverState.verbose << "[Unregister] User " << request.userID
                            << " is not logged in" << std::endl;
      }
    } else {
      response.status = UnregisterResponse::UNR;
      serverState.verbose << "[Unregister] User " << request.userID
                          << " is not registered" << std::endl;
    }
  } catch (InvalidCredentialsException &e) {
    serverState.verbose << "User " << request.userID
                        << " tried to unregister with invalid "
                           "credentials"
                        << std::endl;
    response.status = UnregisterResponse::ERR;
  } catch (InvalidPacketException &e) {
    serverState.verbose << "[Unregister] Invalid packet received" << std::endl;
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

void handleListUserAuctions(AuctionServerState &serverState,
                            std::stringstream &buf,
                            SocketAddress &addressFrom) {
  std::cout << "Handling list user auctions request" << std::endl;
  ListUserAuctionsRequest request;
  ListUserAuctionsResponse response;

  try {
    request.deserialize(buf);
    serverState.verbose << "[ListUserAuctions] User "
                        << " requested to list auctions" << std::endl;

    if (serverState.usersManager.isUserLoggedIn(request.userID) != INVALID) {
      response.auctions =
          serverState.auctionManager.listUserAuctions(request.userID);
      response.status = ListUserAuctionsResponse::OK;
      serverState.verbose << "[ListUserAuctions] Auctions listed successfully"
                          << std::endl;
    } else {
      response.status = ListUserAuctionsResponse::NLG;
      serverState.verbose << "[ListUserAuctions] User " << request.userID
                          << " is not logged in" << std::endl;
    }

  } catch (NoAuctionsException &e) {
    serverState.verbose << "[ListUserAuctions] No auctions to list"
                        << std::endl;
    response.status = ListUserAuctionsResponse::NOK;
  } catch (InvalidPacketException &e) {
    serverState.verbose << "[ListUserAuctions] Invalid packet received"
                        << std::endl;
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

void handleListUserBids(AuctionServerState &serverState, std::stringstream &buf,
                        SocketAddress &addressFrom) {
  std::cout << "Handling list user bids request" << std::endl;
  ListUserBidsRequest request;
  ListUserBidsResponse response;

  try {
    request.deserialize(buf);
    serverState.verbose << "[ListUserBids] User "
                        << " requested to list bids" << std::endl;

    if (serverState.usersManager.isUserLoggedIn(request.userID) != INVALID) {
      response.auctions =
          serverState.auctionManager.getAuctionsBiddedByUser(request.userID);

      response.status = ListUserBidsResponse::OK;
      serverState.verbose << "[ListUserBids] Bids listed successfully"
                          << std::endl;
    } else {
      response.status = ListUserBidsResponse::NLG;
      serverState.verbose << "[ListUserBids] User " << request.userID
                          << " is not logged in" << std::endl;
    }

  } catch (NoOngoingBidsException &e) {
    serverState.verbose << "[ListUserBids] User " << request.userID
                        << " has not bidded on any auction" << std::endl;
    response.status = ListUserBidsResponse::NOK;
  } catch (InvalidPacketException &e) {
    serverState.verbose << "[ListUserBids] Invalid packet received"
                        << std::endl;
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

void handleListAuctions(AuctionServerState &serverState, std::stringstream &buf,
                        SocketAddress &addressFrom) {
  std::cout << "Handling list auctions request" << std::endl;

  ListAuctionsRequest request;
  ListAuctionsResponse response;

  try {
    request.deserialize(buf);
    serverState.verbose << "[ListAuctions] User "
                        << " requested to list auctions" << std::endl;

    response.auctions = serverState.auctionManager.listAuctions();
    response.status = ListAuctionsResponse::OK;
    serverState.verbose << "[ListAuctions] Auctions listed successfully"
                        << std::endl;

  } catch (NoAuctionsException &e) {
    serverState.verbose << "[ListAuctions] No auctions to list" << std::endl;
    response.status = ListAuctionsResponse::NOK;
  } catch (InvalidPacketException &e) {
    serverState.verbose << "[ListAuctions] Invalid packet received"
                        << std::endl;
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

void handleShowRecord(AuctionServerState &serverState, std::stringstream &buf,
                      SocketAddress &addressFrom) {
  std::cout << "Handling show record request" << std::endl;

  ShowRecordRequest request;
  ShowRecordResponse response;

  try {
    request.deserialize(buf);
    serverState.verbose << "[ShowRecord] User "
                        << " requested to show record of auction "
                        << request.auctionID << std::endl;

    std::tuple<
        std::string, std::string, std::string, uint32_t, std::string, uint32_t,
        std::vector<std::tuple<std::string, uint32_t, std::string, uint32_t>>,
        std::pair<std::string, uint32_t>>
        auctionRecords =
            serverState.auctionManager.getAuctionRecord(request.auctionID);
    response.hostUID = std::get<0>(auctionRecords);
    response.auctionName = std::get<1>(auctionRecords);
    response.assetFileName = std::get<2>(auctionRecords);
    response.startValue = std::get<3>(auctionRecords);
    response.startDate = std::get<4>(auctionRecords);
    response.timeActive = std::get<5>(auctionRecords);
    response.bids = std::get<6>(auctionRecords);
    for (auto bid : response.bids) {
      serverState.verbose << "[ShowRecord] Bidsasdasd: " << std::get<0>(bid)
                          << " " << std::get<1>(bid) << " " << std::get<2>(bid)
                          << " " << std::get<3>(bid) << std::endl;
    }

    if (std::get<7>(auctionRecords).first != "") {
      response.end = std::get<7>(auctionRecords);
    }
    response.status = ShowRecordResponse::OK;
    serverState.verbose << "[ShowRecord] Record of auction "
                        << request.auctionID << " shown successfully"
                        << std::endl;
    serverState.verbose << "[ShowRecord] Host UID: " << response.hostUID
                        << std::endl;
    serverState.verbose << "[ShowRecord] Auction name: " << response.auctionName
                        << std::endl;
    serverState.verbose << "[ShowRecord] Asset filename: "
                        << response.assetFileName << std::endl;
    serverState.verbose << "[ShowRecord] Start value: " << response.startValue

                        << std::endl;
    serverState.verbose << "[ShowRecord] Start date: " << response.startDate
                        << std::endl;
    serverState.verbose << "[ShowRecord] Time active: " << response.timeActive
                        << std::endl;
    for (auto bid : response.bids) {
      serverState.verbose << "[ShowRecord] Bid: " << std::get<0>(bid) << " "
                          << std::get<1>(bid) << " " << std::get<2>(bid) << " "
                          << std::get<3>(bid) << std::endl;
    }

    serverState.verbose << "[ShowRecord] End: " << response.end.first << " "
                        << response.end.second << std::endl;
    serverState.verbose << "[ShowRecord] Record shown successfully"
                        << std::endl;

  } catch (AuctionNotFoundException &e) {
    serverState.verbose << "[ShowRecord] Auction " << request.auctionID
                        << " not found" << std::endl;
    response.status = ShowRecordResponse::NOK;
  } catch (InvalidPacketException &e) {
    serverState.verbose << "[ShowRecord] Invalid packet received" << std::endl;
    response.status = ShowRecordResponse::ERR;
  } catch (std::exception &e) {
    std::cerr << "[ShowRecord] There was an unhandled exception that "
                 "prevented the user from showing the record"
              << e.what() << std::endl;
    return;
  }

  send_packet(response, addressFrom.socket,
              (struct sockaddr *)&addressFrom.addr, addressFrom.size);
}

void handleOpenAuction(AuctionServerState &serverState, int fd) {
  OpenAuctionRequest request;
  OpenAuctionResponse response;
  try {
    request.receive(fd);
    validateOpenAuctionArgs(request.userID, request.password,
                            request.auctionName, request.startValue,
                            request.timeActive, request.assetFileName,
                            request.assetSize);
    serverState.verbose << "[OpenAuction] User " << request.userID
                        << " requested to open an auction" << std::endl;

    if (serverState.usersManager.isUserLoggedIn(request.userID) == 0) {

      uint32_t auctionID = serverState.auctionManager.openAuction(
          request.userID, request.auctionName, request.startValue,
          request.timeActive, request.assetFileName, request.assetPath);

      response.status = OpenAuctionResponse::OK;
      response.auctionID = intToStringWithZeros((int)auctionID);

      serverState.verbose << "[OpenAuction] Auction "
                          << " successfully opened" << std::endl;
    } else {
      response.status = OpenAuctionResponse::NLG;

      serverState.verbose << "[OpenAuction] User " << request.userID
                          << " is not logged in" << std::endl;
    }
  } catch (InvalidCredentialsException &e) {
    response.status = OpenAuctionResponse::ERR;

    serverState.verbose << "User " << request.userID
                        << " tried to open an auction with invalid "
                           "credentials"
                        << std::endl;

  } catch (AuctionsLimitExceededException &e) {
    response.status = OpenAuctionResponse::NOK;

    response.status = OpenAuctionResponse::NOK;
    serverState.verbose << "[OpenAuction] Auction "
                        << " failed to open" << std::endl;

  } catch (InvalidPacketException &e) {
    response.status = OpenAuctionResponse::ERR;
    serverState.verbose << "[OpenAuction] Invalid packet received" << std::endl;

  } catch (std::exception &e) {
    std::cerr
        << "[OpenAuction] There was an unhandled exception that prevented "
           "the user from opening an auction"
        << e.what() << std::endl;
    return;
  }
  response.send(fd);
}

void handleCloseAuction(AuctionServerState &serverState, int fd) {
  std::cout << "Handling close auction request" << std::endl;

  CloseAuctionRequest request;
  CloseAuctionResponse response;

  try {
    request.receive(fd);
    serverState.verbose << "[CloseAuction] User " << request.userID
                        << " requested to close an auction" << std::endl;

    serverState.auctionManager.closeAuction(request.userID, request.password,
                                            request.auctionID);

    response.status = CloseAuctionResponse::OK;

    serverState.verbose << "[CloseAuction] Auction "
                        << " successfully closed" << std::endl;
  } catch (UserNotLoggedInException &e) {
    response.status = CloseAuctionResponse::NLG;

    serverState.verbose << "[CloseAuction] User " << request.userID
                        << " is not logged in" << std::endl;

  } catch (InvalidCredentialsException &e) {
    response.status = CloseAuctionResponse::NOK;

    serverState.verbose << "User " << request.userID
                        << " tried to close an auction with invalid "
                           "credentials or it does not exist"
                        << std::endl;
  } catch (AuctionNotFoundException &e) {
    response.status = CloseAuctionResponse::EAU;

    serverState.verbose << "User " << request.userID
                        << " tried to close an auction "
                           "that does not exist"
                        << std::endl;

  } catch (IncorrectAuctionOwnerException &e) {
    response.status = CloseAuctionResponse::EOW;

    serverState.verbose << "User " << request.userID
                        << " tried to close an auction"
                           " that is owned by another user"
                        << std::endl;
  } catch (NonActiveAuctionException &e) {
    response.status = CloseAuctionResponse::END;

    serverState.verbose << "User " << request.userID
                        << " tried to close an auction"
                           " that is not active"
                        << std::endl;
  } catch (InvalidPacketException &e) {
    response.status = CloseAuctionResponse::ERR;
    serverState.verbose << "[CloseAuction] Invalid packet received"
                        << std::endl;

  } catch (std::exception &e) {
    std::cerr
        << "[CloseAuction] There was an unhandled exception that prevented "
           "the user from closing an auction"
        << e.what() << std::endl;
    return;
  }

  response.send(fd);
}

void handleShowAsset(AuctionServerState &serverState, int fd) {
  std::cout << "Handling show assets request" << std::endl;

  ShowAssetRequest request;
  ShowAssetResponse response;
  try {
    request.receive(fd);
    serverState.verbose
        << "[ShowAsset] An user has requested to show an asset of auction "
        << request.auctionID << std::endl;

    std::tuple<std::string, uint32_t, std::string> asset =
        serverState.auctionManager.getAuctionAsset(request.auctionID);
    response.assetFileName = std::get<0>(asset);
    response.assetSize = std::get<1>(asset);
    response.assetPath = std::get<2>(asset);
    response.status = ShowAssetResponse::OK;

    serverState.verbose << "[ShowAsset] Asset of auction " << request.auctionID
                        << " was successfully shown" << std::endl;
  } catch (AuctionNotFoundException &e) {
    response.status = ShowAssetResponse::NOK;

    serverState.verbose << "[ShowAsset] Auction " << request.auctionID
                        << " not found" << std::endl;
  } catch (AssetNotFoundException &e) {
    response.status = ShowAssetResponse::NOK;

    serverState.verbose << "[ShowAsset] Asset of auction " << request.auctionID
                        << " not found" << std::endl;
  } catch (InvalidPacketException &e) {
    response.status = ShowAssetResponse::ERR;
    serverState.verbose << "[ShowAsset] Invalid packet received" << std::endl;

  } catch (
      std::exception &e) { // TODO: catch only the exceptions that can be thrown
    std::cerr << "[ShowAsset] There was an unhandled exception that prevented "
                 "the user from showing an asset"
              << e.what() << std::endl;
    return;
  }

  response.send(fd);
}

void handleBid(AuctionServerState &serverState, int fd) {
  std::cout << "Handling bid request" << std::endl;

  BidRequest request;
  BidResponse response;

  try {
    request.receive(fd);
    serverState.verbose << "[Bid] User " << request.userID
                        << " requested to bid on an auction" << std::endl;

    serverState.auctionManager.bidOnAuction(
        request.userID, request.password, request.auctionID, request.bidValue);

    response.status = BidResponse::ACC;

    serverState.verbose << "[Bid] Bid "
                        << "successfully placed" << std::endl;

  } catch (UserNotLoggedInException &e) {
    response.status = BidResponse::NLG;

    serverState.verbose << "[Bid] User " << request.userID
                        << " is not logged in" << std::endl;

  } catch (NonActiveAuctionException &e) {
    response.status = BidResponse::NOK;

    serverState.verbose << "[Bid] Auction " << request.auctionID
                        << " is no longer active" << std::endl;

  } catch (BidRefusedException &e) {
    response.status = BidResponse::REF;

    serverState.verbose << "[Bid] Bid of " << request.bidValue
                        << " was refused as it was not "
                        << "larger enough" << std::endl;

  } catch (IllegalBidException &e) {

    response.status = BidResponse::ILG;

    serverState.verbose << "[Bid] User " << request.userID
                        << " is the auction owner, hence it cannot bid"
                        << std::endl;

  } catch (InvalidCredentialsException &e) {
    response.status = BidResponse::ERR;

    serverState.verbose << "[Bid] User " << request.userID
                        << " tried to bid with invalid credentials"
                        << std::endl;

  } catch (InvalidPacketException &e) {
    response.status = BidResponse::ERR;
    serverState.verbose << "[Bid] Invalid packet received" << std::endl;

  } catch (std::exception &e) {
    std::cerr << "[Bid] There was an unhandled exception that prevented "
                 "the user from bidding on an auction "
              << e.what() << std::endl;
    return;
  }

  response.send(fd);
}