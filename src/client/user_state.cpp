#include "user_state.hpp"

#include <unistd.h>

#include <cstring>
#include <iostream>

#include "utils/constants.hpp"
#include "utils/utils.hpp"

UserState::UserState(std::string &hostname, std::string &port) {
  this->setupUdpSocket();
  this->resolveServerAddress(hostname, port);
}

UserState::~UserState() {
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

void UserState::setupUdpSocket() {
  // Create a UDP socket
  if ((this->udpSocketFD = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
    throw FatalError("Failed to create a UDP socket", errno);
  }
}

void UserState::resolveServerAddress(std::string &hostname, std::string &port) {
  struct addrinfo hints;
  int addr_res;
  const char *host = hostname.c_str();
  const char *port_str = port.c_str();

  // Get UDP address
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET;      // IPv4
  hints.ai_socktype = SOCK_DGRAM; // UDP socket
  // check if address is valid
  if ((addr_res = getaddrinfo(host, port_str, &hints, &this->serverUdpAddr)) !=
      0) {
    throw FatalError(std::string("Failed to get address for UDP connection: ") +
                     gai_strerror(addr_res));
  }

  // Get TCP address
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET;       // IPv4
  hints.ai_socktype = SOCK_STREAM; // TCP socket
  // check if address is valid
  if ((addr_res = getaddrinfo(host, port_str, &hints, &this->serverTcpAddr)) !=
      0) {
    throw FatalError(std::string("Failed to get address for TCP connection: ") +
                     gai_strerror(addr_res));
  }
}

void UserState::sendUdpPacket(UdpPacket &packet) {
  send_packet(packet, udpSocketFD, serverUdpAddr->ai_addr,
              serverUdpAddr->ai_addrlen);
}

void UserState::waitForUdpPacket(UdpPacket &packet) {
  wait_for_packet(packet, udpSocketFD);
}

void UserState::sendUdpPacketAndWaitForReply(UdpPacket &request,
                                             UdpPacket &response) {
  int triesLeft = UDP_RESEND_TRIES;
  while (triesLeft > 0) {
    --triesLeft;
    try {
      this->sendUdpPacket(request);
      this->waitForUdpPacket(response);
      return;
    } catch (ConnectionTimeoutException &e) {
      if (triesLeft == 0) {
        throw;
      }
    }
  }
}

void UserState::sendTcpPacketAndWaitForReply(TcpPacket &out_packet,
                                             TcpPacket &in_packet) {
  try {
    openTcpSocket();
    sendTcpPacket(out_packet);
    waitForTcpPacket(in_packet);
  } catch (...) {
    closeTcpSocket();
    throw;
  }
  closeTcpSocket();
};

void UserState::sendTcpPacket(TcpPacket &packet) {
  if (connect(tcpSocketFD, serverTcpAddr->ai_addr, serverTcpAddr->ai_addrlen) !=
      0) {
    throw ConnectionTimeoutException();
  }
  packet.send(tcpSocketFD);
}

void UserState::waitForTcpPacket(TcpPacket &packet) {
  packet.receive(tcpSocketFD);
}

void UserState::openTcpSocket() {
  if ((this->tcpSocketFD = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    throw FatalError("Failed to create a TCP socket", errno);
  }
  struct timeval read_timeout;
  read_timeout.tv_sec = TCP_READ_TIMEOUT_SECONDS;
  read_timeout.tv_usec = 0;
  if (setsockopt(this->tcpSocketFD, SOL_SOCKET, SO_RCVTIMEO, &read_timeout,
                 sizeof(read_timeout)) < 0) {
    throw FatalError("Failed to set TCP read timeout socket option", errno);
  }
  struct timeval write_timeout;
  write_timeout.tv_sec = TCP_WRITE_TIMEOUT_SECONDS;
  write_timeout.tv_usec = 0;
  if (setsockopt(this->tcpSocketFD, SOL_SOCKET, SO_SNDTIMEO, &write_timeout,
                 sizeof(write_timeout)) < 0) {
    throw FatalError("Failed to set TCP send timeout socket option", errno);
  }
}

void UserState::closeTcpSocket() {
  if (close(this->tcpSocketFD) != 0) {
    if (errno == EBADF) {
      // was already closed
      return;
    }
    throw FatalError("Failed to close TCP socket", errno);
  }
}
