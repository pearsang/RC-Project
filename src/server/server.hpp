#ifndef SERVER_H
#define SERVER_H

#include <csignal>
#include <filesystem>

#include "../utils/constants.hpp"
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
 */
void wait_for_udp_packet(AuctionServerState &state);

void tcpMainThread(AuctionServerState &state);

void handle_packet(AuctionServerState &state, std::stringstream &buffer,
                   SocketAddress &source_addr);

void handle_packet(std::stringstream &buffer, SocketAddress &source_addr,
                   AuctionServerState &state);

void wait_for_tcp_packet(AuctionServerState &state, TcpWorkerPool &pool);

void setupDB();

#endif