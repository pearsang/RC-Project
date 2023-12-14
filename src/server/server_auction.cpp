#include "server_auction.hpp"
#include "../utils/protocol.hpp"

uint32_t AuctionManager::openAuction(std::string userID,
                                     std::string auctionName,
                                     uint32_t startValue, uint32_t timeActive,
                                     std::string assetFilename) {
  if (validateUserID(userID) == INVALID ||
      validateAuctionName(auctionName) == INVALID ||
      validateAssetFilename(assetFilename) == INVALID) {
    // add more validations !!!! TODO
    throw InvalidPacketException();
  }

  try {
    std::random_device rd;
    std::mt19937 gen(rd());

    // Define the distribution for integers between 0 and 999
    std::uniform_int_distribution<int> distribution(0, 999);

    // Generate a random integer
    int randomInteger = distribution(gen);
    // to sr
    std::string auctionID = std::to_string(randomInteger);
    // std::cout << "Creating auction for user " << userID << std::endl;
    std::string auctionPath = AUCTIONDIR;
    auctionPath += "/" + auctionID;
    create_new_directory(auctionPath);
    std::string start = auctionPath + "/" + "START_" + auctionID + ".txt";
    create_new_file(start);
    std::string start_datetime = getCurrentTimeFormated();
    std::string end_sec_time = getStartFullTime();

    write_to_file(start, userID + " " + auctionName + " " + assetFilename +
                             " " + std::to_string(startValue) + " " +
                             std::to_string(timeActive) + " " + start_datetime +
                             " " + end_sec_time);
    return (uint32_t)std::stoi(auctionID);
  } catch (std::exception &e) {
    return 1;
  }
}

std::vector<std::pair<std::string, uint8_t>> AuctionManager::listAuctions() {

  std::vector<std::pair<std::string, uint8_t>> auctions;
  if (directory_exists(AUCTIONDIR) == INVALID) {
    throw std::exception();
  }
  // get number of auctions in DB
  std::string next_auction_path = AUCTIONDIR;
  next_auction_path += "/next_auction.txt";
  std::string nextAuctionID;

  read_from_file(next_auction_path, nextAuctionID);
  int nAuctions = std::stoi(nextAuctionID) - 1;

  // no auctions in DB
  if (nAuctions == 0) {
    throw NoAuctionsException();
  }

  // get all auctions
  for (int i = 1; i <= nAuctions; i++) {
    std::string auction_dir = AUCTIONDIR;
    auction_dir += "/" + intToStringWithZeros(i);
    std::string auction_end_file =
        auction_dir + "/" + "END_" + intToStringWithZeros(i) + ".txt";

    // if auction directory does not exist - auction count was compromissed
    if (directory_exists(auction_dir) == INVALID) {
      throw std::exception();
    }

    // auction still active - no end file
    if (file_exists(auction_end_file) == INVALID) {

      auctions.push_back(std::make_pair(intToStringWithZeros(i), 1));
    }
    // auction ended
    else {
      auctions.push_back(std::make_pair(intToStringWithZeros(i), 0));
    }
  }

  return auctions;
}

std::string AuctionManager::getNextAuctionID() {
  try {
    std::string nextAuctionID;
    std::string nextAuctionPath = AUCTIONDIR;
    nextAuctionPath += "/next_auction.txt";
    read_from_file(nextAuctionPath, nextAuctionID);

    // update next auction id
    int nextAuctionID_int = std::stoi(nextAuctionID);
    nextAuctionID_int++;
    write_to_file(nextAuctionPath, std::to_string(nextAuctionID_int));

    return nextAuctionID;
  } catch (std::exception &e) {
    throw;
  }
}

std::vector<std::pair<std::string, uint8_t>>
AuctionManager::listUserAuctions(std::string userID) {

  std::vector<std::pair<std::string, uint8_t>> auctions;
  std::vector<std::pair<std::string, uint8_t>> userAuctions;

  try {
    AuctionManager auctionManager;

    auctions = auctionManager.listAuctions();
    for (auto auction : auctions) {
      std::string auctionInfo = auctionManager.getAuctionInfo(auction.first);
      std::string auctionOwner = auctionInfo.substr(0, auctionInfo.find(" "));
      if (auctionOwner == userID) {
        userAuctions.push_back(auction);
      }
    }

    if (userAuctions.size() == 0) {
      throw NoAuctionsException();
    }
    return userAuctions;

  } catch (NoAuctionsException &e) {
    throw;
  } catch (std::exception &e) {
    throw;
  }
}

std::string AuctionManager::getAuctionInfo(std::string auctionID) {
  try {
    std::string auctionPath = AUCTIONDIR;
    auctionPath += "/" + auctionID;
    std::string start = auctionPath + "/" + "START_" + auctionID + ".txt";
    std::string auctionInfo;
    read_from_file(start, auctionInfo);
    return auctionInfo;
  } catch (std::exception &e) {
    throw;
  }
}

std::vector<std::string>
AuctionManager::getAuctionBidders(std::string auctionID) {
  try {
    std::string auctionPath = AUCTIONDIR;
    auctionPath += "/" + auctionID;
    std::string auctionBidsPath = auctionPath + "/BIDS";

    // iterate over all files in BIDS directory
    std::vector<std::string> auctionsBidders;

    int i = 0;
    for (const auto &entry :
         std::filesystem::directory_iterator(auctionBidsPath)) {
      if (entry.is_regular_file() && entry.path().extension() == ".txt") {

        std::string auctionBidFile = entry.path();

        std::string bidder;
        bidder = getFirstWord(auctionBidFile);
        auctionsBidders.push_back(bidder);
      }
      i++;
    }

    if (auctionsBidders.size() == 0) {
      throw NoOngoingBidsException();
    }

    return auctionsBidders;
  } catch (std::exception &e) {
    throw;
  }
}

std::vector<std::pair<std::string, uint8_t>>
AuctionManager::getAuctionsBiddedByUser(std::string userID) {

  std::vector<std::pair<std::string, uint8_t>> auctions;
  std::vector<std::pair<std::string, uint8_t>> auctionsBiddedByUser;
  try {
    AuctionManager auctionManager;

    auctions = auctionManager.listAuctions();
    for (auto auction : auctions) {
      std::vector<std::string> auctionBidders =
          auctionManager.getAuctionBidders(auction.first);
      for (auto bidder : auctionBidders) {
        if (bidder == userID) {
          auctionsBiddedByUser.push_back(auction);
        }
      }
    }

    if (auctionsBiddedByUser.size() == 0) {
      throw NoOngoingBidsException();
    }

    return auctionsBiddedByUser;
  } catch (NoOngoingBidsException &e) {
    throw;
  } catch (NoAuctionsException &e) {
    throw NoOngoingBidsException();
  } catch (std::exception &e) {
    throw;
  }
}