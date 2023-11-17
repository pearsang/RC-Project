#include "commands.hpp"

#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>

void CommandManager::waitForCommand() {
  std::cout << "> ";

  std::string line;
  std::getline(std::cin, line);

  if (std::cin.eof()) {
    return;
  }

  // Split the line into the command name and the arguments
  auto splitIndex = line.find(' ');

  std::string commandName;
  // If there is no space, then the entire line is the command name
  if (splitIndex == std::string::npos) {
    commandName = line;
    line = "";
  } else {
    commandName = line.substr(0, splitIndex);
    line.erase(0, splitIndex + 1);
  }

  if (commandName.length() == 0) {
    return;
  }

  std::cout << "Command: " << commandName << std::endl;
  std ::cout << "Arguments: " << line << std::endl;

}
