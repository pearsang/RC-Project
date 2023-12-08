#ifndef SERVER_H
#define SERVER_H

#include <csignal>

#include "../utils/constants.hpp"
#include "../utils/utils.hpp"

class ServerConfig {
public:
  char *programPath;
  std::string wordFilePath;
  std::string port = DEFAULT_PORT;
  bool help = false;
  bool verbose = false;

  ServerConfig(int argc, char *argv[]);
  void printHelp(std::ostream &stream);
};



#endif