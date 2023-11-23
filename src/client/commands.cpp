#include "commands.hpp"

#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>

void CommandManager::addCommand(std::shared_ptr<CommandHandler> handler) {
  // Add the handler to the list of handlers
  handlerList.push_back(handler);

  // Add the handler to the map of handlers
  handlers[handler->name] = handler;

  // If the handler has an alias, add the alias to the map of handlers
  if (handler->alias.has_value()) {
    handlers[*handler->alias] = handler;
  }
}

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

  // Find the command handler for the given command name
  auto handler = handlers.find(commandName);
  if (handler == handlers.end()) {
    std::cout << "Invalid command: " << commandName << std::endl;
    return;
  }

  std::cout << "Command: " << commandName << std::endl;
  std ::cout << "Arguments: " << line << std::endl;

  try {
    // Perform the command
    handler->second->handleCommand(line);
  } catch (std::exception &e) {
    std::cout << "Error: " << e.what() << std::endl;
  }
}

void LoginCommand::handleCommand(std::string args) {
  // parse args
  std::string user_id, password;
  std::vector<std::string> params = parse_args(args);

  if (params.size() != 2) {
    std::cout << "Invalid number of arguments: Expected 2, got "
              << params.size() << std::endl;
    return;
  }

  user_id = params[0];
  if (validateUserID(user_id) == INVALID) {
    std::cout << "Invalid user ID: Must be a positive 6 digit number"
              << std::endl;
    return;
  }

  password = params[1];
  if (validatePassword(password) == INVALID) {
    std::cout << "Invalid password: Must be alphanumeric and 8 characters long"
              << std::endl;
    return;
  }

  std::string message = "LIN " + user_id + " " + password;

  std::cout << message << std::endl;
}

void LogoutCommand::handleCommand(std::string args) {
  std::cout << "Logout command" << args << std::endl;
}

void UnregisterCommand::handleCommand(std::string args) {

  std::cout << "Unregister command" << args << std::endl;
}

void ExitCommand::handleCommand(std::string args) {

  std::cout << "Exit command" << args << std::endl;
}

void OpenAuctionCommand::handleCommand(std::string args) {

  std::cout << "Open command" << args << std::endl;
}

void CloseAuctionCommand::handleCommand(std::string args) {
  std::string auction_id;
  std::vector<std::string> params = parse_args(args);

  if (params.size() != 1) {
    std::cout << "Invalid number of arguments: Expected 1, got "
              << params.size() << std::endl;
    return;
  }

  auction_id = params[0];

  if (validateAuctionID(auction_id) == INVALID) {
    std::cout << "Invalid auction ID" << std::endl;
    return;
  }

  std::string message = "CLS " + auction_id;

  std::cout << message << std::endl;

  std::cout << "Close command" << args << std::endl;
}

void ListUserAuctionsCommand::handleCommand(std::string args) {

  std::cout << "List user auctions command" << args << std::endl;
}

void ListUserBidsCommand::handleCommand(std::string args) {

  std::cout << "List user bids command" << args << std::endl;
}

void ListAuctionsCommand::handleCommand(std::string args) {

  std::cout << "List auctions command" << args << std::endl;
}

void ShowAssetCommand::handleCommand(std::string args) {
  std::string auction_id;
  std::vector<std::string> params = parse_args(args);

  if (params.size() != 1) {
    std::cout << "Invalid number of arguments: Expected 1, got "
              << params.size() << std::endl;
    return;
  }

  auction_id = params[0];

  if (validateAuctionID(auction_id) == INVALID) {
    std::cout << "Invalid auction ID" << std::endl;
    return;
  }

  std::string message = "SAS " + auction_id;

  std::cout << message << std::endl;

  std::cout << "Show asset command" << args << std::endl;
}

void BidCommand::handleCommand(std::string args) {
  std::string auction_id;
  std::string bid_value;
  std::vector<std::string> params = parse_args(args);

  if (params.size() != 2) {
    std::cout << "Invalid number of arguments: Expected 2, got "
              << params.size() << std::endl;
    return;
  }

  auction_id = params[0];
  bid_value = params[1];

  if (validateAuctionID(auction_id) == INVALID) {
    std::cout << "Invalid auction ID" << std::endl;
    return;
  }

  if (validateBidValue(bid_value) == INVALID) {
    std::cout << "Invalid bid value: Must be a positive number" << std::endl;
    return;
  }

  std::string message = "BID " + auction_id + " " + bid_value;

  std::cout << message << std::endl;

  std::cout << "Bid command" << args << std::endl;
}

void ShowRecordCommand::handleCommand(std::string args) {
  std::string auction_id;
  std::vector<std::string> params = parse_args(args);

  if (params.size() != 1) {
    std::cout << "Invalid number of arguments: Expected 1, got "
              << params.size() << std::endl;
    return;
  }

  auction_id = params[0];

  if (validateAuctionID(auction_id) == INVALID) {
    std::cout << "Invalid auction ID" << std::endl;
    return;
  }

  std::string message = "SRC " + auction_id;

  std::cout << message << std::endl;

  std::cout << "Show record command" << args << std::endl;
}

int8_t validateUserID(std::string userID) {
  if (!is_digits(userID) || userID.length() != USER_ID_LENGTH) {
    return INVALID;
  }

  uint32_t id = (uint32_t)std::stoi(userID);
  if (id > USER_ID_MAX) {
    return INVALID;
  }

  return 0;
}

int8_t validatePassword(std::string password) {

  if (password.length() != 8 || !is_alphanumeric(password)) {
    return INVALID;
  }

  return 0;
}

int8_t validateAuctionID(std::string auctionID) {
  if (!is_digits(auctionID) || auctionID.length() != AUCTION_ID_LENGTH) {
    return INVALID;
  }

  return 0;
}

int8_t validateBidValue(std::string bidValue) {
  if (!is_digits(bidValue)) {
    return INVALID;
  }

  return 0;
}
