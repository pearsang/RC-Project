#ifndef SERVER_AUCTION_H
#define SERVER_AUCTION_H

#include "../utils/constants.hpp"
#include "../utils/protocol.hpp"
#include "../utils/utils.hpp"
#include "server_user.hpp"

#include <random>

#include <string>

class AuctionManager {
public:
  uint32_t openAuction(std::string userID, std::string auctionName,
                       uint32_t startValue, uint32_t timeActive,
                       std::string assetFilename);

  std::string getNextAuctionID();

  std::vector<std::pair<std::string, uint8_t>> listAuctions();

  std::vector<std::pair<std::string, uint8_t>>
  listUserAuctions(std::string userID);

  std::string getAuctionInfo(std::string auctionID);

  std::vector<std::string> getAuctionBidders(std::string auctionID);

  std::vector<std::pair<std::string, uint8_t>>
  getAuctionsBiddedByUser(std::string userID);

  /* lists auctions in which an user has bidded*/
  std::vector<std::pair<std::string, uint8_t>> listUserBids(std::string userID);

  // constructor
  AuctionManager() = default;
  // destructor
  ~AuctionManager() = default;
};

class NoAuctionsException : public std::runtime_error {
public:
  NoAuctionsException()
      : std::runtime_error("No auctions have been opened yet") {}
};

class NoOngoingBidsException : public std::runtime_error {
public:
  NoOngoingBidsException()
      : std::runtime_error("The user has not yet bidded") {}
};

#endif