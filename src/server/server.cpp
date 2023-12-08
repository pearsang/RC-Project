#include "server.hpp"

#include <arpa/inet.h>
#include <unistd.h>

#include <iostream>
#include <thread>

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

    // TODO: print verbose mode activated if so

    while (!is_exiting) {
    }

  } catch (...) {
    std::cerr << "An error has ocurred while running the application. "
                 "Shutting down..."
              << std::endl;

    return EXIT_FAILURE;
  }

  return 0;
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
};
