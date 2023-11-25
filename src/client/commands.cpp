#include "commands.hpp"
#include "./utils/protocol.hpp"

#include "../utils/utils.hpp"
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>

extern bool is_shutting_down;

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

void CommandManager::waitForCommand(UserState &state) {
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

  try {
    // Perform the command
    handler->second->handleCommand(line, state);
  } catch (std::exception &e) {
    std::cout << "Error: " << e.what() << std::endl;
  }
}

void LoginCommand::handleCommand(std::string args, UserState &state) {
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
  LoginRequest loginRequest;
  loginRequest.userID = user_id;
  loginRequest.password = password;

  LoginResponse loginResponse;
  state.sendUdpPacketAndWaitForReply(loginRequest, loginResponse);

  if (loginResponse.status == LoginResponse::status::OK) {
    std::cout << "Login successful!" << std::endl;
    state.setUserID(user_id);
    state.setPassword(password);
  } else if (loginResponse.status == LoginResponse::status::NOK) {
    std::cout << "Login failed: Incorrect password" << std::endl;
  } else if (loginResponse.status == LoginResponse::status::REG) {
    std::cout << "Login successful: You were registred" << std::endl;
  } else if (loginResponse.status == LoginResponse::status::ERR) {
    std::cout << "Login failed: Server error" << std::endl;
  }
}

void LogoutCommand::handleCommand(std::string args, UserState &state) {
  LogoutRequest logoutRequest;
  logoutRequest.userID = state.getUserID();
  logoutRequest.password = state.getPassword();

  LogoutResponse logoutResponse;
  state.sendUdpPacketAndWaitForReply(logoutRequest, logoutResponse);

  if (logoutResponse.status == LogoutResponse::status::OK) {
    std::cout << "Logout successful!" << std::endl;
    std::string empty = " ";
    state.setUserID(empty);
    state.setPassword(empty);
  } else if (logoutResponse.status == LogoutResponse::status::NOK) {
    std::cout << "Logout failed: You are not logged in" << std::endl;
  } else if (logoutResponse.status == LogoutResponse::status::UNR) {
    std::cout << "Logout failed: You are not registered" << std::endl;
  } else if (logoutResponse.status == LogoutResponse::status::ERR) {
    std::cout << "Logout failed: Server error" << std::endl;
  }
}

void UnregisterCommand::handleCommand(std::string args, UserState &state) {
  UnregisterRequest unregisterRequest;
  unregisterRequest.userID = state.getUserID();
  unregisterRequest.password = state.getPassword();

  UnregisterResponse unregisterResponse;
  state.sendUdpPacketAndWaitForReply(unregisterRequest, unregisterResponse);

  if (unregisterResponse.status == UnregisterResponse::status::OK) {
    std::cout << "Unregister successful!" << std::endl;
    std::string empty = " ";
    state.setUserID(empty);
    state.setPassword(empty);
  } else if (unregisterResponse.status == UnregisterResponse::status::NOK) {
    std::cout << "Unregister failed: You are not logged in" << std::endl;
  } else if (unregisterResponse.status == UnregisterResponse::status::UNR) {
    std::cout << "Unregister failed: You are not registered" << std::endl;
  } else if (unregisterResponse.status == UnregisterResponse::status::ERR) {
    std::cout << "Unregister failed: Server error" << std::endl;
  }
}

void ExitCommand::handleCommand(std::string args, UserState &state) {

  std::cout << "Exit command" << args << std::endl;
}

void OpenAuctionCommand::handleCommand(std::string args, UserState &state) {

  std::cout << "Open command" << args << std::endl;
}

