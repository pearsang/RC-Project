#ifndef SERVER_STATE_H
#define SERVER_STATE_H

#include <filesystem>
#include <mutex>
#include <netdb.h>
#include <optional>
#include <stdexcept>
#include <unordered_map>
#include <vector>

#include "../utils/constants.hpp"
#include "../utils/utils.hpp"

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

typedef void (*UdpPacketHandler)(std::stringstream &, SocketAddress &,
                                 AuctionServerState &);
typedef void (*TcpPacketHandler)(int fd, AuctionServerState &);

/**
 * @class ServerState
 *
 * @brief Represents the state of the server.
 *
 * The ServerState class encapsulates the state of the server, including the
 * server's port, word file path, and a map of usernames to passwords.
 */
class AuctionServerState {
  int udpSocketFD = -1;
  int tcpSocketFD = -1;
  struct addrinfo *serverUdpAddr = NULL;
  struct addrinfo *serverTcpAddr = NULL;

  // Maps the command string to the handler type
  std::unordered_map<std::string, UdpPacketHandler> UdpPacketHandlers;
  std::unordered_map<std::string, TcpPacketHandler> TcpPacketHandlers;

public:
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
};

#endif
