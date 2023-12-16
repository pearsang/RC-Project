#include "protocol.hpp"

#include <sys/types.h>
#include <unistd.h>

#include <chrono>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <sys/stat.h>

#include <sys/socket.h>

extern bool is_exiting;

void UdpPacket::readPacketId(std::stringstream &buffer, const char *packet_id) {
  char current_char;
  while (*packet_id != '\0') {
    buffer >> current_char;
    if (!buffer.good() || current_char != *packet_id) {
      throw UnexpectedPacketException();
    }
    ++packet_id;
  }
}

void UdpPacket::readChar(std::stringstream &buffer, char chr) {
  if (readChar(buffer) != chr) {
    throw InvalidPacketException();
  }
}

char UdpPacket::readChar(std::stringstream &buffer) {
  char c;
  buffer >> c;
  if (!buffer.good()) {
    throw InvalidPacketException();
  }
  return c;
}

char UdpPacket::readAlphabeticalChar(std::stringstream &buffer) {
  char c;
  buffer >> c;

  if (!buffer.good() || !isalpha((unsigned char)c)) {
    throw InvalidPacketException();
  }
  return (char)tolower((unsigned char)c);
}

void UdpPacket::readSpace(std::stringstream &buffer) { readChar(buffer, ' '); }

void UdpPacket::readPacketDelimiter(std::stringstream &buffer) {
  readChar(buffer, '\n');
  buffer.peek();
  if (!buffer.eof()) {
    throw InvalidPacketException();
  }
}

std::string UdpPacket::readString(std::stringstream &buffer, uint32_t max_len) {
  std::string str;
  uint32_t i = 0;
  while (i < max_len) {
    char c = (char)buffer.get();
    if (!buffer.good()) {
      throw InvalidPacketException();
    }
    if (c == ' ' || c == '\n') {
      buffer.unget();
      break;
    }
    str += c;
    ++i;
  }
  return str;
}

std::string UdpPacket::readAlphabeticalString(std::stringstream &buffer,
                                              uint32_t max_len) {
  auto str = readString(buffer, max_len);
  for (uint32_t i = 0; i < str.length(); ++i) {
    if (!isalpha((unsigned char)str[i])) {
      throw InvalidPacketException();
    }

    str[i] = (char)tolower((unsigned char)str[i]);
  }
  return str;
}

uint32_t UdpPacket::readInt(std::stringstream &buffer) {
  int64_t i;
  buffer >> i;
  if (!buffer.good() || i < 0 || i > INT32_MAX) {
    throw InvalidPacketException();
  }
  return (uint32_t)i;
}

std::string UdpPacket::readTime(std::stringstream &buffer) {
  auto date = readString(buffer, 10);
  readSpace(buffer);
  auto time = readString(buffer, 8);
  return date + " " + time;
}

std::stringstream LoginRequest::serialize() {
  std::stringstream buffer;
  buffer << LoginRequest::ID << " " << this->userID << " " << this->password
         << std::endl;
  return buffer;
}

void LoginRequest::deserialize(std::stringstream &buffer) {
  buffer >> std::noskipws;
  // requests deserialize dont need to read ID
  readSpace(buffer);
  userID = readString(buffer, 6);
  readSpace(buffer);
  password = readString(buffer, 8);
  readPacketDelimiter(buffer);
}

std::stringstream LoginResponse::serialize() {
  std::stringstream buffer;
  buffer << LoginResponse::ID << " ";
  if (status == OK) {
    buffer << "OK";
  } else if (status == NOK) {
    buffer << "NOK";
  } else if (status == REG) {
    buffer << "REG";
  } else if (status == ERR) {
    buffer << "ERR";
  } else {
    throw InvalidPacketException();
  }
  buffer << std::endl;
  return buffer;
};

