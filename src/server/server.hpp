#ifndef SERVER_H
#define SERVER_H

#include <csignal>

#include "../utils/constants.hpp"
#include "../utils/utils.hpp"
#include "handlers.hpp"
#include "server_state.hpp"

#include "../utils/protocol.hpp"

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

void handle_packet(AuctionServerState &state, std::stringstream &buffer,
                   SocketAddress &source_addr);

void setupDB();
#endif