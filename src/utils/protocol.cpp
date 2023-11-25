#include "protocol.hpp"

#include <sys/types.h>
#include <unistd.h>

#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

#include "../utils/utils.hpp"
#include "constants.hpp"
#include <sys/socket.h>

extern bool is_shutting_down;

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

std::stringstream LoginRequest::serialize() {
  std::stringstream buffer;
  buffer << LoginRequest::ID << " " << this->userID << " " << this->password
         << std::endl;
  return buffer;
}

void LoginRequest::deserialize(std::stringstream &buffer) {
  // server stuff
  buffer >> this->userID >> this->password;
}

std::stringstream LoginResponse::serialize() {
  // server stuff
  std::stringstream buffer;
  buffer << LoginResponse::ID << " " << this->status << std::endl;
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
  // server stuff
  buffer >> this->userID >> this->password;
}

std::stringstream LogoutResponse::serialize() {
  // server stuff
  std::stringstream buffer;
  buffer << LogoutResponse::ID << " " << this->status << std::endl;
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
  // server stuff
  buffer >> this->userID >> this->password;
}

std::stringstream UnregisterResponse::serialize() {
  // server stuff
  std::stringstream buffer;
  buffer << UnregisterResponse::ID << " " << this->status << std::endl;
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
  // server stuff
  buffer >> this->userID;
}

std::stringstream ListUserAuctionsResponse::serialize() {
  // server stuff
  std::stringstream buffer;
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
      auctions.push_back(std::make_pair(auction_id, (uint8_t)1));
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
  buffer >> this->userID;
}

std::stringstream ListAuctionsRequest::serialize() {
  std::stringstream buffer;
  buffer << ListAuctionsRequest::ID << std::endl;
  return buffer;
}

void ListAuctionsRequest::deserialize(std::stringstream &buffer) {}

std::stringstream ShowRecordRequest::serialize() {
  std::stringstream buffer;
  buffer << ShowRecordRequest::ID << " " << this->auctionID << std::endl;
  return buffer;
}

void ShowRecordRequest::deserialize(std::stringstream &buffer) {
  buffer >> this->auctionID;
}

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
  if (is_shutting_down) {
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
