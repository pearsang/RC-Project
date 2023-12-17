#include "server_auction.hpp"
#include "../utils/protocol.hpp"

uint32_t AuctionManager::openAuction(std::string userID,
                                     std::string auctionName,
                                     uint32_t startValue, uint32_t timeActive,
                                     std::string assetFilename,
                                     std::string assetFilePath) {
  try {
    std::string auctionID = getNextAuctionID();

    std::string auctionPath = AUCTION_DIR + SLASH + auctionID;
    create_new_directory(auctionPath);

    std::string start = auctionPath + SLASH + START_FILE + auctionID + TXT_EXT;
    create_new_file(start);

    std::string start_datetime = getCurrentTimeFormated();
    std::string end_sec_time = getStartFullTime();
    write_to_file(start, userID + " " + auctionName + " " + assetFilename +
                             " " + std::to_string(startValue) + " " +
                             std::to_string(timeActive) + " " + start_datetime +
                             " " + end_sec_time);

    std::string assetPath = auctionPath + ASSET_DIR;
    create_new_directory(assetPath);

    std::string bidPath = auctionPath + BID_DIR;
    create_new_directory(bidPath);

    rename_file(assetFilePath, assetPath + assetFilename);

    std::string assetFilenamePathSubstr =
        assetFilePath.substr(0, assetFilePath.find_first_of(SLASH));
    delete_directory(assetFilenamePathSubstr);

    return (uint32_t)std::stoi(auctionID);

  } catch (AuctionsLimitExceededException &e) {
    std::string assetFilenamePathSubstr =
        assetFilePath.substr(0, assetFilePath.find_first_of(SLASH));
    delete_directory(assetFilenamePathSubstr);
    throw;
  } catch (std::exception &e) {
    throw;
  }
  return (uint32_t)INVALID;
}

std::string AuctionManager::getNextAuctionID() {
  try {
    std::string nextAuctionID;
    std::string nextAuctionPath = AUCTION_DIR + SLASH + NEXT_AUCTION_FILE;
    read_from_file(nextAuctionPath, nextAuctionID);

    if (nextAuctionID.length() > AUCTION_ID_LENGTH) {
      throw AuctionsLimitExceededException();
    }

    // Update next auction ID
    int nextAuctionID_int = std::stoi(nextAuctionID);
    nextAuctionID = intToStringWithZeros(nextAuctionID_int, AUCTION_ID_LENGTH);
    nextAuctionID_int++;
    write_to_file(nextAuctionPath, std::to_string(nextAuctionID_int));

    return nextAuctionID;
  } catch (std::exception &e) {
    throw;
  }
}

void validateOpenAuctionArgs(std::string userID, std::string password,
                             std::string auctionName, uint32_t startValue,
                             uint32_t timeActive, std::string assetFilename,
                             uint32_t assetSize) {
  if (validateUserID(userID) == INVALID ||
      validatePassword(password) == INVALID ||
      validateAuctionName(auctionName) == INVALID ||
      validateAssetFilename(assetFilename) == INVALID ||
      validateStartValue(std::to_string(startValue)) == INVALID ||
      validateAuctionDuration(std::to_string(timeActive)) == INVALID ||
      validateAssetFileSize(assetSize) == INVALID) {
    throw InvalidPacketException();
  }
  return;
}

std::vector<std::pair<std::string, uint8_t>> AuctionManager::listAuctions() {
  std::vector<std::pair<std::string, uint8_t>> auctions;
  if (directory_exists(AUCTION_DIR) == INVALID) {
    throw std::exception();
  }

  std::string nextAuctionID;
  std::string next_auction_path = AUCTION_DIR + SLASH + NEXT_AUCTION_FILE;

  // get number of auctions in DB
  read_from_file(next_auction_path, nextAuctionID);
  int numAuctions = std::stoi(nextAuctionID) - 1;

  // no auctions in DB
  if (numAuctions == 0) {
    throw NoAuctionsException();
  }

  // get all auctions
  for (int i = 1; i <= numAuctions; i++) {
    std::string auction_dir =
        AUCTION_DIR + SLASH + intToStringWithZeros(i, AUCTION_ID_LENGTH);
    std::string auction_end_file = auction_dir + SLASH + END_FILE +
                                   intToStringWithZeros(i, AUCTION_ID_LENGTH) +
                                   TXT_EXT;

    // if auction directory does not exist - auction count was compromissed
    if (directory_exists(auction_dir) == INVALID) {
      throw std::exception();
    }

    // auction still active - no end file
    if (file_exists(auction_end_file) == INVALID) {
      if (checkAuctionValidity(intToStringWithZeros(i, AUCTION_ID_LENGTH)) ==
          INVALID) {
        try {
          createCloseAuctionFile(intToStringWithZeros(i, AUCTION_ID_LENGTH),
                                 false);
        } catch (NonActiveAuctionException &e) {
          // if auction was closed by another user, ignore it
        }
        auctions.push_back(
            std::make_pair(intToStringWithZeros(i, AUCTION_ID_LENGTH), 0));
      } else {
        auctions.push_back(
            std::make_pair(intToStringWithZeros(i, AUCTION_ID_LENGTH), 1));
      }
    }
    // auction ended
    else {
      auctions.push_back(
          std::make_pair(intToStringWithZeros(i, AUCTION_ID_LENGTH), 0));
    }
  }
  return auctions;
}

