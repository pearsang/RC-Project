#include "utils.hpp"
#include <filesystem>
#include <fstream>

// Flag to indicate whether the application is terminating
bool is_exiting = false;

void validate_port_number(const std::string &port_number) {
  // Ensure that the port number is a valid number
  for (char c : port_number) {
    if (!std::isdigit(static_cast<unsigned char>(c))) {
      throw FatalError("Invalid port: not a number");
    }
  }

  try {
    // Attempt to convert the string to an integer
    int32_t port_number_int = std::stoi(port_number);
    // Ensure that the port number is within the valid range
    if (port_number_int < 1 || port_number_int > 65535) {
      throw std::runtime_error("");
    }
  } catch (...) {
    throw FatalError("Invalid port: it must be a number between 1 and 65535");
  }
}

// check if string has only digits
bool is_digits(const std::string &str) {
  size_t len = str.length();
  for (size_t i = 0; i < len; i++) {
    if (!std::isdigit(str[i])) {
      return false;
    }
  }
  return true;
}

// check if string is alphanumeric
bool is_alphanumeric(const std::string &str) {
  size_t len = str.length();
  for (size_t i = 0; i < len; i++) {
    if (!std::isalnum(str[i])) {
      return false;
    }
  }
  return true;
}

void setup_custom_signal_handlers() {
  struct sigaction signal_action;
  // set the custom signal handler
  signal_action.sa_handler = terminate_signal_handler;
  // clear the signal set, so that no other signals are blocked
  sigemptyset(&signal_action.sa_mask);
  // no special flags
  signal_action.sa_flags = 0;

  if (sigaction(SIGINT, &signal_action, NULL) == -1) {
    throw FatalError("Setting SIGINT signal handler", errno);
  }
  if (sigaction(SIGTERM, &signal_action, NULL) == -1) {
    throw FatalError("Setting SIGTERM signal handler", errno);
  }

  // ignore SIGPIPE
  signal(SIGPIPE, SIG_IGN);
}

void terminate_signal_handler(int sig) {
  // ignore the signal if the application is already shutting down
  (void)sig;
  if (is_exiting) {
    exit(EXIT_SUCCESS);
  }
  is_exiting = true;
}

std::vector<std::string> parse_args(std::string args) {
  // Create an input string stream
  std::istringstream iss(args);

  std::vector<std::string> parameters;

  std::string parameter;
  while (std::getline(iss, parameter, ' ')) {
    parameters.push_back(parameter);
  }

  return parameters;
}

void create_new_directory(const std::string &path) {
  if (!std::filesystem::exists(path)) {
    std::filesystem::create_directory(path);
    return;
  }
  return;
}

void create_new_file(const std::string &path) {
  try {
    if (!std::filesystem::exists(path)) {
      std::ofstream ofs(path);
      ofs.close();
    }
  } catch (...) {
    throw std::exception();
  }

  return;
}

void rename_file(const std::string &oldPath, const std::string &newPath) {
  try {
    if (std::filesystem::exists(oldPath)) {
      std::filesystem::rename(oldPath, newPath);
    }
  } catch (...) {
    throw std::exception();
  }
  return;
}

void delete_file(const std::string &path) {
  try {
    if (std::filesystem::exists(path)) {
      std::filesystem::remove(path);
    }
  } catch (...) {
    throw std::exception();
  }
  return;
}

void delete_directory(const std::string &path) {
  try {
    if (std::filesystem::exists(path)) {
      std::filesystem::remove_all(path);
    }
  } catch (...) {
    throw std::exception();
  }

  return;
}

int8_t directory_exists(const std::string &path) {
  try {
    if (!std::filesystem::exists(path)) {
      return INVALID;
    }
  } catch (...) {
    throw std::exception();
  }
  return 0;
}

int8_t file_exists(const std::string &path) {
  try {
    if (!std::filesystem::exists(path)) {
      return INVALID;
    }
  } catch (...) {
    throw std::exception();
  }
  return 0;
}

void write_to_file(const std::string &path, const std::string &text) {
  try {
    std::ofstream file(path);
    file << text;
    file.close();
  } catch (...) {
    throw std::exception();
  }
}

void read_from_file(const std::string &path, std::string &text) {

  try {
    std::ifstream file(path);
    if (file.is_open()) {
      std::getline(file, text, '\0');
      file.close();
    }
  } catch (...) {
    throw std::exception();
  }
}

int8_t validateUserID(std::string userID) {
  if (!is_digits(userID) || userID.length() != USER_ID_LENGTH) {
    return INVALID;
  }

  uint32_t id = (uint32_t)std::stoi(userID);
  if (id > USER_ID_MAX) {
    return INVALID;
  }

  return 0;
}

