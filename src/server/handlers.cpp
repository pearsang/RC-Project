#include "handlers.hpp"

#include <fstream>
#include <iomanip>
#include <iostream>

#include "../utils/utils.hpp"
#include "server_state.hpp"

void handleLogin(AuctionServerState &state, std::stringstream &buf,
                 SocketAddress &addressFrom) {
  std::cout << "Handling login request" << std::endl;

  (void)state;
  (void)buf;
  (void)addressFrom;
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

void handleExit(AuctionServerState &state, std::stringstream &buf,
                SocketAddress &addressFrom) {
  std::cout << "Handling exit request" << std::endl;

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

void handleOpenAuction(AuctionServerState &state, std::stringstream &buf,
                       SocketAddress &addressFrom) {
  std::cout << "Handling open auction request" << std::endl;

  (void)state;
  (void)buf;
  (void)addressFrom;
}

void handleCloseAuction(AuctionServerState &state, std::stringstream &buf,
                        SocketAddress &addressFrom) {
  std::cout << "Handling close auction request" << std::endl;

  (void)state;
  (void)buf;
  (void)addressFrom;
}

void handleShowAsset(AuctionServerState &state, std::stringstream &buf,
                      SocketAddress &addressFrom) {
  std::cout << "Handling show assets request" << std::endl;

  (void)state;
  (void)buf;
  (void)addressFrom;
}

void handleBid(AuctionServerState &state, std::stringstream &buf,
               SocketAddress &addressFrom) {
  std::cout << "Handling bid request" << std::endl;

  (void)state;
  (void)buf;
  (void)addressFrom;
}