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
