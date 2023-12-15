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
  // Create a UDP socket, AF_INET for IPv4, SOCK_DGRAM for UDP, 0 for IP
  if ((this->udpSocketFD = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
    throw FatalError("Failed to create a UDP socket", errno);
  }
}

void UserState::resolveServerAddress(std::string &hostname, std::string &port) {
  struct addrinfo hints;               // address info hints
  int addr_res;                        // address resolution result
  const char *host = hostname.c_str(); // host string
  const char *port_str = port.c_str(); // port string

  memset(&hints, 0, sizeof hints); // clear hints
  hints.ai_family = AF_INET;       // IPv4
  hints.ai_socktype = SOCK_DGRAM;  // UDP socket

  // check if address is valid
  if ((addr_res = getaddrinfo(host, port_str, &hints, &this->serverUdpAddr)) !=
      0) {
    throw FatalError(std::string("Failed to get address for UDP connection: ") +
                     gai_strerror(addr_res));
  }

  memset(&hints, 0, sizeof hints); // clear hints
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
  int triesLeft = UDP_RESEND_TRIES; // number of tries left
  while (triesLeft > 0) {
    --triesLeft;
    try {
      this->sendUdpPacket(request);
      this->waitForUdpPacket(response);
      return;
    } catch (ConnectionTimeoutException &e) { // timeout
      if (triesLeft == 0) {                   // no more tries left
        throw;
      }
    }
  }
}

void UserState::sendTcpPacketAndWaitForReply(TcpPacket &out_packet,
                                             TcpPacket &in_packet) {
  // NOTE: We only want the socket to be open while sending the packet
  try {
    openTcpSocket();
    sendTcpPacket(out_packet);
    waitForTcpPacket(in_packet);
  } catch (...) {
    closeTcpSocket(); // close socket on error
    throw;
  }
  closeTcpSocket(); // close socket on success
};

void UserState::sendTcpPacket(TcpPacket &packet) {
  // Connect to the server using the TCP socket
  if (connect(tcpSocketFD, serverTcpAddr->ai_addr, serverTcpAddr->ai_addrlen) !=
      0) {
    throw ConnectionTimeoutException(); // timeout
  }
  packet.send(tcpSocketFD);
}

void UserState::waitForTcpPacket(TcpPacket &packet) {
  packet.receive(tcpSocketFD);
}

void UserState::openTcpSocket() {
  // Create a TCP socket, AF_INET for IPv4, SOCK_STREAM for TCP, 0 for IP
  if ((this->tcpSocketFD = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    throw FatalError("Failed to create a TCP socket", errno);
  }

  struct timeval read_timeout;                    // timeout on read operations
  read_timeout.tv_sec = TCP_READ_TIMEOUT_SECONDS; // seconds to wait
  read_timeout.tv_usec = 0;                       // microseconds to wait

  // Set socket options, SOL_SOCKET for socket-level options, SO_RCVTIMEO for a
  // timeout on read operations
  if (setsockopt(this->tcpSocketFD, SOL_SOCKET, SO_RCVTIMEO, &read_timeout,
                 sizeof(read_timeout)) < 0) {
    throw FatalError("Failed to set TCP read timeout socket option", errno);
  }

  struct timeval write_timeout; // timeout on write operations
  write_timeout.tv_sec = TCP_WRITE_TIMEOUT_SECONDS; // seconds to wait
  write_timeout.tv_usec = 0;                        // microseconds to wait

  // Set socket options, SOL_SOCKET for socket-level options, SO_SNDTIMEO for a
  // timeout on write operations
  if (setsockopt(this->tcpSocketFD, SOL_SOCKET, SO_SNDTIMEO, &write_timeout,
                 sizeof(write_timeout)) < 0) {
    throw FatalError("Failed to set TCP send timeout socket option", errno);
  }
}

void UserState::closeTcpSocket() {
  if (close(this->tcpSocketFD) != 0) {
    if (errno == EBADF) { // invalid file descriptor, it was already closed
      return;
    }
    throw FatalError("Failed to close TCP socket", errno);
  }
}
