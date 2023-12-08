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

  (void)state;
  (void)buf;
  (void)addressFrom;

  // check if directory names ASDIR exists
  // if not, create it
  if (!std::filesystem::exists(ASDIR))
    std::filesystem::create_directory(ASDIR);
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