std::vector<std::pair<std::string, uint8_t>>
AuctionManager::listUserAuctions(std::string userID) {
  std::vector<std::pair<std::string, uint8_t>> userAuctions;
  try {
    std::vector<std::pair<std::string, uint8_t>> auctions = listAuctions();

    for (auto auction : auctions) {
      std::string auctionInfo = getAuctionInfo(auction.first);
      std::string auctionOwner = auctionInfo.substr(0, auctionInfo.find(" "));

      if (auctionOwner == userID) { // auction belongs to user
        userAuctions.push_back(auction);
      }
    }

    if (userAuctions.size() == 0) { // user has no auctions
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
    std::string auctionPath = AUCTION_DIR + SLASH + auctionID;
    std::string start = auctionPath + SLASH + START_FILE + auctionID + TXT_EXT;
    std::string auctionInfo;
    read_from_file(start, auctionInfo);
    return auctionInfo;
  } catch (std::exception &e) {
    throw;
  }
}

int8_t AuctionManager::checkAuctionValidity(std::string auctionID) {
  try {
    std::string auctionPath = AUCTION_DIR + SLASH + auctionID;
    std::string auctionInfo = getAuctionInfo(auctionID);
    std::string startTimeSeconds = auctionInfo.substr(
        auctionInfo.find_last_of(" ") + 1, auctionInfo.length());

    std::vector<std::string> words = splitOnSeparator(auctionInfo, ' ');
    std::string timeActive = words[4];

    int currentTimeSeconds = (int)std::time(nullptr);
    if ((std::stoi(startTimeSeconds) + std::stoi(timeActive) -
         currentTimeSeconds) > 0) {
      return 0;
    }
    return INVALID;

  } catch (std::exception &e) {
    throw;
  }
}

std::vector<std::string>
AuctionManager::getAuctionBidders(std::string auctionID) {
  try {
    std::string auctionPath = AUCTION_DIR + SLASH + auctionID;
    std::string auctionBidsPath = auctionPath + BID_DIR;

    // iterate over all files in BIDS directory
    std::vector<std::string> auctionsBidders;

    int i = 0;
    for (const auto &entry :
         std::filesystem::directory_iterator(auctionBidsPath)) {
      if (entry.is_regular_file() && entry.path().extension() == TXT_EXT) {

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

// my bids
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

    // remove duplicate auctions
    std::sort(auctionsBiddedByUser.begin(), auctionsBiddedByUser.end());
    auctionsBiddedByUser.erase(
        std::unique(auctionsBiddedByUser.begin(), auctionsBiddedByUser.end()),
        auctionsBiddedByUser.end());

    return auctionsBiddedByUser;
  } catch (NoOngoingBidsException &e) {
    throw;
  } catch (NoAuctionsException &e) {
    throw NoOngoingBidsException();
  } catch (std::exception &e) {
    throw;
  }
}

std::string AuctionManager::getAuctionOwner(std::string auctionID) {
  try {
    std::string auctionPath = AUCTION_DIR + SLASH + auctionID;
    std::string start = auctionPath + SLASH + START_FILE + auctionID + TXT_EXT;
    std::string auctionInfo;
    read_from_file(start, auctionInfo);
    std::string auctionOwner = auctionInfo.substr(0, auctionInfo.find(" "));
    return auctionOwner;
  } catch (std::exception &e) {
    throw;
  }
}

void AuctionManager::createCloseAuctionFile(std::string auctionID,
                                            bool earlyClosure) {
  try {
    std::string auctionPath = AUCTION_DIR + SLASH + auctionID;
    std::string end = auctionPath + SLASH + END_FILE + auctionID + TXT_EXT;
    if (file_exists(end) != INVALID) {
      throw NonActiveAuctionException();
    }
    // close auction
    create_new_file(end);

    std::string auctionInfo = getAuctionInfo(auctionID);
    std::vector<std::string> words = splitOnSeparator(auctionInfo, ' ');

    // get the word after the last space
    int startTimeSeconds = std::stoi(words[words.size() - 1]);

    // calculate the time has passed
    if (earlyClosure) {
      int end_time = (int)(std::time(nullptr));
      std::string end_datetime = getCurrentTimeFormated();
      int diff = end_time - startTimeSeconds;
      std::string duration = std::to_string(diff);
      write_to_file(end, end_datetime + " " + duration);
    }

    else {
      // calculate the end_datetime
      time_t end_time = startTimeSeconds + std::stoi(words[4]);
      // Format the date and time
      char buffer[20];
      std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S",
                    std::localtime(&end_time));
      std::string end_datetime = buffer;
      std::string timeActive = words[4];
      write_to_file(end, end_datetime + " " + timeActive);
    }

  } catch (std::exception &e) {
    throw;
  }
}

void AuctionManager::closeAuction(std::string userID, std::string password,
                                  std::string auctionID) {
  try {
    UserManager userManager;

    if (validateUserID(userID) == INVALID ||
        validatePassword(password) == INVALID ||
        validateAuctionID(auctionID) == INVALID) {
      throw InvalidPacketException();
    }

    if (userManager.userExists(userID) == INVALID ||
        userManager.getUserPassword(userID) != password) {
      throw InvalidCredentialsException();
    }

    if (userManager.isUserLoggedIn(userID) == INVALID) {
      throw UserNotLoggedInException();
    }

    std::string auctionPath = AUCTION_DIR + SLASH + auctionID;
    if (directory_exists(auctionPath) == INVALID) {
      throw AuctionNotFoundException();
    }

    if (getAuctionOwner(auctionID) != userID) {
      throw IncorrectAuctionOwnerException();
    }

    // check if auction has already expired
    if (checkAuctionValidity(auctionID) == INVALID) {
      createCloseAuctionFile(auctionID, false);
      throw NonActiveAuctionException();
    } else {
      createCloseAuctionFile(auctionID, true);
    }

    return;
  } catch (NonActiveAuctionException &e) {
    throw;
  } catch (std::exception &e) {
    throw;
  }
}

uint32_t AuctionManager::getLargestBid(std::string auctionID) {

  std::string auctionPath = AUCTION_DIR + SLASH + auctionID;
  std::string auctionBidsPath = auctionPath + BID_DIR;
  std::string max_filename;
  uintmax_t max = 0;

  for (const auto &entry : fs::directory_iterator(auctionBidsPath)) {
    if (entry.is_regular_file() && entry.path().extension() == TXT_EXT) {
      std::string filename = entry.path().stem().string();
      try {
        uintmax_t curr = std::stoull(filename);
        if (curr > max) {
          max = curr;
          max_filename = entry.path().string();
        }
      } catch (const std::invalid_argument &) {
        // ignore, filename that is not composed by digits
      }
    }
  }
  // no bids yet
  if (max_filename.empty()) {
    std::string auctionInfo = getAuctionInfo(auctionID);
    std::vector<std::string> words = splitOnSeparator(auctionInfo, ' ');
    return (uint32_t)std::stoi(words[3]);
  }

  std::string bidValue = max_filename.substr(0, max_filename.find_last_of("."));
  bidValue = bidValue.substr(bidValue.find_last_of(SLASH) + 1);

  return (uint32_t)std::stoi(bidValue);
}

void AuctionManager::bidOnAuction(std::string userID, std::string password,
                                  std::string auctionID, uint32_t bidValue) {
  try {
    UserManager userManager;

    if (validateUserID(userID) == INVALID ||
        validatePassword(password) == INVALID ||
        validateAuctionID(auctionID) == INVALID ||
        validateBidValue(std::to_string(bidValue)) == INVALID) {
      throw InvalidPacketException();
    }

    if (userManager.userExists(userID) == INVALID ||
        userManager.getUserPassword(userID) != password) {
      throw InvalidCredentialsException();
    }

    if (userManager.isUserLoggedIn(userID) == INVALID) {
      throw UserNotLoggedInException();
    }

    std::string auctionPath = AUCTION_DIR + SLASH + auctionID;
    std::string end = auctionPath + SLASH + END_FILE + auctionID + TXT_EXT;
    if (file_exists(end) != INVALID) { // auction is closed
      throw NonActiveAuctionException();
    } else if (checkAuctionValidity(auctionID) == INVALID) {
      try {
        createCloseAuctionFile(auctionID, false);
      } catch (NonActiveAuctionException &e) {
        // if auction was closed by another user, ignore it
      }
      throw NonActiveAuctionException();
    }

    if (getAuctionOwner(auctionID) == userID) { // user is auction owner
      throw IllegalBidException();
    }

    if (bidValue <= getLargestBid(auctionID)) { // bid value is too low
      throw BidRefusedException();
    }

    std::string auctionBidsPath = auctionPath + BID_DIR;
    std::string bidPath =
        auctionBidsPath +
        intToStringWithZeros((int)bidValue, BID_VALLUE_LENGTH) + TXT_EXT;
    create_new_file(bidPath);

    std::string bidDateTime = getCurrentTimeFormated();
    std::string bidTimeSeconds = std::to_string(std::time(nullptr));

    std::string auctionInfo = getAuctionInfo(auctionID);
    std::string startTimeSeconds = auctionInfo.substr(
        auctionInfo.find_last_of(" ") + 1, auctionInfo.length());

    write_to_file(bidPath,
                  userID + " " + std::to_string(bidValue) + " " + bidDateTime +
                      " " +
                      getTimeDifferenceStr(startTimeSeconds, bidTimeSeconds));

    return;
  } catch (std::exception &e) {
    throw;
  }
}

std::tuple<std::string, uint32_t, std::string>
AuctionManager::getAuctionAsset(std::string auctionID) {
  try {
    if (validateAuctionID(auctionID) == INVALID) { // check auctionID
      throw InvalidPacketException();
    }

    std::string auctionPath = AUCTION_DIR;
    auctionPath += SLASH + auctionID;
    if (directory_exists(auctionPath) == INVALID) { // check auction exists
      throw AuctionNotFoundException();
    }

    if (checkAuctionValidity(auctionID) == INVALID) { // check auction validity
      try {
        createCloseAuctionFile(auctionID, false);
      } catch (NonActiveAuctionException &e) {
        // if auction was closed by another user, ignore it
      }
    }

    std::string auctionInfo = getAuctionInfo(auctionID);
    // review me please!
    std::string assetFilename = auctionInfo.substr(
        auctionInfo.find_first_of(" ", auctionInfo.find_first_of(" ") + 1) + 1);
    std::string assetPath = auctionPath + ASSET_DIR + assetFilename;
    // keep only till the first space, not included
    assetPath = assetPath.substr(0, assetPath.find_first_of(" "));
    assetFilename = assetFilename.substr(0, assetFilename.find_first_of(" "));

    if (file_exists(assetPath) == INVALID) {
      throw AssetNotFoundException();
    }

    uint32_t assetSize = getFileSize(assetPath);
    return std::make_tuple(assetFilename, assetSize, assetPath);
  } catch (std::exception &e) {
    throw;
  }
}

std::tuple<
    std::string, std::string, std::string, uint32_t, std::string, uint32_t,
    std::vector<std::tuple<std::string, uint32_t, std::string, uint32_t>>,
    std::pair<std::string, uint32_t>>
AuctionManager::getAuctionRecord(std::string auctionID) {
  try {
    if (validateAuctionID(auctionID) == INVALID) { // check auctionID
      throw InvalidPacketException();
    }

    std::string auctionPath = AUCTION_DIR + SLASH + auctionID;
    if (directory_exists(auctionPath) == INVALID) { // check auction exists
      throw AuctionNotFoundException();
    }

    if (checkAuctionValidity(auctionID) == INVALID) { // check auction validity
      try {
        createCloseAuctionFile(auctionID, false);
      } catch (NonActiveAuctionException &e) {
        // if auction was closed, ignore it
      }
    }

    std::string auctionInfo = getAuctionInfo(auctionID);
    std::vector<std::string> words = splitOnSeparator(auctionInfo, ' ');

    std::string auctionOwner = words[0];
    std::string auctionName = words[1];
    std::string assetFilename = words[2];
    uint32_t startValue = (uint32_t)std::stoi(words[3]);
    std::string start_datetime = words[5] + " " + words[6];
    uint32_t timeActive = (uint32_t)std::stoi(words[4]);

    std::vector<std::tuple<std::string, uint32_t, std::string, uint32_t>>
        auctionBids = getAuctionBids(auctionID);

    std::pair<std::string, uint32_t> auctionEnd = getAuctionEndInfo(auctionID);

    sortAuctionBids(auctionBids);   // sort by bidValue
    getTopNumBids(auctionBids, 50); // get top 50 bids

    return std::make_tuple(auctionOwner, auctionName, assetFilename, startValue,
                           start_datetime, timeActive, auctionBids, auctionEnd);
  } catch (std::exception &e) {
    throw;
  }
}
