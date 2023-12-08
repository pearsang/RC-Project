#include "server.hpp"

#include <arpa/inet.h>
#include <unistd.h>

#include <iostream>
#include <thread>


int main(){
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