void LoginResponse::deserialize(std::stringstream &buffer) {
  buffer >> std::noskipws;
  readPacketId(buffer, LoginResponse::ID);
  readSpace(buffer);
  auto status_str = readString(buffer, 3);
  if (status_str == "OK") {
    status = OK;
  } else if (status_str == "NOK") {
    status = NOK;
  } else if (status_str == "REG") {
    status = REG;
  } else if (status_str == "ERR") {
    status = ERR;
  } else {
    throw InvalidPacketException();
  }
  readPacketDelimiter(buffer);
};

std::stringstream LogoutRequest::serialize() {
  std::stringstream buffer;
  buffer << LogoutRequest::ID << " " << this->userID << " " << this->password
         << std::endl;
  return buffer;
}

void LogoutRequest::deserialize(std::stringstream &buffer) {
  buffer >> std::noskipws;
  // requests deserialize dont need to read ID
  readSpace(buffer);
  userID = readString(buffer, 6);
  readSpace(buffer);
  password = readString(buffer, 8);
  readPacketDelimiter(buffer);
}

std::stringstream LogoutResponse::serialize() {
  std::stringstream buffer;
  buffer << LogoutResponse::ID << " ";
  if (status == OK) {
    buffer << "OK";
  } else if (status == NOK) {
    buffer << "NOK";
  } else if (status == UNR) {
    buffer << "UNR";
  } else if (status == ERR) {
    buffer << "ERR";
  } else {
    throw InvalidPacketException();
  }
  buffer << std::endl;
  return buffer;
};

void LogoutResponse::deserialize(std::stringstream &buffer) {
  buffer >> std::noskipws;
  readPacketId(buffer, LogoutResponse::ID);
  readSpace(buffer);
  auto status_str = readString(buffer, 3);
  if (status_str == "OK") {
    status = OK;
  } else if (status_str == "NOK") {
    status = NOK;
  } else if (status_str == "UNR") {
    status = UNR;
  } else if (status_str == "ERR") {
    status = ERR;
  } else {
    throw InvalidPacketException();
  }
  readPacketDelimiter(buffer);
};

std::stringstream UnregisterRequest::serialize() {
  std::stringstream buffer;
  buffer << UnregisterRequest::ID << " " << this->userID << " "
         << this->password << std::endl;
  return buffer;
}

void UnregisterRequest::deserialize(std::stringstream &buffer) {
  buffer >> std::noskipws;
  // requests deserialize dont need to read ID
  readSpace(buffer);
  userID = readString(buffer, 6);
  readSpace(buffer);
  password = readString(buffer, 8);
  readPacketDelimiter(buffer);
}

std::stringstream UnregisterResponse::serialize() {
  std::stringstream buffer;
  buffer << UnregisterResponse::ID << " ";
  if (status == OK) {
    buffer << "OK";
  } else if (status == NOK) {
    buffer << "NOK";
  } else if (status == UNR) {
    buffer << "UNR";
  } else if (status == ERR) {
    buffer << "ERR";
  } else {
    throw InvalidPacketException();
  }
  buffer << std::endl;
  return buffer;
};

void UnregisterResponse::deserialize(std::stringstream &buffer) {
  buffer >> std::noskipws;
  readPacketId(buffer, UnregisterResponse::ID);
  readSpace(buffer);
  auto status_str = readString(buffer, 3);
  if (status_str == "OK") {
    status = OK;
  } else if (status_str == "NOK") {
    status = NOK;
  } else if (status_str == "UNR") {
    status = UNR;
  } else if (status_str == "ERR") {
    status = ERR;
  } else {
    throw InvalidPacketException();
  }
  readPacketDelimiter(buffer);
};

std::stringstream ListUserAuctionsRequest::serialize() {
  std::stringstream buffer;
  buffer << ListUserAuctionsRequest::ID << " " << this->userID << std::endl;
  return buffer;
}

void ListUserAuctionsRequest::deserialize(std::stringstream &buffer) {
  buffer >> std::noskipws;
  readSpace(buffer);
  userID = readString(buffer, 6);
  readPacketDelimiter(buffer);
}

