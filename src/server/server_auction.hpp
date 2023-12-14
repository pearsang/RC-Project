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
                       std::string assetFilename, std::string assetFilePath);
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

  std::string getAuctionOwner(std::string auctionID);

  void closeAuction(std::string userID, std::string password,
                    std::string auctionID);

  void bidOnAuction(std::string userID, std::string password,
                    std::string auctionID, uint32_t bidValue);

  // constructor
  AuctionManager() = default;
  // destructor
  ~AuctionManager() = default;
};

void validateOpenAuctionArgs(std::string userID, std::string password,
                             std::string auctionName, uint32_t startValue,
                             uint32_t timeActive, std::string assetFilename,
                             uint32_t assetSize);

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

class AuctionsLimitExceededException : public std::runtime_error {
public:
  AuctionsLimitExceededException()
      : std::runtime_error("The server has reached the maximum number of "
                           "auctions") {}
};

class IncorrectAuctionOwnerException : public std::runtime_error {
public:
  IncorrectAuctionOwnerException()
      : std::runtime_error("The user is not the owner of the auction") {}
};

class NonActiveAuctionException : public std::runtime_error {
public:
  NonActiveAuctionException()
      : std::runtime_error("The auction is no longer active") {}
};

class AuctionNotFoundException : public std::runtime_error {
public:
  AuctionNotFoundException()
      : std::runtime_error("The auction requested does not exist") {}
};

class IllegalBidException : public std::runtime_error {
public:
  IllegalBidException()
      : std::runtime_error("The owner of an auction cannot bid on it") {}
};

class BidRefusedException : public std::runtime_error {
public:
  BidRefusedException()
      : std::runtime_error("The bid is not larger then the current bid") {}
};

#endif