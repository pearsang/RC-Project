#include "server.hpp"

#include <arpa/inet.h>
#include <filesystem>
#include <iostream>
#include <unistd.h>

#include <iostream>
#include <string>
#include <thread>

extern bool is_exiting; // flag to indicate whether the application is exiting

int main(int argc, char *argv[]) {

  try {

    ServerConfig config(argc, argv); // parse command-line arguments

    if (config.help) { // if the help flag is set, print the help menu and exit
      config.printHelp(std::cout);
      return EXIT_SUCCESS;
    }

    setup_custom_signal_handlers(); // change the signal handlers to our own
    setupDB();                      // setup the database

    AuctionServerState serverState(config.port, config.verbose);
    serverState.registerHandlers(); // register all handlers with the manager

    serverState.verbose << "Server is running on verbose mode" << std::endl;

    // start the TCP thread
    std::thread tcp_thread(tcpMainThread, std::ref(serverState));
    uint32_t ex_trial = 0; // exception trial counter
    while (!is_exiting) { // while not exiting, wait for packets and handle them
      try {
        wait_for_udp_packet(serverState);
        ex_trial = 0; // reset the exception trial counter
      } catch (std::exception &e) {
        std::cerr << "Encountered a fatal error while running the "
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
      if (ex_trial >= EXCEPTION_RETRY_MAX_TRIALS) { // if max trials reached
        std::cerr << "Max trials reached, shutting down..." << std::endl;
        is_exiting = true; // set the exiting flag
      }
    }

    std::cout << "Shutting down UDP server..." << std::endl;

    tcp_thread.join(); // wait for the TCP thread to finish

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
  // -p -v -h are valid options, and : means that they need an argument
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
      std::cerr << std::endl; // print a newline before printing help
      printHelp(std::cerr);   // print the help menu
      exit(EXIT_FAILURE);
    }
  }

  validate_port_number(port); // validate the port number
}

void ServerConfig::printHelp(std::ostream &stream) {
  stream << "Usage: " << programPath << " [-p ASport] [-v]" << std::endl;
  stream << "Available options:" << std::endl;
  stream << "  -p ASport: Set the port number to listen on" << std::endl;
  stream << "  -v: Enable verbose logging" << std::endl;
}

void setupDB() {
  create_new_directory(AS_DIR);      // create the AS directory
  create_new_directory(USER_DIR);    // create the user directory
  create_new_directory(AUCTION_DIR); // create the auction directory

  std::string nextAuctionFile = AUCTION_DIR + SLASH + NEXT_AUCTION_FILE;
  create_new_file(AUCTION_DIR + SLASH + NEXT_AUCTION_FILE);

  int count = 0; // count the number of auctions that exist
  for (const auto &entry : std::filesystem::directory_iterator(AUCTION_DIR)) {
    if (std::filesystem::is_directory(entry.path())) { // if it's a directory
      count++;                                         // increment the count
    }
  }
  count++; // increment the count for the next auction

  std::string numAuctions = std::to_string(count) + "\n";
  write_to_file(nextAuctionFile, numAuctions); // for the next auction id
};

void wait_for_udp_packet(AuctionServerState &serverState) {
  SocketAddress sourceAddr;       // the source address of the packet
  std::stringstream stream;       // the stream to read the packet into
  char buffer[SOCKET_BUFFER_LEN]; // the buffer to read the packet into

  sourceAddr.size = sizeof(sourceAddr.addr); // set the size of the address

  // receive the packet into the buffer, and store the number of bytes read
  ssize_t n = recvfrom(serverState.udpSocketFD, buffer, SOCKET_BUFFER_LEN, 0,
                       (struct sockaddr *)&sourceAddr.addr, &sourceAddr.size);

  if (is_exiting) { // if the application is exiting, return
    return;
  }

  if (n == -1) {           // if the number of bytes read is -1, caught an error
    if (errno == EAGAIN) { // if the error is EAGAIN, just go around
      return;
    }
    throw FatalError("Failed to receive UDP message (recvfrom)", errno);
  }

  sourceAddr.socket =
      serverState.udpSocketFD; // set the socket of the source address

  // convert the source address to a string
  char addr_str[INET_ADDRSTRLEN + 1] = {0};

  // inet_ntop converts the address from binary to text form (IPV4)
  inet_ntop(AF_INET, &sourceAddr.addr.sin_addr, addr_str, INET_ADDRSTRLEN);

  std::cout << "Receiving incoming UDP message from " << addr_str << ":"
            << ntohs(sourceAddr.addr.sin_port) << std::endl;

  // write the buffer into the stream
  stream.write(buffer, n);

  return handle_packet(serverState, stream, sourceAddr);
}