std::stringstream ListUserAuctionsResponse::serialize() {
  std::stringstream buffer;
  buffer << ListUserAuctionsResponse::ID << " ";
  if (status == OK) {
    buffer << "OK";
    for (auto auction : auctions) {
      buffer << " " << auction.first << " " << std::to_string(auction.second);
    }
  } else if (status == NOK) {
    buffer << "NOK";
  } else if (status == NLG) {
    buffer << "NLG";
  } else if (status == ERR) {
    buffer << "ERR";
  } else {
    throw InvalidPacketException();
  }
  buffer << std::endl;
  return buffer;
};

void ListUserAuctionsResponse::deserialize(std::stringstream &buffer) {
  buffer >> std::noskipws;
  readPacketId(buffer, ListUserAuctionsResponse::ID);
  readSpace(buffer);
  auto status_str = readString(buffer, 3);
  if (status_str == "OK") {
    status = OK;

    while (buffer.peek() != '\n') {
      readSpace(buffer);
      auto auction_id = readString(buffer, 3);
      readSpace(buffer);
      auto state = readInt(buffer);
      auctions.push_back(std::make_pair(auction_id, (uint8_t)state));
    }
  } else if (status_str == "NOK") {
    status = NOK;
  } else if (status_str == "NLG") {
    status = NLG;
  } else if (status_str == "ERR") {
    status = ERR;
  } else {
    throw InvalidPacketException();
  }
  readPacketDelimiter(buffer);
};

std::stringstream ListUserBidsRequest::serialize() {
  std::stringstream buffer;
  buffer << ListUserBidsRequest::ID << " " << this->userID << std::endl;
  return buffer;
}

void ListUserBidsRequest::deserialize(std::stringstream &buffer) {
  buffer >> std::noskipws;
  readSpace(buffer);
  userID = readString(buffer, 6);
  readPacketDelimiter(buffer);
}

std::stringstream ListUserBidsResponse::serialize() {
  std::stringstream buffer;
  buffer << ListUserBidsResponse::ID << " ";
  if (status == OK) {
    buffer << "OK";
    for (auto auction : auctions) {
      buffer << " " << auction.first << " " << std::to_string(auction.second);
    }
  } else if (status == NOK) {
    buffer << "NOK";
  } else if (status == NLG) {
    buffer << "NLG";
  } else if (status == ERR) {
    buffer << "ERR";
  } else {
    throw InvalidPacketException();
  }
  buffer << std::endl;
  return buffer;
};

void ListUserBidsResponse::deserialize(std::stringstream &buffer) {
  buffer >> std::noskipws;
  readPacketId(buffer, ListUserBidsResponse::ID);
  readSpace(buffer);
  auto status_str = readString(buffer, 3);
  if (status_str == "OK") {
    status = OK;

    while (buffer.peek() != '\n') {
      readSpace(buffer);
      auto auction_id = readString(buffer, 3);
      readSpace(buffer);
      auto state = readInt(buffer);
      auctions.push_back(std::make_pair(auction_id, (uint8_t)state));
    }
  } else if (status_str == "NOK") {
    status = NOK;
  } else if (status_str == "NLG") {
    status = NLG;
  } else if (status_str == "ERR") {
    status = ERR;
  } else {
    throw InvalidPacketException();
  }
  readPacketDelimiter(buffer);
};

std::stringstream ListAuctionsRequest::serialize() {
  std::stringstream buffer;
  buffer << ListAuctionsRequest::ID << std::endl;
  return buffer;
}

void ListAuctionsRequest::deserialize(std::stringstream &buffer) {
  buffer >> std::noskipws;
  readPacketDelimiter(buffer);
}

std::stringstream ListAuctionsResponse::serialize() {
  std::stringstream buffer;
  buffer << ListAuctionsResponse::ID << " ";

  if (status == OK) {
    buffer << "OK";
    for (auto auction : this->auctions) {
      buffer << " " << auction.first << " " << std::to_string(auction.second);
    }
  } else if (status == NOK) {
    buffer << "NOK";
  } else if (status == ERR) {
    buffer << "ERR";
  } else {
    std::cout << "InvalidPacketException" << std::endl;
    throw InvalidPacketException();
  }

  buffer << std::endl;
  return buffer;
};

