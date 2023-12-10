#include "commands.hpp"
#include "./utils/protocol.hpp"

#include "../utils/utils.hpp"
#include <cctype>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <unistd.h>

extern bool is_exiting;

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
    state.setUserID(user_id);
    state.setPassword(password);
  } else if (loginResponse.status == LoginResponse::status::ERR) {
    std::cout << "Login failed: Server error" << std::endl;
  }
}

void LogoutCommand::handleCommand(std::string args, UserState &state) {
  if (parse_args(args).size() != 0) {
    std::cout << "Invalid number of arguments: Expected 0, got "
              << parse_args(args).size() << std::endl;
    return;
  }
  if (!state.isLoggedIn()) {
    std::cout << "You are not logged in" << std::endl;
    return;
  }

  LogoutRequest logoutRequest;
  logoutRequest.userID = state.getUserID();
  logoutRequest.password = state.getPassword();

  logoutRequest.userID = "192837";
  logoutRequest.password = "eusogayy";

  LogoutResponse logoutResponse;
  state.sendUdpPacketAndWaitForReply(logoutRequest, logoutResponse);

  if (logoutResponse.status == LogoutResponse::status::OK) {
    std::cout << "Logout successful!" << std::endl;
    std::string empty = "";
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
  if (parse_args(args).size() != 0) {
    std::cout << "Invalid number of arguments: Expected 0, got "
              << parse_args(args).size() << std::endl;
    return;
  }

  // check if a user is logged in
  if (!state.isLoggedIn()) {
    std::cout << "You must be logged in to unregister" << std::endl;
    return;
  }

  UnregisterRequest unregisterRequest;
  unregisterRequest.userID = state.getUserID();
  unregisterRequest.password = state.getPassword();

  UnregisterResponse unregisterResponse;
  state.sendUdpPacketAndWaitForReply(unregisterRequest, unregisterResponse);

  if (unregisterResponse.status == UnregisterResponse::status::OK) {
    std::cout << "Unregister successful!" << std::endl;
    std::string empty = "";
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
  // args are not used
  (void)args;
  // user has not logged out
  if (state.isLoggedIn()) {
    std::cout << "You are still logged in. Please logout first." << std::endl;
    return;
  }

  is_exiting = true;
}

void OpenAuctionCommand::handleCommand(std::string args, UserState &state) {
  std::vector<std::string> params = parse_args(args);

  if (!state.isLoggedIn()) {
    std::cout << "Im useless for now REMOVE ME" << std::endl;
  }

  if (params.size() != 4) {
    std::cout << "Invalid number of arguments: Expected 4, got "
              << params.size() << std::endl;
    return;
  }

  std::string auction_name = params[0];
  std::string asset_fname = params[1];
  std::string start_value = params[2];
  std::string timeactive = params[3];

  if (validateAuctionName(auction_name) == INVALID) {
    std::cout << "Invalid auction_name: Must be alphanumeric and 10 characters "
                 "long"
              << std::endl;
    return;
  }

  if (validateAssetFilename(asset_fname) == INVALID) {
    std::cout << "Invalid asset_filename: Must be alphanumeric, 24 characters "
                 "including a 3 character long extension"
              << std::endl;
    return;
  }

  // check if start_value is numerical
  if (validateStartValue(start_value) == INVALID) {
    std::cout << "Invalid start value: Must be a positive number up to 6 digits"
              << std::endl;
    return;
  }

  // check if timeactive is numerical
  if (validateAuctionDuration(timeactive) == INVALID) {
    std::cout << "Invalid time active: Must be a positive number up to 5 digits"
              << std::endl;
    return;
  }

  // check file size
  if (validateFileSize(asset_fname) == INVALID) {
    std::cout << "Invalid file size: File must exist or be less than 10MB"
              << std::endl;
    return;
  }

  OpenAuctionRequest openAuctionRequest;
  openAuctionRequest.userID = state.getUserID();
  openAuctionRequest.password = state.getPassword();
  openAuctionRequest.auctionName = auction_name;
  openAuctionRequest.assetFilename = asset_fname;
  openAuctionRequest.startValue = (uint32_t)std::stoi(start_value);
  openAuctionRequest.timeActive = (uint32_t)std::stoi(timeactive);
  openAuctionRequest.assetSize = getFileSize(asset_fname);

  OpenAuctionResponse openAuctionResponse;
  state.sendTcpPacketAndWaitForReply(openAuctionRequest, openAuctionResponse);

  if (openAuctionResponse.status == OpenAuctionResponse::status::OK) {
    std::cout << "Open auction successful: " << openAuctionResponse.auctionID
              << std::endl;
  } else if (openAuctionResponse.status == OpenAuctionResponse::status::NOK) {
    std::cout << "Open auction failed: Auction could not be started"
              << std::endl;
  } else if (openAuctionResponse.status == OpenAuctionResponse::status::NLG) {
    std::cout << "Open auction failed: You are not logged in" << std::endl;
  } else if (openAuctionResponse.status == OpenAuctionResponse::status::ERR) {
    std::cout << "Open auction failed: Server error" << std::endl;
  }
}

void CloseAuctionCommand::handleCommand(std::string args, UserState &state) {
  if (!state.isLoggedIn()) {
    std::cout << "Im useless for now REMOVE ME" << std::endl;
  }
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

  CloseAuctionRequest closeAuctionRequest;
  closeAuctionRequest.userID = state.getUserID();
  closeAuctionRequest.password = state.getPassword();
  closeAuctionRequest.auctionID = auction_id;

  CloseAuctionResponse closeAuctionResponse;
  state.sendTcpPacketAndWaitForReply(closeAuctionRequest, closeAuctionResponse);

  if (closeAuctionResponse.status == CloseAuctionResponse::status::OK) {
    std::cout << "Close auction successful!" << std::endl;
  } else if (closeAuctionResponse.status == CloseAuctionResponse::status::NLG) {
    std::cout << "Close auction failed: You are not logged in" << std::endl;
  } else if (closeAuctionResponse.status == CloseAuctionResponse::status::EAU) {
    std::cout << "Close auction failed: The auction does not exist"
              << std::endl;
  } else if (closeAuctionResponse.status == CloseAuctionResponse::status::EOW) {
    std::cout << "Close auction failed: You do not own this auction"
              << std::endl;
  } else if (closeAuctionResponse.status == CloseAuctionResponse::status::END) {
    std::cout << "Close auction failed: The auction has already ended"
              << std::endl;
  } else if (closeAuctionResponse.status == CloseAuctionResponse::status::ERR) {
    std::cout << "Close auction failed: Server error" << std::endl;
  }
}

void ListUserAuctionsCommand::handleCommand(std::string args,
                                            UserState &state) {
  if (parse_args(args).size() != 0) {
    std::cout << "Invalid number of arguments: Expected 0, got "
              << parse_args(args).size() << std::endl;
    return;
  }

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

  if (parse_args(args).size() != 0) {
    std::cout << "Invalid number of arguments: Expected 0, got "
              << parse_args(args).size() << std::endl;
    return;
  }

  ListUserBidsRequest listUserBidsRequest;
  listUserBidsRequest.userID = state.getUserID();

  ListUserBidsResponse listUserBidsResponse;
  state.sendUdpPacketAndWaitForReply(listUserBidsRequest, listUserBidsResponse);

  if (listUserBidsResponse.status == ListUserBidsResponse::status::OK) {
    std::cout << "List user auctions with bids successful!" << std::endl;
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

    for (const auto &auction : listUserBidsResponse.auctions) {
      std::cout << "|" << std::setw(columnWidth - 1) << std::left
                << auction.first << "|" << std::setw(columnWidth - 1)
                << std::left << (auction.second ? "Active" : "Not Active")
                << "|" << std::endl;

      // Print the border between rows
      std::cout << std::setw(columnWidth) << std::setfill('-') << "+"
                << std::setw(columnWidth) << "+" << std::setfill(' ') << "+"
                << std::endl;
    }
  } else if (listUserBidsResponse.status == ListUserBidsResponse::status::NOK) {
    std::cout << "List user auctions with bids failed: You have no ongoing bids"
              << std::endl;
  } else if (listUserBidsResponse.status == ListUserBidsResponse::status::NLG) {
    std::cout << "List user bids failed: You are not logged in" << std::endl;
  } else if (listUserBidsResponse.status == ListUserBidsResponse::status::ERR) {
    std::cout << "List user bids failed: Server error" << std::endl;
  }
}

void ListAuctionsCommand::handleCommand(std::string args, UserState &state) {

  if (parse_args(args).size() != 0) {
    std::cout << "Invalid number of arguments: Expected 0, got "
              << parse_args(args).size() << std::endl;
    return;
  }

  ListAuctionsRequest listAuctionsRequest;

  ListAuctionsResponse listAuctionsResponse;
  state.sendUdpPacketAndWaitForReply(listAuctionsRequest, listAuctionsResponse);

  if (listAuctionsResponse.status == ListAuctionsResponse::status::OK) {
    std::cout << "List auctions successful!" << std::endl;
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

    for (const auto &auction : listAuctionsResponse.auctions) {
      std::cout << "|" << std::setw(columnWidth - 1) << std::left
                << auction.first << "|" << std::setw(columnWidth - 1)
                << std::left << (auction.second ? "Active" : "Not Active")
                << "|" << std::endl;

      // Print the border between rows
      std::cout << std::setw(columnWidth) << std::setfill('-') << "+"
                << std::setw(columnWidth) << "+" << std::setfill(' ') << "+"
                << std::endl;
    }
  } else if (listAuctionsResponse.status == ListAuctionsResponse::status::NOK) {
    std::cout << "List auctions failed: There are no ongoing auctions"
              << std::endl;
  } else if (listAuctionsResponse.status == ListAuctionsResponse::status::ERR) {
    std::cout << "List auctions failed: Server error" << std::endl;
  }
}

void ShowAssetCommand::handleCommand(std::string args, UserState &state) {

  if (!state.isLoggedIn()) {
    std::cout << "Im useless for now REMOVE ME" << std::endl;
  }

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

  ShowAssetRequest showAssetRequest;
  showAssetRequest.auctionID = auction_id;

  ShowAssetResponse showAssetResponse;
  state.sendTcpPacketAndWaitForReply(showAssetRequest, showAssetResponse);

  if (showAssetResponse.status == ShowAssetResponse::status::OK) {
    std::cout << "Asset image downloaded successfully!" << std::endl;

    std::cout << "File name: " << showAssetResponse.assetFilename << std::endl;

    // show the file path
    std::cout << "File saved to: " << std::filesystem::current_path() << "/"
              << showAssetResponse.assetFilename << std::endl;
  } else if (showAssetResponse.status == ShowAssetResponse::status::NOK) {
    std::cout << "Show asset failed: There is auction file available"
              << std::endl;
  } else if (showAssetResponse.status == ShowAssetResponse::status::ERR) {
    std::cout << "Show asset failed: Server error" << std::endl;
  }
}

void BidCommand::handleCommand(std::string args, UserState &state) {
  // if there is no user logged in, bid fails
  if (!state.isLoggedIn()) {
    std::cout << "Bid failed: You are not logged in" << std::endl;
    return;
  }
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

  BidRequest bidRequest;
  bidRequest.userID = state.getUserID();
  bidRequest.password = state.getPassword();
  bidRequest.auctionID = auction_id;
  bidRequest.bidValue = (uint32_t)std::stoi(bid_value);

  BidResponse bidResponse;
  state.sendTcpPacketAndWaitForReply(bidRequest, bidResponse);

  if (bidResponse.status == BidResponse::status::ACC) {
    std::cout << "Bid successful!" << std::endl;
  } else if (bidResponse.status == BidResponse::status::NLG) {
    std::cout << "Bid failed: You are not logged in" << std::endl;
  } else if (bidResponse.status == BidResponse::status::NOK) {
    std::cout << "Bid failed: The auction is not active" << std::endl;
  } else if (bidResponse.status == BidResponse::status::REF) {
    std::cout << "Bid failed: A larger bid has already been placed"
              << std::endl;
  } else if (bidResponse.status == BidResponse::status::ILG) {
    std::cout << "Bid failed: Can not bid on your own auction" << std::endl;
  } else if (bidResponse.status == BidResponse::status::ERR) {
    std::cout << "Bid failed: Server error" << std::endl;
  }
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

  ShowRecordResponse showRecordResponse;
  state.sendUdpPacketAndWaitForReply(showRecordRequest, showRecordResponse);

  if (showRecordResponse.status == ShowRecordResponse::status::OK) {

    const int columnWidth = 30;

    std::tm *tm_info = std::localtime(&showRecordResponse.startDate);

    // Format the date and time
    char start_date[20];
    std::strftime(start_date, sizeof(start_date), "%Y-%m-%d %H:%M:%S", tm_info);

    std::cout << "Show record successful!" << std::endl;
    std::cout << "Host ID: " << showRecordResponse.hostUID << "\t\t"
              << "Auction Name: " << showRecordResponse.auctionName << "\t"
              << "Asset Filename: " << showRecordResponse.assetFilename << "\t"
              << "Start Value: " << showRecordResponse.startValue << "\t"
              << "Start Date: " << start_date << "\t\t"
              << "Active Time: " << showRecordResponse.timeActive << std::endl;

    // Print the top border
    std::cout << "+" << std::setw(columnWidth) << std::setfill('-') << "+"
              << std::setw(columnWidth) << "+" << std::setw(columnWidth) << "+"
              << std::setw(columnWidth) << "+" << std::setfill(' ')
              << std::endl;

    // Print the header
    std::cout << "|" << std::setw(columnWidth - 1) << std::left << "Bidder"
              << "|" << std::setw(columnWidth - 1) << std::left << "Bid Value"
              << "|" << std::setw(columnWidth - 1) << std::left << "Bid Date"
              << "|" << std::setw(columnWidth - 1) << std::left
              << "Bid Sec Time"
              << "|" << std::endl;

    // Print the middle border
    std::cout << std::setw(columnWidth) << std::setfill('-') << "+"
              << std::setw(columnWidth) << "+" << std::setw(columnWidth) << "+"
              << std::setw(columnWidth) << "+" << std::setfill(' ') << "+"
              << std::endl;

    for (const auto &bid : showRecordResponse.bids) {
      // Accessing elements of the tuple
      std::string bidder_UID = std::get<0>(bid);
      uint32_t bid_value = std::get<1>(bid);
      time_t bid_date_time = std::get<2>(bid);
      uint32_t bid_sec_time = std::get<3>(bid);

      // Convert time_t to a struct tm for extracting date and time components
      std::tm *bid_date_time_tm_info = std::localtime(&bid_date_time);

      // Format the date and time
      char date[20];
      std::strftime(date, sizeof(date), "%Y-%m-%d %H:%M:%S",
                    bid_date_time_tm_info);

      std::cout << "|" << std::setw(columnWidth - 1) << std::left << bidder_UID
                << "|" << std::setw(columnWidth - 1) << std::left << bid_value
                << "|" << std::setw(columnWidth - 1) << std::left << date << "|"
                << std::setw(columnWidth - 1) << std::left << bid_sec_time
                << "|" << std::endl;

      // Print the border between rows
      std::cout << std::setw(columnWidth) << std::setfill('-') << "+"
                << std::setw(columnWidth) << "+" << std::setw(columnWidth)
                << "+" << std::setw(columnWidth) << "+" << std::setfill(' ')
                << "+" << std::endl;
    }
    if (showRecordResponse.end.first != 0) {
      std::cout << "End Date: " << showRecordResponse.end.first << "\t\t"
                << "Time passed: " << showRecordResponse.end.second << "\t"
                << std::endl;
    }
  } else if (showRecordResponse.status == ShowRecordResponse::status::NOK) {
    std::cout << "Show record failed: Auction does not exist" << std::endl;
  } else if (showRecordResponse.status == ShowRecordResponse::status::ERR) {
    std::cout << "Show record failed: Server error" << std::endl;
  }
}