void CloseAuctionCommand::handleCommand(std::string args, UserState &state) {
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

void ListUserAuctionsCommand::handleCommand(std::string args,
                                            UserState &state) {

  ListUserAuctionsRequest listUserAuctionsRequest;
  listUserAuctionsRequest.userID = state.getUserID();
  // THIS USER ID IS FOR TESTING PURPOSES ONLY
  listUserAuctionsRequest.userID = "123456";

  ListUserAuctionsResponse listUserAuctionsResponse;
  state.sendUdpPacketAndWaitForReply(listUserAuctionsRequest,
                                     listUserAuctionsResponse);

  if (listUserAuctionsResponse.status == ListUserAuctionsResponse::status::OK) {
    // probably refactor this into a function
    std::cout << "List user auctions successful!" << std::endl;
    const int columnWidth = 15;

    // Print the top border
    std::cout << "+" << std::setw(columnWidth) << std::setfill('-') << "+"
              << std::setw(columnWidth) << "+" << std::setfill(' ')
              << std::endl;

    // Print the header
    std::cout << "|" << std::setw(columnWidth - 1) << std::left << "Auction ID"
              << "|" << std::setw(columnWidth - 1) << std::left << "Status"
              << "|" << std::endl;

    // Print the border between header and data
    std::cout << std::setw(columnWidth) << std::setfill('-') << "+"
              << std::setw(columnWidth) << "+" << std::setfill(' ') << "+"
              << std::endl;

    for (const auto &auction : listUserAuctionsResponse.auctions) {
      std::cout << "|" << std::setw(columnWidth - 1) << std::left
                << auction.first << "|" << std::setw(columnWidth - 1)
                << std::left << (auction.second ? "Active" : "Not Active")
                << "|" << std::endl;

      // Print the border between rows
      std::cout << std::setw(columnWidth) << std::setfill('-') << "+"
                << std::setw(columnWidth) << "+" << std::setfill(' ') << "+"
                << std::endl;
    }

  } else if (listUserAuctionsResponse.status ==
             ListUserAuctionsResponse::status::NOK) {
    std::cout << "List user auctions failed: You have no ongoing auctions"
              << std::endl;
  } else if (listUserAuctionsResponse.status ==
             ListUserAuctionsResponse::status::NLG) {
    std::cout << "List user auctions failed: You are not logged in"
              << std::endl;
  } else if (listUserAuctionsResponse.status ==
             ListUserAuctionsResponse::status::ERR) {
    std::cout << "List user auctions failed: Server error" << std::endl;
  }
}

void ListUserBidsCommand::handleCommand(std::string args, UserState &state) {

  ListUserBidsRequest listUserBidsRequest;
  listUserBidsRequest.userID = state.getUserID();

  std::cout << listUserBidsRequest.serialize().str() << std::endl;
}

void ListAuctionsCommand::handleCommand(std::string args, UserState &state) {

  ListAuctionsRequest listAuctionsRequest;

  std::cout << listAuctionsRequest.serialize().str() << std::endl;
}

void ShowAssetCommand::handleCommand(std::string args, UserState &state) {
  std::string auction_id;
  std::vector<std::string> params = parse_args(args);

  if (params.size() != 1) {
    std::cout << "Invalid number of arguments: Expected 1, got "
              << params.size() << std::endl;
    return;
  }

  auction_id = params[0];

  if (validateAuctionID(auction_id) == INVALID) {
    std::cout << "Invalid auction ID: Must be a 3 digit postive number"
              << std::endl;
    return;
  }

  std::string message = "SAS " + auction_id;

  std::cout << message << std::endl;

  std::cout << "Show asset command" << args << std::endl;
}

void BidCommand::handleCommand(std::string args, UserState &state) {
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
    std::cout << "Invalid auction ID: Must be a 3 digit positive number"
              << std::endl;
    return;
  }

  if (validateBidValue(bid_value) == INVALID) {
    std::cout << "Invalid bid value: Must be a positive number" << std::endl;
    return;
  }

  std::string message = "BID " + auction_id + " " + bid_value;

  std::cout << message << std::endl;
}

void ShowRecordCommand::handleCommand(std::string args, UserState &state) {
  std::string auction_id;
  std::vector<std::string> params = parse_args(args);

  if (params.size() != 1) {
    std::cout << "Invalid number of arguments: Expected 1, got "
              << params.size() << std::endl;
    return;
  }

  auction_id = params[0];

  if (validateAuctionID(auction_id) == INVALID) {
    std::cout << "Invalid auction ID: Must be a 3 digit positive number"
              << std::endl;
    return;
  }

  ShowRecordRequest showRecordRequest;
  showRecordRequest.auctionID = auction_id;

  std::cout << showRecordRequest.serialize().str() << std::endl;
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
