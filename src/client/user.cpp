#include "user.hpp"

#include "../utils/utils.hpp"
#include "commands.hpp"
#include "user_state.hpp"
#include <arpa/inet.h>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

extern bool is_exiting;

int main(int argc, char *argv[]) {
  try {
    setup_custom_signal_handlers();

    ClientConfig config(argc, argv);
    if (config.help) {
      config.printHelp(std::cout);
      return EXIT_SUCCESS;
    }

    UserState userState(config.host, config.port);

    CommandManager commandManager;
    registerCommands(commandManager);

    while (!std::cin.eof() && !is_exiting) {
      commandManager.waitForCommand(userState);
    }

    std::cout << std::endl
              << "Exiting... Press CTRL + C (again) to forcefully close "
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
  this->program_path = argv[0];
  // REMOVE UNECESSARY FLAGS
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