void ListAuctionsResponse::deserialize(std::stringstream &buffer) {
  buffer >> std::noskipws;
  readPacketId(buffer, ListAuctionsResponse::ID);
  readSpace(buffer);
  auto status_str = readString(buffer, 3);
  if (status_str == "OK") {
    status = OK;

    while (buffer.peek() != '\n') {
      readSpace(buffer);
      auto auction_id = readString(buffer, 3);
      readSpace(buffer);
      auto state = readInt(buffer);
      auctions.push_back(std::make_pair(auction_id, (uint8_t)state));
    }
  } else if (status_str == "NOK") {
    status = NOK;
  } else if (status_str == "ERR") {
    status = ERR;
  } else {
    throw InvalidPacketException();
  }
  readPacketDelimiter(buffer);
};

std::stringstream ShowRecordRequest::serialize() {
  std::stringstream buffer;
  buffer << ShowRecordRequest::ID << " " << this->auctionID << std::endl;
  return buffer;
}

void ShowRecordRequest::deserialize(std::stringstream &buffer) {
  buffer >> std::noskipws;
  readSpace(buffer);
  auctionID = readString(buffer, 3);
  readPacketDelimiter(buffer);
}

std::stringstream ShowRecordResponse::serialize() {
  std::stringstream buffer;
  buffer << ShowRecordResponse::ID << " ";
  if (status == OK) {
    buffer << "OK"
           << " " << hostUID << " " << auctionName << " " << assetFileName
           << " " << startValue << " " << startDate << " " << timeActive;
    for (auto bid : bids) {
      buffer << " B " << std::get<0>(bid) << " " << std::get<1>(bid) << " "
             << std::get<2>(bid) << " " << std::get<3>(bid);
    }
    if (end.first != "") {
      buffer << " E " << end.first << " " << end.second;
    }
  } else if (status == NOK) {
    buffer << "NOK";
  } else if (status == ERR) {
    buffer << "ERR";
  } else {
    throw InvalidPacketException();
  }
  buffer << std::endl;
  return buffer;
};

void ShowRecordResponse::deserialize(std::stringstream &buffer) {
  buffer >> std::noskipws;
  readPacketId(buffer, ShowRecordResponse::ID);
  readSpace(buffer);
  auto status_str = readString(buffer, 3);
  if (status_str == "OK") {
    status = OK;
    readSpace(buffer);
    auto user_id = readString(buffer, 6);
    hostUID = user_id;
    readSpace(buffer);
    auto auction_name = readString(buffer, 10);
    auctionName = auction_name;
    readSpace(buffer);
    auto asset_filename = readString(buffer, 24);
    assetFileName = asset_filename;
    readSpace(buffer);
    auto start_price = readInt(buffer);
    startValue = start_price;
    readSpace(buffer);
    auto start_date = readTime(buffer);
    startDate = start_date;
    readSpace(buffer);
    auto active_time = readInt(buffer);
    timeActive = active_time;

    while (buffer.peek() != '\n') {
      readSpace(buffer);
      char b_or_e = readChar(buffer);
      if (b_or_e == 'B') {
        readSpace(buffer);
        auto bidder_id = readString(buffer, 6);
        readSpace(buffer);
        auto bid_value = readInt(buffer);
        readSpace(buffer);
        auto bid_date = readTime(buffer);
        readSpace(buffer);
        auto bid_sec_time = readInt(buffer);
        bids.push_back(
            std::make_tuple(bidder_id, bid_value, bid_date, bid_sec_time));
      } else if (b_or_e == 'E') {
        readSpace(buffer);
        auto end_date_time = readTime(buffer);
        readSpace(buffer);
        auto end_sec_time = readInt(buffer);
        end = std::make_pair(end_date_time, end_sec_time);
      } else {
        continue;
      }
    }
  } else if (status_str == "NOK") {
    status = NOK;
  } else if (status_str == "ERR") {
    status = ERR;
  } else {
    throw InvalidPacketException();
  }
  readPacketDelimiter(buffer);
};

