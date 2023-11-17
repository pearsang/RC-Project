#ifndef USER_H
#define USER_H

#include <csignal>
#include <iostream>
#include "../utils/constants.hpp"

/**
 * @class ClientConfig
 *
 * @brief Represents configuration options for a client application.
 *
 * The ClientConfig class encapsulates parameters such as the program path,
 * host, port, and a flag indicating whether to display help information.
 */
class ClientConfig {
 public:
  char* program_path;
  std::string host = DEFAULT_HOSTNAME;
  std::string port = DEFAULT_PORT;
  bool help = false;

  /**
   * @brief Constructs a new ClientConfig object.
   *
   * @param argc The number of command-line arguments.
   * @param argv The command-line arguments.
   */
  ClientConfig(int argc, char* argv[]);

  /**
   * @brief Prints the help menu.
   *
   * @param stream The output stream to print to.
   */
  void printHelp(std::ostream& stream);
};


#endif