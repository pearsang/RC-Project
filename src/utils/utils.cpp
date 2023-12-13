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
  if (!is_digits(bidValue)) {
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
  if (!is_digits(startValue) || startValue.length() > START_VALUE_MAX) {
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
  std::string passwordPath = USERDIR;
  std::string validPassword;
  try {
    passwordPath += "/" + userID + "/" + userID + "_pass.txt";
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

  // Convert the time_t object to a tm struct
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

std::string intToStringWithZeros(int number) {
  // Ensure the number is within the valid range
  if (number < 0 || number > 999) {
    throw std::runtime_error("Invalid number");
  }

  // Use stringstream to convert int to string with leading zeros
  std::ostringstream oss;
  oss << std::setw(3) << std::setfill('0') << number;

  return oss.str();
}