std::stringstream ErrorUdpPacket::serialize() {
  std::stringstream buffer;
  buffer << ErrorUdpPacket::ID << std::endl;
  return buffer;
};

void ErrorUdpPacket::deserialize(std::stringstream &buffer) { (void)buffer; };

// TCP
void TcpPacket::writeString(int fd, const std::string &str) {
  const char *buffer = str.c_str();
  ssize_t bytes_to_send = (ssize_t)str.length();
  ssize_t bytes_sent = 0;
  while (bytes_sent < bytes_to_send) {
    ssize_t sent =
        write(fd, buffer + bytes_sent, (size_t)(bytes_to_send - bytes_sent));
    if (sent < 0) {
      throw PacketSerializationException();
    }
    bytes_sent += sent;
  }
}

void TcpPacket::readPacketId(int fd, const char *packet_id) {
  char current_char;
  while (*packet_id != '\0') {
    if (read(fd, &current_char, 1) != 1 || current_char != *packet_id) {
      throw UnexpectedPacketException();
    }
    ++packet_id;
  }
}

void TcpPacket::readChar(int fd, char chr) {
  if (readChar(fd) != chr) {
    throw InvalidPacketException();
  }
}

char TcpPacket::readChar(int fd) {
  char c = 0;
  if (delimiter != 0) {
    // use last read char instead, since it wasn't consumed yet
    c = delimiter;
    delimiter = 0;
    return c;
  }
  if (read(fd, &c, 1) != 1) {
    throw InvalidPacketException();
  }
  return c;
}

void TcpPacket::readSpace(int fd) { readChar(fd, ' '); }

void TcpPacket::readPacketDelimiter(int fd) { readChar(fd, '\n'); }

std::string TcpPacket::readString(const int fd) {
  std::string result;
  char c = 0;

  while (!std::iswspace((wint_t)c)) {
    if (read(fd, &c, 1) != 1) {
      throw InvalidPacketException();
    }
    result += c;
  }
  delimiter = c;

  result.pop_back();

  return result;
}

uint32_t TcpPacket::readInt(const int fd) {
  std::string int_str = readString(fd);
  try {
    size_t converted = 0;
    int64_t result = std::stoll(int_str, &converted, 10);
    if (converted != int_str.length() || std::iswspace((wint_t)int_str.at(0)) ||
        result < 0 || result > INT32_MAX) {
      throw InvalidPacketException();
    }
    return (uint32_t)result;
  } catch (InvalidPacketException &ex) {
    throw ex;
  } catch (...) {
    throw InvalidPacketException();
  }
}

void ErrorTcpPacket::send(int fd) {
  writeString(fd, ErrorTcpPacket::ID);
  writeString(fd, "\n");
}

void ErrorTcpPacket::receive(int fd) { (void)fd; }

std::string generateUniqueIdentifier() {
  // Use current time as a seed for the random number generator
  auto seed =
      std::chrono::high_resolution_clock::now().time_since_epoch().count();
  std::mt19937 generator(static_cast<unsigned int>(seed));

  // Generate a random number and convert it to a string
  std::uniform_int_distribution<unsigned long long> distribution;
  unsigned long long random_number = distribution(generator);

  // Convert the random number to a string
  std::string unique_identifier = std::to_string(random_number);

  return unique_identifier;
}

