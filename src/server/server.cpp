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
      verbose = true;
      break;
    case 1:
      if (wordFilePath.empty()) {
        wordFilePath = std::string(optarg);
      }
      break;
    default:
      std::cerr << std::endl;
      printHelp(std::cerr);
      exit(EXIT_FAILURE);
    }
  }

  if (wordFilePath.empty()) {
    std::cerr << programPath << ": required argument 'word_file' not provided"
              << std::endl
              << std::endl;
    printHelp(std::cerr);
    exit(EXIT_FAILURE);
  }

  validate_port_number(port);
}

void ServerConfig::printHelp(std::ostream &stream) {
  stream << "Usage: " << programPath << " [-p ASport] [-v]" << std::endl;
  stream << "Available options:" << std::endl;
  stream << "  -p ASport: Set the port number to listen on" << std::endl;
  stream << "  -v: Enable verbose logging" << std::endl;
}