#include "server.hpp"

#include <arpa/inet.h>
#include <unistd.h>

#include <iostream>
#include <string>
#include <thread>

#include "../utils/protocol.hpp"
#include "../utils/utils.hpp"

// flag to indicate whether the server is terminating
extern bool is_exiting;

int main(int argc, char *argv[]) {

  try {

    ServerConfig config(argc, argv);

    if (config.help) {
      config.printHelp(std::cout);
      return EXIT_SUCCESS;
    }

    setup_custom_signal_handlers();
    setupDB();

    AuctionServerState state(config.port, config.verbose);
    state.registerHandlers();

    state.cdebug << "Server is running on verbose mode" << std::endl;

    std::thread tcp_thread(tcpMainThread, std::ref(state));
    uint32_t ex_trial = 0;
    while (!is_exiting) {
      try {
        wait_for_udp_packet(state);
        ex_trial = 0;
      } catch (std::exception &e) {
        std::cerr << "Encountered a fatak error while running the "
                     "application. Retrying..."
                  << std::endl
                  << e.what() << std::endl;
        ex_trial++;
      } catch (...) {
        std::cerr << "Encountered a fatal error while running the "
                     "application. Retrying..."
                  << std::endl;
        ex_trial++;
      }
      if (ex_trial >= EXCEPTION_RETRY_MAX_TRIALS) {
        std::cerr << "Max trials reached, shutting down..." << std::endl;
        is_exiting = true;
      }
    }

    std::cout << "Shutting down UDP server..." << std::endl;

    tcp_thread.join();

  } catch (std::exception &e) {
    std::cerr << "Encountered a fatal error while running the "
                 "application. Shutting down..."
              << std::endl
              << e.what() << std::endl;
    return EXIT_FAILURE;
  } catch (...) {
    std::cerr << "Encountered a fatal error while running the "
                 "application. Shutting down..."
              << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

ServerConfig::ServerConfig(int argc, char *argv[]) {
  programPath = argv[0];
  int opt;

  while ((opt = getopt(argc, argv, "-p:vh")) != -1) {
    switch (opt) {
    case 'p':
      port = std::string(optarg);
      break;
    case 'h':
      help = true;
      return;
      break;
    case 'v':
      std::cout << "Verbose mode activated" << std::endl;
      verbose = true;
      break;

    default:
      std::cerr << std::endl;
      printHelp(std::cerr);
      exit(EXIT_FAILURE);
    }
  }

  validate_port_number(port);
}

void ServerConfig::printHelp(std::ostream &stream) {
  stream << "Usage: " << programPath << " [-p ASport] [-v]" << std::endl;
  stream << "Available options:" << std::endl;
  stream << "  -p ASport: Set the port number to listen on" << std::endl;
  stream << "  -v: Enable verbose logging" << std::endl;
}

void setupDB() {
  create_new_directory(ASDIR);
  create_new_directory(USERDIR);
  create_new_directory(AUCTIONDIR);

  std::string nextAuctionFile = AUCTIONDIR;
  nextAuctionFile += "/next_auction.txt";
  create_new_file(nextAuctionFile);
  write_to_file(nextAuctionFile, "1\n");
};

void wait_for_udp_packet(AuctionServerState &state) {
  SocketAddress source_addr;
  std::stringstream stream;
  char buffer[SOCKET_BUFFER_LEN];

  source_addr.size = sizeof(source_addr.addr);
  ssize_t n = recvfrom(state.udpSocketFD, buffer, SOCKET_BUFFER_LEN, 0,
                       (struct sockaddr *)&source_addr.addr, &source_addr.size);
  if (is_exiting) {
    return;
  }
  if (n == -1) {
    if (errno == EAGAIN) {
      return;
    }
    throw FatalError("Failed to receive UDP message (recvfrom)", errno);
  }
  source_addr.socket = state.udpSocketFD;

  char addr_str[INET_ADDRSTRLEN + 1] = {0};
  inet_ntop(AF_INET, &source_addr.addr.sin_addr, addr_str, INET_ADDRSTRLEN);
  std::cout << "Receiving incoming UDP message from " << addr_str << ":"
            << ntohs(source_addr.addr.sin_port) << std::endl;

  stream.write(buffer, n);

  return handle_packet(state, stream, source_addr);
}

void handle_packet(AuctionServerState &state, std::stringstream &buffer,
                   SocketAddress &source_addr) {
  try {
    char packet_id[PACKET_ID_LEN + 1];
    buffer >> packet_id;

    if (!buffer.good()) {
      std::cerr << "Received unknown packet ID" << std::endl;
      throw InvalidPacketException();
    }

    std::string packet_id_str = std::string(packet_id);

    state.callUdpPacketHandler(packet_id_str, buffer, source_addr);
  } catch (InvalidPacketException &e) {
    try {
      ErrorUdpPacket error;
      send_packet(error, source_addr.socket,
                  (struct sockaddr *)&source_addr.addr, source_addr.size);
    } catch (std::exception &ex) {
      std::cerr << "Failed to reply with ERR packet: " << ex.what()
                << std::endl;
    }
  } catch (std::exception &e) {
    std::cerr << "Failed to handle UDP packet: " << e.what() << std::endl;
  } catch (...) {
    std::cerr << "Failed to handle UDP packet: unknown" << std::endl;
  }
}

void tcpMainThread(AuctionServerState &state) {
  TcpWorkerPool pool(state);

  if (listen(state.tcpSocketFD, TCP_MAX_CONNECTIONS) < 0) {
    perror("Error while executing listen");
    std::cerr << "TCP server is being shutdown..." << std::endl;
    is_exiting = true;
    return;
  }

  uint32_t ex_trial = 0;
  while (!is_exiting) {
    try {
      wait_for_tcp_packet(state, pool);
      ex_trial = 0;
    } catch (std::exception &e) {
      std::cerr << "Encountered fatal error while running the "
                   "application. Retrying..."
                << std::endl
                << e.what() << std::endl;
      ex_trial++;
    } catch (...) {
      std::cerr << "Encountered fatal error while running the "
                   "application. Retrying..."
                << std::endl;
      ex_trial++;
    }
    if (ex_trial >= EXCEPTION_RETRY_MAX_TRIALS) {
      std::cerr << "Max attempts, shutting down..." << std::endl;
      is_exiting = true;
    }
  }

  std::cout << "Shutting down TCP server..." << std::endl;
}

void wait_for_tcp_packet(AuctionServerState &state, TcpWorkerPool &pool) {
  SocketAddress source_addr;

  source_addr.size = sizeof(source_addr.addr);
  int connection_fd =
      accept(state.tcpSocketFD, (struct sockaddr *)&source_addr.addr,
             &source_addr.size);
  if (is_exiting) {
    return;
  }
  if (connection_fd < 0) {
    if (errno == EAGAIN) { // timeout, just go around and keep listening
      return;
    }
    throw FatalError("[ERROR] Failed to accept a connection", errno);
  }

  struct timeval read_timeout;
  read_timeout.tv_sec = TCP_READ_TIMEOUT_SECONDS;
  read_timeout.tv_usec = 0;
  if (setsockopt(connection_fd, SOL_SOCKET, SO_RCVTIMEO, &read_timeout,
                 sizeof(read_timeout)) < 0) {
    throw FatalError("Failed to set TCP read timeout socket option", errno);
  }

  char addr_str[INET_ADDRSTRLEN + 1] = {0};
  inet_ntop(AF_INET, &source_addr.addr.sin_addr, addr_str, INET_ADDRSTRLEN);
  std::cout << "Receiving incoming TCP connection from " << addr_str << ":"
            << ntohs(source_addr.addr.sin_port) << std::endl;

  try {
    pool.giveConnection(connection_fd);
  } catch (std::exception &e) {
    close(connection_fd);
    throw FatalError(std::string("Failed to give connection to worker: ") +
                     e.what() + "\nClosing connection.");
  }
}