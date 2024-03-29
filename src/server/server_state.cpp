#include "server_state.hpp"

#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <unistd.h>

#include "../utils/protocol.hpp"
#include "../utils/utils.hpp"
#include "handlers.hpp"

AuctionServerState::AuctionServerState(std::string &port, bool _verbose)
    : verbose{VerboseStream(_verbose)} {
  this->setupUdpSocket();
  this->setupTcpSocket();
  this->resolveServerAddress(port);
}

AuctionServerState::~AuctionServerState() {
  if (this->udpSocketFD != -1) {
    close(this->udpSocketFD);
  }
  if (this->tcpSocketFD != -1) {
    close(this->tcpSocketFD);
  }
  if (this->serverUdpAddr != NULL) {
    freeaddrinfo(this->serverUdpAddr);
  }
  if (this->serverTcpAddr != NULL) {
    freeaddrinfo(this->serverTcpAddr);
  }
}

void AuctionServerState::resolveServerAddress(std::string &port) {
  struct addrinfo hints;
  int addr_res;
  const char *port_str = port.c_str();
  // Get the UDP address
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET;      // IPv4
  hints.ai_socktype = SOCK_DGRAM; // UDP socket
  hints.ai_flags = AI_PASSIVE;    // Listen on 0.0.0.0
  if ((addr_res = getaddrinfo(NULL, port_str, &hints, &this->serverUdpAddr)) !=
      0) {
    throw FatalError(std::string("Failed to get address for UDP connection: ") +
                     gai_strerror(addr_res));
  }
  if (bind(this->udpSocketFD, this->serverUdpAddr->ai_addr,
           this->serverUdpAddr->ai_addrlen)) {
    throw FatalError("Failed to bind UDP address", errno);
  }

  // Get the TCP address
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET;       // IPv4
  hints.ai_socktype = SOCK_STREAM; // TCP socket
  hints.ai_flags = AI_PASSIVE;     // Listen on 0.0.0.0
  if ((addr_res = getaddrinfo(NULL, port.c_str(), &hints,
                              &this->serverTcpAddr)) != 0) {
    throw FatalError(std::string("Failed to get address for TCP connection: ") +
                     gai_strerror(addr_res));
  }

  if (bind(this->tcpSocketFD, this->serverTcpAddr->ai_addr,
           this->serverTcpAddr->ai_addrlen)) {
    throw FatalError("Failed to bind TCP address", errno);
  }

  std::cout << "Listening for connections on port " << port << std::endl;
}

void AuctionServerState::setupUdpSocket() {
  // Create a UDP socket
  if ((this->udpSocketFD = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
    throw FatalError("Failed to create a UDP socket", errno);
  }
  struct timeval timeout;
  timeout.tv_sec = SERVER_TIMEOUT;
  timeout.tv_usec = 0;
  if (setsockopt(this->udpSocketFD, SOL_SOCKET, SO_RCVTIMEO, &timeout,
                 sizeof(timeout)) < 0) {
    throw FatalError("Failed to set UDP read timeout socket option", errno);
  }
}

void AuctionServerState::setupTcpSocket() {
  // Create a TCP socket
  if ((this->tcpSocketFD = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    throw FatalError("Failed to create a TCP socket", errno);
  }
  const int enable = 1;
  if (setsockopt(this->tcpSocketFD, SOL_SOCKET, SO_REUSEADDR, &enable,
                 sizeof(int)) < 0) {
    throw FatalError("Failed to set TCP reuse address socket option", errno);
  }
  struct timeval timeoutRead;
  timeoutRead.tv_sec = SERVER_TIMEOUT;
  timeoutRead.tv_usec = 0;
  if (setsockopt(this->tcpSocketFD, SOL_SOCKET, SO_RCVTIMEO, &timeoutRead,
                 sizeof(timeoutRead)) < 0) {
    throw FatalError("Failed to set TCP read timeout socket option", errno);
  }
  struct timeval timeoutWrite;
  timeoutWrite.tv_sec = TCP_WRITE_TIMEOUT_SECONDS;
  timeoutWrite.tv_usec = 0;
  if (setsockopt(this->tcpSocketFD, SOL_SOCKET, SO_SNDTIMEO, &timeoutWrite,
                 sizeof(timeoutWrite)) < 0) {
    throw FatalError("Failed to set TCP write timeout socket option", errno);
  }
}

void AuctionServerState::registerHandlers() {
  registerUdpPacketHandlers();
  registerTcpPacketHandlers();
}

void AuctionServerState::registerUdpPacketHandlers() {
  // add to UDP packet handlers
  UdpPacketHandlers.insert({LoginRequest::ID, handleLogin});
  UdpPacketHandlers.insert({LogoutRequest::ID, handleLogout});
  UdpPacketHandlers.insert({UnregisterRequest::ID, handleUnregister});
  UdpPacketHandlers.insert(
      {ListUserAuctionsRequest::ID, handleListUserAuctions});
  UdpPacketHandlers.insert({ListUserBidsRequest::ID, handleListUserBids});
  UdpPacketHandlers.insert({ListAuctionsRequest::ID, handleListAuctions});
  UdpPacketHandlers.insert({ShowRecordRequest::ID, handleShowRecord});
}

void AuctionServerState::registerTcpPacketHandlers() {
  // add to TCP packet handlers
  TcpPacketHandlers.insert({OpenAuctionRequest::ID, handleOpenAuction});
  TcpPacketHandlers.insert({CloseAuctionRequest::ID, handleCloseAuction});
  TcpPacketHandlers.insert({ShowAssetRequest::ID, handleShowAsset});
  TcpPacketHandlers.insert({BidRequest::ID, handleBid});
}

void AuctionServerState::callUdpPacketHandler(std::string packet_id,
                                              std::stringstream &stream,
                                              SocketAddress &source_addr) {
  auto handler = this->UdpPacketHandlers.find(packet_id);
  if (handler == this->UdpPacketHandlers.end()) {
    verbose << "Received unknown Packet ID" << std::endl;
    throw InvalidPacketException();
  }

  handler->second(*this, stream, source_addr);
}

void AuctionServerState::callTcpPacketHandler(std::string packet_id, int fd) {
  auto handler = this->TcpPacketHandlers.find(packet_id);
  if (handler == this->TcpPacketHandlers.end()) {
    verbose << "Received unknown Packet ID" << std::endl;
    throw InvalidPacketException();
  }

  handler->second(*this, fd);
}