#include "server_auction.hpp"
#include "../utils/protocol.hpp"

uint32_t AuctionManager::openAuction(std::string userID,
                                     std::string auctionName,
                                     uint32_t startValue, uint32_t timeActive,
                                     std::string assetFilename,
                                     std::string assetFilePath) {
  // validations of arguments already performed
  // verify this function!!!!!!!
  try {

    std::string auctionID = getNextAuctionID();

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

    // create ASSET directory
    std::string assetPath = auctionPath + "/" + "ASSET" + "/";
    std::string bidPath = auctionPath + "/" + "BIDS" + "/";

    create_new_directory(assetPath);
    create_new_directory(bidPath);
    rename_file(assetFilePath, assetPath + assetFilename);

    std::string assetFilenamePathSubstr =
        assetFilePath.substr(0, assetFilePath.find_first_of("/"));
    delete_directory(assetFilenamePathSubstr);

    return (uint32_t)std::stoi(auctionID);
  } catch (AuctionsLimitExceededException &e) {
    std::string assetFilenamePathSubstr =
        assetFilePath.substr(0, assetFilePath.find_first_of("/"));
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
    std::string nextAuctionPath = AUCTIONDIR;
    nextAuctionPath += "/next_auction.txt";
    read_from_file(nextAuctionPath, nextAuctionID);

    if (nextAuctionID.length() > AUCTION_ID_LENGTH) {
      throw AuctionsLimitExceededException();
    }

    // update next auction id
    int nextAuctionID_int = std::stoi(nextAuctionID);
    nextAuctionID = intToStringWithZeros(nextAuctionID_int);
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

std::string AuctionManager::getAuctionOwner(std::string auctionID) {
  try {
    std::string auctionPath = AUCTIONDIR;
    auctionPath += "/" + auctionID;
    std::string start = auctionPath + "/" + "START_" + auctionID + ".txt";
    std::string auctionInfo;
    read_from_file(start, auctionInfo);
    std::string auctionOwner = auctionInfo.substr(0, auctionInfo.find(" "));
    return auctionOwner;
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

    std::string auctionPath = AUCTIONDIR;
    auctionPath += "/" + auctionID;
    if (directory_exists(auctionPath) == INVALID) {
      throw AuctionNotFoundException();
    }

    if (getAuctionOwner(auctionID) != userID) {
      throw IncorrectAuctionOwnerException();
    }

    std::string end = auctionPath + "/" + "END_" + auctionID + ".txt";
    if (file_exists(end) != INVALID) {
      throw NonActiveAuctionException();
    }

    // close auction
    create_new_file(end);
    std::string end_datetime = getCurrentTimeFormated();
    write_to_file(end, end_datetime);

    return;
  } catch (std::exception &e) {
    throw;
  }
}

uint32_t AuctionManager::getLargestBid(std::string auctionID) {

  std::string auctionPath = AUCTIONDIR;
  auctionPath += "/" + auctionID;
  std::string auctionBidsPath = auctionPath + "/BIDS";
  std::string max_filename;
  uintmax_t max = 0;

  for (const auto &entry : fs::directory_iterator(auctionBidsPath)) {
    if (entry.is_regular_file() && entry.path().extension() == ".txt") {
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
    return 0;
  }

  std::string bidValue = max_filename.substr(0, max_filename.find_last_of("."));
  bidValue = bidValue.substr(bidValue.find_last_of("/") + 1);

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

    std::string auctionPath = AUCTIONDIR;
    auctionPath += "/" + auctionID;
    std::string end = auctionPath + "/" + "END_" + auctionID + ".txt";
    if (file_exists(end) != INVALID) {
      throw NonActiveAuctionException();
    }

    std::string auctionBidsPath = auctionPath + "/BIDS";
    uint32_t currBid = getLargestBid(auctionID);
    if (bidValue <= currBid) {
      throw BidRefusedException();
    }

    std::string owner = getAuctionOwner(auctionID);
    if (owner == userID) {
      throw IllegalBidException();
    }

    // bid can be placed
    std::string bidPath =
        auctionBidsPath + "/" + std::to_string(bidValue) + ".txt";
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
    if (validateAuctionID(auctionID) == INVALID) {
      throw InvalidPacketException();
    }

    std::string auctionPath = AUCTIONDIR;
    auctionPath += "/" + auctionID;
    if (directory_exists(auctionPath) == INVALID) {
      throw AuctionNotFoundException();
    }

    std::string auctionInfo = getAuctionInfo(auctionID);
    // review me please!
    std::string assetFilename = auctionInfo.substr(
        auctionInfo.find_first_of(" ", auctionInfo.find_first_of(" ") + 1) + 1);
    std::string assetPath = auctionPath + "/ASSET/" + assetFilename;
    // keep only till the first space, not included
    assetPath = assetPath.substr(0, assetPath.find_first_of(" "));
    assetFilename = assetFilename.substr(0, assetFilename.find_first_of(" "));
    printf("assetPath: %s\n", assetPath.c_str());

    if (file_exists(assetPath) == INVALID) {
      throw AssetNotFoundException();
    }

    // get asset size
    uint32_t assetSize = getFileSize(assetPath);
    printf("assetSize: %d\n", assetSize);
    printf("assetFilename: %s\n", assetFilename.c_str());

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
    if (validateAuctionID(auctionID) == INVALID) {
      throw InvalidPacketException();
    }

    std::string auctionPath = AUCTIONDIR;
    auctionPath += "/" + auctionID;
    if (directory_exists(auctionPath) == INVALID) {
      throw AuctionNotFoundException();
    }

    std::string auctionOwner;
    std::string auctionName;
    std::string assetFilename;
    uint32_t startValue;
    std::string start_datetime;
    uint32_t timeActive;

    std::string bidder;
    uint32_t bidValue;
    std::string bid_datetime;
    uint32_t bid_sec_time;
    std::tuple<std::string, uint32_t, std::string, uint32_t> bids;

    std::string end_datetime;
    uint32_t end_sec_time;
    std::pair<std::string, uint32_t> auctionEndInfoPair;

    std::string auctionInfo = getAuctionInfo(auctionID);
    std::string word;

    // constructing stream from the string
    std::stringstream start(auctionInfo);

    // declaring vector to store the string after split
    std::vector<std::string> words;
    while (getline(start, word, ' ')) {
      words.push_back(word);
    }

    auctionOwner = words[0];
    auctionName = words[1];
    assetFilename = words[2];
    startValue = (uint32_t)std::stoi(words[3]);
    start_datetime = words[5] + " " + words[6];
    timeActive = (uint32_t)std::stoi(words[4]);

    std::string auctionBidsPath = auctionPath + "/BIDS";
    std::vector<std::tuple<std::string, uint32_t, std::string, uint32_t>>
        auctionBids;
    // UID bid_value bid_datetime bid_sec_time
    for (const auto &entry :
         std::filesystem::directory_iterator(auctionBidsPath)) {
      if (entry.is_regular_file() && entry.path().extension() == ".txt") {
        std::string bidFile = entry.path();
        std::string bidInfo;
        read_from_file(bidFile, bidInfo);
        // split on spaces
        std::stringstream bid(bidInfo);
        std::vector<std::string> bidWords;
        while (getline(bid, word, ' ')) {
          bidWords.push_back(word);
        }

        bidder = bidWords[0];
        bidValue = (uint32_t)std::stoi(bidWords[1]);
        bid_datetime = bidWords[2] + " " + bidWords[3];
        bid_sec_time = (uint32_t)std::stoi(bidWords[4]);
        bids = std::make_tuple(bidder, bidValue, bid_datetime, bid_sec_time);
        auctionBids.push_back(bids);
      }
    }

    // end_datetime end_sec_time
    std::pair<std::string, uint32_t> auctionEnd;
    std::string auctionEndPath = auctionPath + "/END_" + auctionID + ".txt";
    if (file_exists(auctionEndPath) != INVALID) {
      std::string auctionEndInfo;
      read_from_file(auctionEndPath, auctionEndInfo);
      std::stringstream auctionEnd_(auctionEndInfo);
      std::vector<std::string> auctionEndWords;
      while (getline(auctionEnd_, word, ' ')) {
        auctionEndWords.push_back(word);
      }
      end_datetime = auctionEndWords[0] + " " + auctionEndWords[1];
      end_sec_time = (uint32_t)std::stoi(auctionEndWords[1]);

      auctionEndInfoPair = std::make_pair(end_datetime, end_sec_time);
    }

    // sort auctionsBids by bidValue
    std::sort(
        auctionBids.begin(), auctionBids.end(),
        [](const std::tuple<std::string, uint32_t, std::string, uint32_t> &a,
           const std::tuple<std::string, uint32_t, std::string, uint32_t> &b) {
          return std::get<1>(a) > std::get<1>(b);
        });

    if (auctionBids.size() > 50) {
      auctionBids.erase(auctionBids.begin() + 50, auctionBids.end());
    }

    return std::make_tuple(auctionOwner, auctionName, assetFilename, startValue,
                           start_datetime, timeActive, auctionBids,
                           auctionEndInfoPair);

  } catch (std::exception &e) {
    throw;
  }
}
