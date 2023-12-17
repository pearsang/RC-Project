#ifndef SERVER_AUCTION_H
#define SERVER_AUCTION_H

#include "../utils/constants.hpp"
#include "../utils/protocol.hpp"
#include "../utils/utils.hpp"
#include "server_user.hpp"

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <random>
#include <string>
#include <mutex>

namespace fs = std::filesystem;

class AuctionManager {
public:
  /**
   * @brief Opens an auction.
   *
   * @param userID  the user ID of the auction owner
   * @param auctionName  the name of the auction
   * @param startValue  the starting value of the auction
   * @param timeActive  the time the auction will be active
   * @param assetFilename  the filename of the asset
   * @param assetFilePath  the path of the asset
   * @return uint32_t the ID of the auction, or Error identifier
   */
  uint32_t openAuction(std::string userID, std::string auctionName,
                       uint32_t startValue, uint32_t timeActive,
                       std::string assetFilename, std::string assetFilePath);

  /**
   * @brief Get the Next Auction ID object
   *
   * @return The next auction ID
   */
  std::string getNextAuctionID();

  /**
   * @brief Lists all auctions.
   *
   * @return A vector of pairs containing the auction ID and the auction status
   */
  std::vector<std::pair<std::string, uint8_t>> listAuctions();

  /**
   * @brief Lists all auctions in which an user has bidded.
   *
   * @param userID the user ID to check
   * @return A vector of pairs containing the auction ID and the auction status
   */
  std::vector<std::pair<std::string, uint8_t>>
  listUserAuctions(std::string userID);

  /**
   * @brief Gets the auction info.
   *
   * @param auctionID the auction ID to get the info
   * @return A string containing the auction info
   */
  std::string getAuctionInfo(std::string auctionID);

  /**
   * @brief Gets the auction bidders.
   *
   * @param auctionID the auction ID to get the asset
   * @return A string containing the auction bidders
   */
  std::vector<std::string> getAuctionBidders(std::string auctionID);

  /**
   * @brief Get the auctions in which an user has bidded
   *
   * @param userID  the user ID to check
   * @return A vector of pairs containing the auction ID and the auction status
   */
  std::vector<std::pair<std::string, uint8_t>>
  getAuctionsBiddedByUser(std::string userID);

  /**
   * @brief Get the Auction Owner
   *
   * @param auctionID  the auction ID to check
   * @return The auction owner
   */
  std::string getAuctionOwner(std::string auctionID);

  /**
   * @brief Closes an auction.
   *
   * @param userID  the user ID of the auction owner
   * @param password  the password of the auction owner
   * @param auctionID  the auction ID to close
   */
  void closeAuction(std::string userID, std::string password,
                    std::string auctionID);

  /**
   * @brief Get the Largest Bid of an auction
   *
   * @param auctionID  the auction ID to check
   * @return The largest bid
   */
  uint32_t getLargestBid(std::string auctionID);

  /**
   * @brief Bid on an auction.
   *
   * @param userID  the user ID of the bidder
   * @param password  the password of the bidder
   * @param auctionID  the auction ID to bid
   * @param bidValue  the value of the bid
   */
  void bidOnAuction(std::string userID, std::string password,
                    std::string auctionID, uint32_t bidValue);

  /**
   * @brief Get the Auction Asset object
   *
   * @param auctionID  the auction ID to check
   * @return A tuple containing the asset filename, asset size and asset path
   */
  std::tuple<std::string, uint32_t, std::string>
  getAuctionAsset(std::string auctionID);

  /**
   * @brief Get the Auction Record object
   *
   * @param auctionID  the auction ID to check
   * @return A tuple containing all the auction relevant information
   */
  std::tuple<
      std::string, std::string, std::string, uint32_t, std::string, uint32_t,
      std::vector<std::tuple<std::string, uint32_t, std::string, uint32_t>>,
      std::pair<std::string, uint32_t>>
  getAuctionRecord(std::string auctionID);

  /**
   * @brief Check if an auction is past its deadline
   *
   * @param auctionID  the auction ID to check
   * @return valid if the auction is still active, invalid otherwise
   */
  int8_t checkAuctionValidity(std::string auctionID);

  /**
   * @brief Create a Close Auction File object
   *
   * @param auctionID  the auction ID to close
   */
  void createCloseAuctionFile(std::string auctionID, bool automaticClosure);

  /**
   * @brief Construct a new Auction Manager object
   *
   */
  AuctionManager() = default;

  /**
   * @brief Destroy the Auction Manager object
   *
   */
  ~AuctionManager() = default;
};

/**
 * @brief Validates the arguments for the open auction command.
 *
 * @param userID the user ID of the auction owner
 * @param password the password of the auction owner
 * @param auctionName the name of the auction
 * @param startValue the starting value of the auction
 * @param timeActive the time the auction will be active
 * @param assetFilename the filename of the asset
 * @param assetSize the size of the asset
 */
void validateOpenAuctionArgs(std::string userID, std::string password,
                             std::string auctionName, uint32_t startValue,
                             uint32_t timeActive, std::string assetFilename,
                             uint32_t assetSize);

/**
 * @brief Exception thrown when there ano no auctions opened yet.
 *
 */
class NoAuctionsException : public std::runtime_error {
public:
  NoAuctionsException()
      : std::runtime_error("No auctions have been opened yet") {}
};

/**
 * @brief Exception thrown when there are no ongoing bids.
 *
 */
class NoOngoingBidsException : public std::runtime_error {
public:
  NoOngoingBidsException()
      : std::runtime_error("The user has not yet bidded") {}
};

/**
 * @brief Exception thrown when the server has reached the maximum number of
 * auctions.
 *
 */
class AuctionsLimitExceededException : public std::runtime_error {
public:
  AuctionsLimitExceededException()
      : std::runtime_error("The server has reached the maximum number of "
                           "auctions") {}
};

/**
 * @brief Exception thrown when the auction owner is not the user.
 *
 */
class IncorrectAuctionOwnerException : public std::runtime_error {
public:
  IncorrectAuctionOwnerException()
      : std::runtime_error("The user is not the owner of the auction") {}
};

/**
 * @brief Exception thrown when the auction is no longer active.
 *
 */
class NonActiveAuctionException : public std::runtime_error {
public:
  NonActiveAuctionException()
      : std::runtime_error("The auction is no longer active") {}
};

/**
 * @brief Exception thrown when the auction doest not exists.
 *
 */
class AuctionNotFoundException : public std::runtime_error {
public:
  AuctionNotFoundException()
      : std::runtime_error("The auction requested does not exist") {}
};

/**
 * @brief Exception thrown when the asset doest not exists.
 *
 */
class AssetNotFoundException : public std::runtime_error {
public:
  AssetNotFoundException()
      : std::runtime_error("The asset requested does not exist") {}
};

/**
 * @brief Exception thrown when a bid is not allowed.
 *
 */
class IllegalBidException : public std::runtime_error {
public:
  IllegalBidException()
      : std::runtime_error("The owner of an auction cannot bid on it") {}
};

/**
 * @brief Exception thrown when the bid is not larger than the current bid.
 *
 */
class BidRefusedException : public std::runtime_error {
public:
  BidRefusedException()
      : std::runtime_error("The bid is not larger then the current bid") {}
};

#endif