void handle_packet(AuctionServerState &serverState, std::stringstream &buffer,
                   SocketAddress &sourceAddr) {
  try {
    char _packetID[PACKET_ID_LEN + 1]; // the packet id
    buffer >> _packetID;               // read the packet id from the stream

    if (!buffer.good()) { // if the stream is not good, throw an exception
      std::cerr << "Received unknown packet ID" << std::endl;
      throw InvalidPacketException();
    }

    std::string packetID = std::string(_packetID); // convert to string

    serverState.callUdpPacketHandler(packetID, buffer, sourceAddr);

  } catch (InvalidPacketException &e) {
    try {
      ErrorUdpPacket error; // create an error packet
      // set the error message, and the source address, and send the packet
      send_packet(error, sourceAddr.socket, (struct sockaddr *)&sourceAddr.addr,
                  sourceAddr.size);
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

void tcpMainThread(AuctionServerState &serverState) {
  // We create a pool of threads to handle the TCP connections
  TcpWorkerPool pool(serverState);

  // We create a socket for the TCP server, and set the socket options
  if (listen(serverState.tcpSocketFD, TCP_MAX_CONNECTIONS) < 0) {
    perror("Error while executing listen");
    std::cerr << "TCP server is being shutdown..." << std::endl;
    is_exiting = true;
    return;
  }

  uint32_t ex_trial = 0; // exception trial counter
  while (!is_exiting) {  // while not exiting, wait for packets and handle them
    try {
      wait_for_tcp_packet(serverState, pool);
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
    if (ex_trial >= EXCEPTION_RETRY_MAX_TRIALS) { // if max trials reached
      std::cerr << "Max attempts, shutting down..." << std::endl;
      is_exiting = true; // set the exiting flag
    }
  }

  std::cout << "Shutting down TCP server..." << std::endl;
}

void wait_for_tcp_packet(AuctionServerState &serverState, TcpWorkerPool &pool) {
  SocketAddress sourceAddr; // the source address of the packet

  sourceAddr.size = sizeof(sourceAddr.addr); // set the size of the address
  // accept a connection, and store the file descriptor of the connection
  int connection_fd =
      accept(serverState.tcpSocketFD, (struct sockaddr *)&sourceAddr.addr,
             &sourceAddr.size);
  if (is_exiting) {
    return;
  }
  if (connection_fd < 0) {
    if (errno == EAGAIN) { // timeout, just go around and keep listening
      return;
    }
    throw FatalError("[ERROR] Failed to accept a connection", errno);
  }

  struct timeval read_timeout;                    // set the read timeout
  read_timeout.tv_sec = TCP_READ_TIMEOUT_SECONDS; // set the timeout
  read_timeout.tv_usec = 0;

  // set the read timeout socket option for the connection
  if (setsockopt(connection_fd, SOL_SOCKET, SO_RCVTIMEO, &read_timeout,
                 sizeof(read_timeout)) < 0) {
    throw FatalError("Failed to set TCP read timeout socket option", errno);
  }

  char addr_str[INET_ADDRSTRLEN + 1] = {0}; // convert the address to string

  // inet_ntop converts the address from binary to text form (IPV4)
  inet_ntop(AF_INET, &sourceAddr.addr.sin_addr, addr_str, INET_ADDRSTRLEN);
  std::cout << "Receiving incoming TCP connection from " << addr_str << ":"
            << ntohs(sourceAddr.addr.sin_port) << std::endl;

  try {
    pool.giveConnection(connection_fd); // give the connection to a worker
  } catch (std::exception &e) {
    close(connection_fd);
    throw FatalError(std::string("Failed to give connection to worker: ") +
                     e.what() + "\nClosing connection.");
  }
}