std::string TcpPacket::readAndSaveToFile(const int fd, std::string &file_name,
                                         const size_t file_size, bool flag) {
  if (flag) {
    std::string filepath = generateUniqueIdentifier();
    create_new_directory(filepath);
    filepath += SLASH + file_name;
    file_name = filepath;
  }
  std::ofstream file(file_name);
  if (!file.good()) {
    throw IOException();
  }

  size_t remaining_size = file_size;
  size_t to_read;
  ssize_t n;
  char buffer[FILE_BUFFER_LEN];

  while (remaining_size > 0) {
    fd_set file_descriptors;
    FD_ZERO(&file_descriptors);
    FD_SET(fd, &file_descriptors);

    struct timeval timeout;
    timeout.tv_sec = TCP_READ_TIMEOUT_SECONDS;
    timeout.tv_usec = 0;

    int ready_fd = select(std::max(fd, fileno(stdin)) + 1, &file_descriptors,
                          NULL, NULL, &timeout);
    if (is_exiting) {
      std::cout << "Cancelling TCP download, player is shutting down..."
                << std::endl;
      throw OperationCancelledException();
    }
    if (ready_fd == -1) {
      perror("select");
      throw ConnectionTimeoutException();
    } else if (FD_ISSET(fd, &file_descriptors)) {
      // Read from socket
      to_read = std::min(remaining_size, (size_t)FILE_BUFFER_LEN);
      n = read(fd, buffer, to_read);
      if (n <= 0) {
        file.close();
        throw InvalidPacketException();
      }
      file.write(buffer, n);
      if (!file.good()) {
        file.close();
        throw IOException();
      }
      remaining_size -= (size_t)n;

    } else if (FD_ISSET(fileno(stdin), &file_descriptors)) {
      if (std::cin.peek() != '\n') {
        continue;
      }
      std::cin.get();
      std::cout << "Cancelling TCP download" << std::endl;
      throw OperationCancelledException();
    } else {
      throw ConnectionTimeoutException();
    }
  }

  file.close();
  // print filepath
  return file_name;
}

void ShowAssetRequest::send(int fd) {
  std::stringstream stream;
  stream << ShowAssetRequest::ID << " " << this->auctionID << std::endl;
  writeString(fd, stream.str());
}

void ShowAssetRequest::receive(int fd) {
  readSpace(fd);
  auctionID = readString(fd);
  readPacketDelimiter(fd);
}

void ShowAssetResponse::send(int fd) {
  std::stringstream stream;
  stream << ShowAssetResponse::ID << " ";

  if (status == OK) {
    stream << "OK";
    stream << " " << assetFileName << " " << assetSize << " ";
    writeString(fd, stream.str());

    stream.str(std::string());
    stream.clear();
    sendFile(fd, assetPath);
  } else if (status == NOK) {
    stream << "NOK";
  } else if (status == ERR) {
    stream << "ERR";
  } else {
    throw InvalidPacketException();
  }
  stream << std::endl;
  writeString(fd, stream.str());
}

void ShowAssetResponse::receive(int fd) {
  readPacketId(fd, ShowAssetResponse::ID);
  readSpace(fd);
  auto status_str = readString(fd);
  if (status_str == "OK") {
    this->status = OK;
    readSpace(fd);
    assetFileName = readString(fd);
    readSpace(fd);
    assetSize = readInt(fd);
    readSpace(fd);
    assetPath = readAndSaveToFile(fd, assetFileName, assetSize, false);
  } else if (status_str == "NOK") {
    this->status = NOK;
  } else {
    throw InvalidPacketException();
  }
  readPacketDelimiter(fd);
}

void OpenAuctionRequest::send(int fd) {
  std::stringstream stream;
  stream << OpenAuctionRequest::ID << " " << this->userID << " "
         << this->password << " " << this->auctionName << " "
         << this->startValue << " " << this->timeActive << " "
         << this->assetFileName << " " << assetSize << " ";
  writeString(fd, stream.str());

  stream.str(std::string());
  stream.clear();
  sendFile(fd, this->assetFileName);

  stream << std::endl;
  writeString(fd, stream.str());
}

