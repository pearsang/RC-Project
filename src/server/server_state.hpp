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
#include "verbose_stream.hpp"

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
  VerboseStream verbose;
  UserManager usersManager;
  AuctionManager auctionManager;

  AuctionServerState(std::string &port, bool _verbose);

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

  /**
   * @brief Calls the handler for the given UDP packet.
   *
   * @param packet_id  The packet ID.
   * @param stream  The packet stream.
   * @param addr_from  The address the packet came from.
   */
  void callUdpPacketHandler(std::string packet_id, std::stringstream &stream,
                            SocketAddress &addr_from);

  /**
   * @brief Calls the handler for the given TCP packet.
   *
   * @param packet_id  The packet ID.
   * @param fd  The file descriptor the packet came from
   */
  void callTcpPacketHandler(std::string packet_id, int fd);
};
#endif