int8_t validatePassword(std::string password) {

  if (password.length() != 8 || !is_alphanumeric(password)) {
    return INVALID;
  }

  return 0;
}

int8_t validateAuctionID(std::string auctionID) {
  if (!is_digits(auctionID) || auctionID.length() != AUCTION_ID_LENGTH) {
    return INVALID;
  }

  return 0;
}

int8_t validateBidValue(std::string bidValue) {
  if (!is_digits(bidValue) || bidValue.length() > START_VALUE_MAX) {
    return INVALID;
  }

  return 0;
}

int8_t validateAssetFilename(std::string assetFilename) {
  if (assetFilename.length() > FILENAME_MAX_LENGTH) {
    return INVALID;
  }

  // test if chars are either alphanumeric or special chars
  for (char c : assetFilename) {
    if (!std::isalnum(c) && c != '-' && c != '_' && c != '.') {
      return INVALID;
    }
  }

  // check  if there is an extension
  if (assetFilename[assetFilename.length() - 4] != '.') {
    return INVALID;
  }

  return 0;
}

int8_t validateStartValue(std::string startValue) {
  if (!is_digits(startValue) || startValue.length() > START_VALUE_MAX ||
      std::stoi(startValue) <= 0 || std::stoi(startValue) >= 999999) {
    return INVALID;
  }
  return 0;
}

int8_t validateAuctionDuration(std::string auctionDuration) {
  if (!is_digits(auctionDuration) ||
      auctionDuration.length() > AUCTION_DURATION_MAX) {
    return INVALID;
  }
  return 0;
}

int8_t validateAuctionName(std::string name) {
  if (name.length() > ASSET_NAME_MAX)
    return INVALID;

  return 0;
}

int8_t validateFileSize(std::string file_path) {
  try {
    uint32_t fileSize = (uint32_t)std::filesystem::file_size(file_path);
    if (std::to_string(fileSize).length() > FILESIZE_MAX) {
      return INVALID;
    }
  } catch (...) {
    return INVALID;
  }

  return 0;
}

std::string getUserPassword(std::string userID) {
  std::string passwordPath = USER_DIR;
  std::string validPassword;
  try {
    passwordPath += SLASH + userID + SLASH + userID + PASS_FILE;
    read_from_file(passwordPath, validPassword);
  } catch (...) {
    throw std::exception();
  }
  return validPassword;
}

std::string getCurrentTimeFormated() {
  // Get the current time point
  auto now = std::chrono::system_clock::now();

  // Convert the time point to a time_t object
  std::time_t currentTime = std::chrono::system_clock::to_time_t(now);

  // Convert the time_t object to a time_t struct
  std::tm *timeInfo = std::localtime(&currentTime);

  // Format the date and time
  char buffer[20];
  std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeInfo);

  return std::string(buffer);
}

std::string getStartFullTime() {
  std::time_t currentTimeSeconds = std::time(nullptr);
  std::string timeActiveStr = std::to_string(currentTimeSeconds);
  return timeActiveStr;
}

int8_t validateAssetFileSize(uint32_t assetSize) {
  // convert assetSize to str
  std::stringstream ss;
  ss << assetSize;
  std::string assetSizeStr = ss.str();
  if (assetSizeStr.length() == 10000000) { // 10MB
    return 0;
  }
  if (assetSizeStr.length() > FILESIZE_MAX) {
    return INVALID;
  }
  return 0;
}

std::string intToStringWithZeros(int number, size_t size) {

  std::string nine = "9";

  // Ensure the number is within the valid range
  if (number < 0 || number > std::stoi(nine.append(size - 1, '9'))) {
    throw std::runtime_error("Invalid number");
  }
  // Use stringstream to convert int to string with leading zeros
  std::ostringstream oss;
  oss << std::setw((int)size) << std::setfill('0') << number;

  return oss.str();
}

std::string getFirstWord(std::string path) {
  std::string firstWord;
  read_from_file(path, firstWord);
  firstWord = firstWord.substr(0, firstWord.find(" "));

  // remove the last character if it is a newline
  if (firstWord[firstWord.length() - 1] == '\n') {
    firstWord = firstWord.substr(0, firstWord.length() - 1);
  }
  return firstWord;
}

std::string getTimeDifferenceStr(std::string startTime, std::string endTime) {
  try {
    int start = std::stoi(startTime);
    int end = std::stoi(endTime);
    int diff = end - start;
    std::string diffStr = std::to_string(diff);
    return diffStr;
  } catch (...) {
    throw std::exception();
  }
}