void OpenAuctionRequest::receive(int fd) {
  readSpace(fd);
  userID = readString(fd);
  readSpace(fd);
  password = readString(fd);

  // must confirm the user password is correct
  if (this->password != getUserPassword(userID)) {
    throw InvalidPacketException();
  }

  readSpace(fd);
  auctionName = readString(fd);
  readSpace(fd);
  startValue = readInt(fd);
  readSpace(fd);
  timeActive = readInt(fd);
  readSpace(fd);
  assetFileName = readString(fd);
  readSpace(fd);
  assetSize = readInt(fd);

  if (validateAssetFileSize(assetSize) == INVALID) {
    throw InvalidPacketException();
  }

  readSpace(fd);
  std::string originalAssetFileName = assetFileName;
  assetPath = readAndSaveToFile(fd, originalAssetFileName, assetSize, true);
  readPacketDelimiter(fd);
}

void OpenAuctionResponse::send(int fd) {
  std::stringstream stream;
  stream << OpenAuctionResponse::ID << " ";
  if (status == OK) {
    stream << "OK";
    stream << " " << auctionID;
  } else if (status == NOK) {
    stream << "NOK";
  } else if (status == NLG) {
    stream << "NLG";
  } else if (status == ERR) {
    stream << "ERR";
  } else {
    throw InvalidPacketException();
  }
  stream << std::endl;
  writeString(fd, stream.str());
}

void OpenAuctionResponse::receive(int fd) {
  readPacketId(fd, OpenAuctionResponse::ID);
  readSpace(fd);
  auto status_str = readString(fd);
  if (status_str == "OK") {
    this->status = OK;
    readSpace(fd);
    this->auctionID = readString(fd);
  } else if (status_str == "NOK") {
    this->status = NOK;
  } else if (status_str == "NLG") {
    this->status = NLG;
  } else if (status_str == "ERR") {
    this->status = ERR;
  } else {
    throw InvalidPacketException();
  }
  readPacketDelimiter(fd);
}

void CloseAuctionRequest::send(int fd) {
  std::stringstream stream;
  stream << CloseAuctionRequest::ID << " " << this->userID << " "
         << this->password << " " << this->auctionID << std::endl;
  writeString(fd, stream.str());
}

void CloseAuctionRequest::receive(int fd) {
  readSpace(fd);
  userID = readString(fd);
  readSpace(fd);
  password = readString(fd);
  readSpace(fd);
  auctionID = readString(fd);
  readPacketDelimiter(fd);
}

void CloseAuctionResponse::send(int fd) {
  std::stringstream stream;
  stream << CloseAuctionResponse::ID << " ";

  if (status == OK) {
    stream << "OK";
  } else if (status == EAU) {
    stream << "EAU";
  } else if (status == NLG) {
    stream << "NLG";
  } else if (status == EOW) {
    stream << "EOW";
  } else if (status == END) {
    stream << "END";
  } else if (status == ERR) {
    stream << "ERR";
  } else {
    throw InvalidPacketException();
  }
  stream << std::endl;
  writeString(fd, stream.str());
}

void CloseAuctionResponse::receive(int fd) {
  readPacketId(fd, CloseAuctionResponse::ID);
  readSpace(fd);
  auto status_str = readString(fd);
  if (status_str == "OK") {
    this->status = OK;
  } else if (status_str == "EAU") {
    this->status = EAU;
  } else if (status_str == "EOW") {
    this->status = EOW;
  } else if (status_str == "END") {
    this->status = END;
  } else if (status_str == "ERR") {
    this->status = ERR;
  } else if (status_str == "NLG") {
    this->status = NLG;
  } else {
    throw InvalidPacketException();
  }
  readPacketDelimiter(fd);
}

void BidRequest::send(int fd) {
  std::stringstream stream;
  stream << BidRequest::ID << " " << this->userID << " " << this->password
         << " " << this->auctionID << " " << this->bidValue << std::endl;
  writeString(fd, stream.str());
}

