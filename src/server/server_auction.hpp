#ifndef SERVER_AUCTION_H
#define SERVER_AUCTION_H

#include <string>

class AuctionManager {
public:
  void openAuction(std::string userID, std::string password,
                   std::string auctionName, uint32_t startValue,
                   uint32_t timeActive, std::string assetFilename,
                   uint32_t assetSize, std::string fileData);

  // constructor
  AuctionManager();

  // destructor
  ~AuctionManager();
};

#endif