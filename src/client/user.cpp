#include "user.hpp"

#include "../utils/utils.hpp"
#include "commands.hpp"
#include <arpa/inet.h>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  try {
    setup_custom_signal_handlers();

    ClientConfig config(argc, argv);
    if (config.help) {
      config.printHelp(std::cout);
      return EXIT_SUCCESS;
    }

    CommandManager commandManager;

    while (!std::cin.eof()) {
      commandManager.waitForCommand();
    }

    std::cout << std::endl
              << "Shutting down... Press CTRL + C (again) to forcefully close "
                 "the application."
              << std::endl;

    return EXIT_SUCCESS;

  } catch (std::exception &e) {
    std::cerr << "Encountered fatal error while running the "
                 "application. Shutting down..."
              << std::endl
              << e.what() << std::endl;
    return EXIT_FAILURE;

  } catch (...) {
    std::cerr << "Encountered fatal error while running the "
                 "application. Shutting down..."
              << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

ClientConfig::ClientConfig(int argc, char *argv[]) {
  this->program_path = argv[0];
  int opt;
  // hn:p: means that -h, -n, -p are valid options, and : means that they need
  // an argument
  while ((opt = getopt(argc, argv, "hn:p:")) != -1) {
    switch (opt) {
    case 'h':
      this->help = true;
      break;
    case 'n':
      this->host = std::string(optarg);
      break;
    case 'p':
      this->port = std::string(optarg);
      break;
    default:
      // If an invalid option is provided, print the help menu and exit
      std::cerr << std::endl;
      printHelp(std::cerr);
      exit(EXIT_FAILURE);
    }
  }
  validate_port_number(port);
}

void ClientConfig::printHelp(std::ostream &stream) {
  stream << "Usage: " << program_path << " [-n ASIP] [-p ASport] [-h]"
         << std::endl;
  stream << "Available options:" << std::endl;
  stream << "-n ASIP\t\tSet hostname of Auction Server. Default: "
         << DEFAULT_HOSTNAME << std::endl;
  stream << "-p ASport\tSet port of Auction Server. Default: " << DEFAULT_PORT
         << std::endl;
  stream << "-h\t\tPrint this menu." << std::endl;
}