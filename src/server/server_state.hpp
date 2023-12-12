#ifndef SERVER_STATE_H
#define SERVER_STATE_H

#include <filesystem>
#include <iostream>
#include <mutex>
#include <netdb.h>
#include <optional>
#include <stdexcept>
#include <unordered_map>
#include <vector>

#include "server_auction.hpp"
#include "server_user.hpp"

class DebugStream {
  bool active;

public:
  DebugStream(bool __active) : active{__active} {};

  template <class T> DebugStream &operator<<(T val) {
    if (active) {
      std::cout << val;
    }
    return *this;
  }

  DebugStream &operator<<(std::ostream &(*f)(std::ostream &)) {
    if (active) {
      f(std::cout);
    }
    return *this;
  }

  DebugStream &operator<<(std::ostream &(*f)(std::ios &)) {
    if (active) {
      f(std::cout);
    }
    return *this;
  }

  DebugStream &operator<<(std::ostream &(*f)(std::ios_base &)) {
    if (active) {
      f(std::cout);
    }
    return *this;
  }
};

class AuctionServerState;

/**
 * @class SocketAddress
 *
 * @brief Represents a socket address.
 */
class SocketAddress {
public:
  int socket;
  struct sockaddr_in addr;
  socklen_t size;
};

typedef void (*UdpPacketHandler)(AuctionServerState &, std::stringstream &,
                                 SocketAddress &);
typedef void (*TcpPacketHandler)(AuctionServerState &, int fd);

/**
 * @class ServerState
 *
 * @brief Represents the state of the server.
 *
 * The ServerState class encapsulates the state of the server, including the
 * server's port, word file path, and a map of usernames to passwords.
 */
class AuctionServerState {

  // Maps the command string to the handler type
  std::unordered_map<std::string, UdpPacketHandler> UdpPacketHandlers;
  std::unordered_map<std::string, TcpPacketHandler> TcpPacketHandlers;

public:
  int udpSocketFD = -1;
  int tcpSocketFD = -1;
  struct addrinfo *serverUdpAddr = NULL;
  struct addrinfo *serverTcpAddr = NULL;
  DebugStream cdebug;
  UserManager usersManager;
  AuctionManager auctionManager;

  AuctionServerState(std::string &port, bool verbose);

  ~AuctionServerState();
  /**
   * @brief Sets up a UDP socket.
   *
   */
  void setupUdpSocket();

  /**
   * @brief Sets up a TCP socket.
   *
   */
  void setupTcpSocket();

  /**
   * @brief Resolves the incoming address.
   *
   * @param port The port of the server.
   */
  void resolveServerAddress(std::string &port);

  /**
   * @brief Registers all server handlers
   */
  void registerHandlers();

  /**
   * @brief Registers a UDP packet handler with the server.
   */
  void registerUdpPacketHandlers();

  /**
   * @brief Registers a TCP packet handler with the server.
   */
  void registerTcpPacketHandlers();

  void callUdpPacketHandler(std::string packet_id, std::stringstream &stream,
                            SocketAddress &addr_from);

  void callTcpPacketHandler(std::string packet_id, int fd);

  uint8_t existsDB();
};

#endif