void printListUserAuctionsTable(
    std::vector<std::pair<std::string, uint8_t>> auctions) {
  const int columnWidth = 15;

  // Print the top border
  std::cout << "+" << std::setw(columnWidth) << std::setfill('-') << "+"
            << std::setw(columnWidth) << "+" << std::setfill(' ') << std::endl;

  // Print the header
  std::cout << "|" << std::setw(columnWidth - 1) << std::left << "Auction ID"
            << "|" << std::setw(columnWidth - 1) << std::left << "Status"
            << "|" << std::endl;

  // Print the border between header and data
  std::cout << std::setw(columnWidth) << std::setfill('-') << "+"
            << std::setw(columnWidth) << "+" << std::setfill(' ') << "+"
            << std::endl;

  for (const auto &auction : auctions) {
    std::cout << "|" << std::setw(columnWidth - 1) << std::left << auction.first
              << "|" << std::setw(columnWidth - 1) << std::left
              << (auction.second ? "Active" : "Not Active") << "|" << std::endl;

    // Print the border between rows
    std::cout << std::setw(columnWidth) << std::setfill('-') << "+"
              << std::setw(columnWidth) << "+" << std::setfill(' ') << "+"
              << std::endl;
  }
}

void printListUserBidsTable(
    std::vector<std::pair<std::string, uint8_t>> auctions) {
  const int columnWidth = 15;

  // Print the top border
  std::cout << "+" << std::setw(columnWidth) << std::setfill('-') << "+"
            << std::setw(columnWidth) << "+" << std::setfill(' ') << std::endl;

  // Print the header
  std::cout << "|" << std::setw(columnWidth - 1) << std::left << "Auction ID"
            << "|" << std::setw(columnWidth - 1) << std::left << "Status"
            << "|" << std::endl;

  // Print the border between header and data
  std::cout << std::setw(columnWidth) << std::setfill('-') << "+"
            << std::setw(columnWidth) << "+" << std::setfill(' ') << "+"
            << std::endl;

  for (const auto &auction : auctions) {
    std::cout << "|" << std::setw(columnWidth - 1) << std::left << auction.first
              << "|" << std::setw(columnWidth - 1) << std::left
              << (auction.second ? "Active" : "Not Active") << "|" << std::endl;

    // Print the border between rows
    std::cout << std::setw(columnWidth) << std::setfill('-') << "+"
              << std::setw(columnWidth) << "+" << std::setfill(' ') << "+"
              << std::endl;
  }
}

void printListAuctionsTable(
    std::vector<std::pair<std::string, uint8_t>> auctions) {
  const int columnWidth = 15;

  // Print the top border
  std::cout << "+" << std::setw(columnWidth) << std::setfill('-') << "+"
            << std::setw(columnWidth) << "+" << std::setfill(' ') << std::endl;

  // Print the header
  std::cout << "|" << std::setw(columnWidth - 1) << std::left << "Auction ID"
            << "|" << std::setw(columnWidth - 1) << std::left << "Status"
            << "|" << std::endl;

  // Print the border between header and data
  std::cout << std::setw(columnWidth) << std::setfill('-') << "+"
            << std::setw(columnWidth) << "+" << std::setfill(' ') << "+"
            << std::endl;

  for (const auto &auction : auctions) {
    std::cout << "|" << std::setw(columnWidth - 1) << std::left << auction.first
              << "|" << std::setw(columnWidth - 1) << std::left
              << (auction.second ? "Active" : "Not Active") << "|" << std::endl;

    // Print the border between rows
    std::cout << std::setw(columnWidth) << std::setfill('-') << "+"
              << std::setw(columnWidth) << "+" << std::setfill(' ') << "+"
              << std::endl;
  }
}

