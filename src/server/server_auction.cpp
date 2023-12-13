#include "server_auction.hpp"

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
    std::cout << "Creating auction for user " << userID << std::endl;
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