void BidRequest::receive(int fd) {
  readSpace(fd);
  userID = readString(fd);
  readSpace(fd);
  password = readString(fd);
  readSpace(fd);
  auctionID = readString(fd);
  readSpace(fd);
  bidValue = readInt(fd);
  readPacketDelimiter(fd);
}

void BidResponse::send(int fd) {
  std::stringstream stream;
  stream << BidResponse::ID << " ";

  if (status == ACC) {
    stream << "ACC";
  } else if (status == NOK) {
    stream << "NOK";
  } else if (status == NLG) {
    stream << "NLG";
  } else if (status == REF) {
    stream << "REF";
  } else if (status == ILG) {
    stream << "ILG";
  } else if (status == ERR) {
    stream << "ERR";
  } else {
    throw InvalidPacketException();
  }
  stream << std::endl;
  writeString(fd, stream.str());
}

void BidResponse::receive(int fd) {
  readPacketId(fd, BidResponse::ID);
  readSpace(fd);
  auto status_str = readString(fd);
  if (status_str == "ACC") {
    this->status = ACC;
  } else if (status_str == "NOK") {
    this->status = NOK;
  } else if (status_str == "NLG") {
    this->status = NLG;
  } else if (status_str == "REF") {
    this->status = REF;
  } else if (status_str == "ILG") {
    this->status = ILG;
  } else if (status_str == "ERR") {
    this->status = ERR;
  } else {
    throw InvalidPacketException();
  }
  readPacketDelimiter(fd);
}

// TCP END

// Packet sending and receiving
void send_packet(UdpPacket &packet, int socket, struct sockaddr *address,
                 socklen_t addrlen) {
  const std::stringstream buffer = packet.serialize();
  ssize_t n = sendto(socket, buffer.str().c_str(), buffer.str().length(), 0,
                     address, addrlen);
  if (n == -1) {
    throw FatalError("Failed to send UDP packet", errno);
  }
}

void wait_for_packet(UdpPacket &packet, int socket) {
  fd_set file_descriptors;
  FD_ZERO(&file_descriptors);
  FD_SET(socket, &file_descriptors);

  struct timeval timeout;
  timeout.tv_sec = UDP_TIMEOUT_SECONDS; // wait for a response before throwing
  timeout.tv_usec = 0;

  int ready_fd = select(socket + 1, &file_descriptors, NULL, NULL, &timeout);
  if (is_exiting) {
    throw OperationCancelledException();
  }
  if (ready_fd == -1) {
    throw FatalError("Failed waiting for UDP packet on select", errno);
  } else if (ready_fd == 0) {
    throw ConnectionTimeoutException();
  }

  std::stringstream data;
  char buffer[SOCKET_BUFFER_LEN];

  ssize_t n = recvfrom(socket, buffer, SOCKET_BUFFER_LEN, 0, NULL, NULL);
  if (n == -1) {
    throw FatalError("Failed waiting for UDP packet on recvfrom", errno);
  }

  data.write(buffer, n);

  packet.deserialize(data);
}

void sendFile(int fd, std::filesystem::path file_path) {
  std::ifstream file(file_path, std::ios::in | std::ios::binary);
  if (!file) {
    std::cerr << "Error opening file: " << file_path << std::endl;
    throw PacketSerializationException();
  }
  char buffer[FILE_BUFFER_LEN];

  while (file) {
    file.read(buffer, FILE_BUFFER_LEN);
    ssize_t bytes_read = (ssize_t)file.gcount();
    ssize_t bytes_sent = 0;
    while (bytes_sent < bytes_read) {
      ssize_t sent =
          write(fd, buffer + bytes_sent, (size_t)(bytes_read - bytes_sent));
      if (sent < 0) {
        throw PacketSerializationException();
      }
      bytes_sent += sent;
    }
  }
}

uint32_t getFileSize(std::filesystem::path file_path) {
  try {
    return (uint32_t)std::filesystem::file_size(file_path);
  } catch (...) {
    throw PacketSerializationException();
  }
}
