#ifndef SERVER_H
#define SERVER_H

#include <csignal>
#include <filesystem>

#include "../utils/constants.hpp"
#include "../utils/protocol.hpp"
#include "../utils/utils.hpp"
#include "server_state.hpp"
#include "tcp_worker_pool.hpp"

class ServerConfig {
public:
  char *programPath;
  std::string port = DEFAULT_PORT;
  bool help = false;
  bool verbose = false;

  ServerConfig(int argc, char *argv[]);
  void printHelp(std::ostream &stream);
};

/**
 * @brief Waits for an UDP packet to arrive.
 *
 * @param serverState The server state.
 *
 */
void wait_for_udp_packet(AuctionServerState &serverState);

/**
 * @brief Handles an UDP packet.
 *
 * @param serverState The server state.
 *
 */
void tcpMainThread(AuctionServerState &serverState);

/**
 * @brief Handles an UDP packet.
 *
 * @param serverState The server state.
 * @param buffer The buffer containing the packet.
 * @param source_addr The source address of the packet.
 *
 */
void handle_packet(AuctionServerState &serverState, std::stringstream &buffer,
                   SocketAddress &source_addr);

/**
 * @brief Waits for a TCP packet to arrive.
 *
 * @param serverState The server state.
 * @param pool The TCP worker pool to use.
 */
void wait_for_tcp_packet(AuctionServerState &serverState, TcpWorkerPool &pool);

void setupDB();

#endif