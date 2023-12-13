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
  std::string getnextAuctionID();

  // constructor
  AuctionManager() = default;
  // destructor
  ~AuctionManager() = default;
};
#endif