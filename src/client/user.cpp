#include "user.hpp"

#include <arpa/inet.h>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

extern bool is_exiting; // flag to indicate whether the application is exiting

int main(int argc, char *argv[]) {
  try {
    setup_custom_signal_handlers();  // change the signal handlers to our own
    ClientConfig config(argc, argv); // parse command-line arguments

    if (config.help) { // if the help flag is set, print the help menu and exit
      config.printHelp(std::cout);
      return EXIT_SUCCESS;
    }

    UserState userState(config.host, config.port); // create a new user state
    CommandManager commandManager;    // create a new command manager
    registerCommands(commandManager); // register all commands with the manager

    while (!std::cin.eof() && !is_exiting) {    // while not exiting, wait for
      commandManager.waitForCommand(userState); // commands and handle them
    }

    // If it breaks out of the loop, it means that the application is exiting
    std::cout << std::endl
              << "Exiting... Press CTRL + C (again) to forcefully close "
                 "the application."
              << std::endl;

    return EXIT_SUCCESS;

  } catch (std::exception &e) { // catch any exceptions thrown
    std::cerr << "Encountered fatal error while running the "
                 "application. Shutting down..."
              << std::endl
              << e.what() << std::endl;
    return EXIT_FAILURE;

  } catch (...) { // catch any other exceptions thrown
    std::cerr << "Encountered fatal error while running the "
                 "application. Shutting down..."
              << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

void registerCommands(CommandManager &commandManager) {
  commandManager.addCommand(std::make_shared<LoginCommand>());
  commandManager.addCommand(std::make_shared<LogoutCommand>());
  commandManager.addCommand(std::make_shared<UnregisterCommand>());
  commandManager.addCommand(std::make_shared<ExitCommand>());
  commandManager.addCommand(std::make_shared<OpenAuctionCommand>());
  commandManager.addCommand(std::make_shared<CloseAuctionCommand>());
  commandManager.addCommand(std::make_shared<ListUserAuctionsCommand>());
  commandManager.addCommand(std::make_shared<ListUserBidsCommand>());
  commandManager.addCommand(std::make_shared<ListAuctionsCommand>());
  commandManager.addCommand(std::make_shared<ShowAssetCommand>());
  commandManager.addCommand(std::make_shared<BidCommand>());
  commandManager.addCommand(std::make_shared<ShowRecordCommand>());
}

ClientConfig::ClientConfig(int argc, char *argv[]) {
  this->programPath = argv[0]; // set the program path to the first argument

  // -h -n -p are valid options, and : means that they need an argument
  int opt;
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
      std::cerr << std::endl; // print a newline before printing help
      printHelp(std::cerr);
      exit(EXIT_FAILURE);
    }
  }
  validate_port_number(port); // validate the port number
}

void ClientConfig::printHelp(std::ostream &stream) {
  stream << "Usage: " << programPath << " [-n ASIP] [-p ASport] [-h]"
         << std::endl;
  stream << "Available options:" << std::endl;
  stream << "-n ASIP\t\tSet hostname of Auction Server. Default: "
         << DEFAULT_HOSTNAME << std::endl;
  stream << "-p ASport\tSet port of Auction Server. Default: " << DEFAULT_PORT
         << std::endl;
  stream << "-h\t\tPrint this menu." << std::endl;
}