void printListShowRecordTable(
    std::string hostUID, std::string auctionName, std::string assetFileName,
    uint32_t startValue, std::string startDate, uint32_t timeActive,
    std::vector<std::tuple<std::string, uint32_t, std::string, uint32_t>> bids,
    std::pair<std::string, uint32_t> end) {
  const int columnWidth = 30;

  std::cout << "Host ID: " << hostUID << "\t\t"
            << "Auction Name: " << auctionName << "\t"
            << "Asset Filename: " << assetFileName << "\t"
            << "Start Value: " << startValue << "\t"
            << "Start Date: " << startDate << "\t\t"
            << "Active Time: " << timeActive << std::endl;

  // Print the top border
  std::cout << "+" << std::setw(columnWidth) << std::setfill('-') << "+"
            << std::setw(columnWidth) << "+" << std::setw(columnWidth) << "+"
            << std::setw(columnWidth) << "+" << std::setfill(' ') << std::endl;

  // Print the header
  std::cout << "|" << std::setw(columnWidth - 1) << std::left << "Bidder"
            << "|" << std::setw(columnWidth - 1) << std::left << "Bid Value"
            << "|" << std::setw(columnWidth - 1) << std::left << "Bid Date"
            << "|" << std::setw(columnWidth - 1) << std::left << "Bid Sec Time"
            << "|" << std::endl;

  // Print the middle border
  std::cout << std::setw(columnWidth) << std::setfill('-') << "+"
            << std::setw(columnWidth) << "+" << std::setw(columnWidth) << "+"
            << std::setw(columnWidth) << "+" << std::setfill(' ') << "+"
            << std::endl;

  for (const auto &bid : bids) {
    // Accessing elements of the tuple
    std::string bidder_UID = std::get<0>(bid);
    uint32_t bid_value = std::get<1>(bid);
    std::string bid_date_time = std::get<2>(bid);
    uint32_t bid_sec_time = std::get<3>(bid);

    std::cout << "|" << std::setw(columnWidth - 1) << std::left << bidder_UID
              << "|" << std::setw(columnWidth - 1) << std::left << bid_value
              << "|" << std::setw(columnWidth - 1) << std::left << bid_date_time
              << "|" << std::setw(columnWidth - 1) << std::left << bid_sec_time
              << "|" << std::endl;

    // Print the border between rows
    std::cout << std::setw(columnWidth) << std::setfill('-') << "+"
              << std::setw(columnWidth) << "+" << std::setw(columnWidth) << "+"
              << std::setw(columnWidth) << "+" << std::setfill(' ') << "+"
              << std::endl;
  }
  if (end.first != "") {
    std::cout << "End Date: " << end.first << "\t\t"
              << "Time passed: " << end.second << "\t" << std::endl;
  }
}

std::vector<std::string> splitOnSeparator(std::string __str, char __separator) {
  std::stringstream start(__str);
  std::string word;
  std::vector<std::string> words;
  while (getline(start, word, __separator)) {
    words.push_back(word);
  }
  return words;
}

void sortAuctionBids(
    std::vector<std::tuple<std::string, uint32_t, std::string, uint32_t>>
        &auctionBids) {
  std::sort(
      auctionBids.begin(), auctionBids.end(),
      [](const std::tuple<std::string, uint32_t, std::string, uint32_t> &a,
         const std::tuple<std::string, uint32_t, std::string, uint32_t> &b) {
        return std::get<1>(a) > std::get<1>(b);
      });
}

void getTopNumBids(
    std::vector<std::tuple<std::string, uint32_t, std::string, uint32_t>>
        &auctionBids,
    uint32_t numBids) {
  if (auctionBids.size() > numBids) {
    auctionBids.erase(auctionBids.begin() + numBids, auctionBids.end());
  }
}

std::pair<std::string, uint32_t> getAuctionEndInfo(std::string auctionID) {
  // end format: end_datetime end_sec_time
  std::string auctionPath = AUCTION_DIR + SLASH + auctionID;
  std::string auctionEndPath =
      auctionPath + SLASH + END_FILE + auctionID + TXT_EXT;
  if (file_exists(auctionEndPath) != INVALID) {
    std::string auctionEndInfo;
    read_from_file(auctionEndPath, auctionEndInfo);
    std::vector<std::string> auctionEndWords =
        splitOnSeparator(auctionEndInfo, ' ');

    std::string end_datetime = auctionEndWords[0] + " " + auctionEndWords[1];
    uint32_t end_sec_time = (uint32_t)std::stoi(auctionEndWords[2]);

    return std::make_pair(end_datetime, end_sec_time);
  }
  return std::make_pair("", 0);
}

std::vector<std::tuple<std::string, uint32_t, std::string, uint32_t>>
getAuctionBids(std::string auctionID) {
  // bids format: UID bid_value bid_datetime bid_sec_time
  std::string auctionPath = AUCTION_DIR + SLASH + auctionID;
  std::string auctionBidsPath = auctionPath + BID_DIR;
  std::vector<std::tuple<std::string, uint32_t, std::string, uint32_t>>
      auctionBids;
  for (const auto &entry :
       std::filesystem::directory_iterator(auctionBidsPath)) {
    if (entry.is_regular_file() && entry.path().extension() == TXT_EXT) {
      std::string bidInfo;
      read_from_file(entry.path(), bidInfo);
      std::vector<std::string> bidWords = splitOnSeparator(bidInfo, ' ');

      std::string bidder = bidWords[0];
      uint32_t bidValue = (uint32_t)std::stoi(bidWords[1]);
      std::string bid_datetime = bidWords[2] + " " + bidWords[3];
      uint32_t bid_sec_time = (uint32_t)std::stoi(bidWords[4]);
      std::tuple<std::string, uint32_t, std::string, uint32_t> bids =
          std::make_tuple(bidder, bidValue, bid_datetime, bid_sec_time);
      auctionBids.push_back(bids);
    }
  }
  return auctionBids;
}
