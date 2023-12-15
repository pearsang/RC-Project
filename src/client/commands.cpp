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
  handlerList.push_back(handler);    // Add the handler to the list
  handlers[handler->name] = handler; // Add the handler to the map

  if (handler->alias.has_value()) {      // If the handler has an alias
    handlers[*handler->alias] = handler; // Add the handler alias to the map
  }
}

void CommandManager::waitForCommand(UserState &state) {
  std::cout << "> "; // Print the prompt

  std::string line;             // The line read from the user
  std::getline(std::cin, line); // Read the line from the user

  if (std::cin.eof()) { // If the user has pressed Ctrl+D
    return;
  }

  std::string commandName;          // The name of the command
  std::string args;                 // The arguments to the command
  auto splitIndex = line.find(' '); // Find the index of the separator space

  if (splitIndex == std::string::npos) { // If there is no space
    commandName = line;                  // The entire line is the command name
    args = "";                           // There are no arguments
  } else {
    commandName = line.substr(0, splitIndex);
    args = line.erase(0, splitIndex + 1);
  }

  if (commandName.length() == 0) { // If the command name is empty
    return;
  }

  auto handler = handlers.find(commandName); // Find the handler for the command

  if (handler == handlers.end()) { // If the handler does not exist
    std::cout << "Invalid command: " << commandName << std::endl;
    return;
  }

  try {
    handler->second->handleCommand(args, state); // Handle the command
  } catch (std::exception &e) {
    std::cout << "Error: " << e.what() << std::endl;
  }
}

void LoginCommand::handleCommand(std::string args, UserState &state) {
  std::vector<std::string> params = parse_args(args); // parse the arguments
  size_t numArgs = params.size();

  if (numArgs != LOGIN_ARGS_NUM) { // check the number of arguments
    std::cout << "Invalid number of arguments: Expected " << LOGIN_ARGS_NUM
              << " , got " << numArgs << std::endl;
    return;
  }

  std::string userID = params[0];
  if (validateUserID(userID) == INVALID) { // check if the user id is valid
    std::cout << "Invalid user ID: Must be a positive 6 digit number"
              << std::endl;
    return;
  }

  std::string password = params[1];
  if (validatePassword(password) == INVALID) { // check if the password is valid
    std::cout << "Invalid password: Must be alphanumeric and 8 characters long"
              << std::endl;
    return;
  }
  LoginRequest loginRequest;
  loginRequest.userID = userID;
  loginRequest.password = password;

  LoginResponse loginResponse;
  state.sendUdpPacketAndWaitForReply(loginRequest, loginResponse);

  switch (loginResponse.status) {
  case LoginResponse::status::OK:
    state.setUserID(userID);
    state.setPassword(password);
    std::cout << "Login successful!" << std::endl;
    break;
  case LoginResponse::status::NOK:
    std::cout << "Login failed: Incorrect password" << std::endl;
    break;
  case LoginResponse::status::REG:
    state.setUserID(userID);
    state.setPassword(password);
    std::cout << "Login successful: You were registered" << std::endl;
    break;
  case LoginResponse::status::ERR:
    std::cout << "Login failed: Server error" << std::endl;
    break;
  default:
    std::cout << "Unknown login status" << std::endl;
    break;
  }
}

void LogoutCommand::handleCommand(std::string args, UserState &state) {
  std::vector<std::string> params = parse_args(args); // parse the arguments
  size_t numArgs = params.size();

  if (numArgs != LOGOUT_ARGS_NUM) { // check the number of arguments
    std::cout << "Invalid number of arguments: Expected " << LOGOUT_ARGS_NUM
              << " , got " << numArgs << std::endl;
    return;
  }

  if (!state.isLoggedIn()) { // check if the user is logged in
    std::cout << "You are not logged in" << std::endl;
    return;
  }

  LogoutRequest logoutRequest;
  logoutRequest.userID = state.getUserID();
  logoutRequest.password = state.getPassword();

  LogoutResponse logoutResponse;
  state.sendUdpPacketAndWaitForReply(logoutRequest, logoutResponse);

  switch (logoutResponse.status) {
  case LogoutResponse::status::OK:
    state.setUserID("");   // clear the user id after logout
    state.setPassword(""); // clear the password after logout
    std::cout << "Logout successful!" << std::endl;
    break;
  case LogoutResponse::status::NOK:
    std::cout << "Logout failed: You are not logged in" << std::endl;
    break;
  case LogoutResponse::status::UNR:
    std::cout << "Logout failed: You are not registered" << std::endl;
    break;
  case LogoutResponse::status::ERR:
    std::cout << "Logout failed: Server error" << std::endl;
    break;
  default:
    std::cout << "Unknown logout status" << std::endl;
    break;
  }
}

void UnregisterCommand::handleCommand(std::string args, UserState &state) {
  std::vector<std::string> params = parse_args(args); // parse the arguments
  size_t numArgs = params.size();

  if (numArgs != UNREGISTER_ARGS_NUM) { // check the number of arguments
    std::cout << "Invalid number of arguments: Expected " << UNREGISTER_ARGS_NUM
              << " , got " << numArgs << std::endl;
    return;
  }

  if (!state.isLoggedIn()) { // check if the user is logged in
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

  switch (unregisterResponse.status) {
  case UnregisterResponse::status::OK:
    state.setUserID("");   // clear the user id after unregister
    state.setPassword(""); // clear the password after unregister
    std::cout << "Unregister successful!" << std::endl;
    break;
  case UnregisterResponse::status::NOK:
    std::cout << "Unregister failed: You are not logged in" << std::endl;
    break;
  case UnregisterResponse::status::UNR:
    std::cout << "Unregister failed: You are not registered" << std::endl;
    break;
  case UnregisterResponse::status::ERR:
    std::cout << "Unregister failed: Server error" << std::endl;
    break;
  default:
    std::cout << "Unknown unregister status" << std::endl;
    break;
  }
}

void ExitCommand::handleCommand(std::string args, UserState &state) {
  (void)args; // args are not used in this command

  if (state.isLoggedIn()) { // check if the user is logged in
    std::cout << "You are still logged in. Please logout first." << std::endl;
    return;
  }

  is_exiting = true; // set the global variable to true to exit the program
}

void OpenAuctionCommand::handleCommand(std::string args, UserState &state) {
  std::vector<std::string> params = parse_args(args);
  size_t numArgs = params.size();

  if (numArgs != OPEN_AUCTION_ARGS_NUM) { // check the number of arguments
    std::cout << "Invalid number of arguments: Expected "
              << OPEN_AUCTION_ARGS_NUM << " , got " << numArgs << std::endl;
    return;
  }

  if (!state.isLoggedIn()) { // check if the user is logged in
    std::cout << "Im useless for now REMOVE ME" << std::endl;
  }

  std::string auctionName = params[0];
  std::string assetFileName = params[1];
  std::string startValue = params[2];
  std::string timeActive = params[3];

  if (validateAuctionName(auctionName) == INVALID) { // check auction name
    std::cout << "Invalid auction_name: Must be alphanumeric and 10 characters "
                 "long"
              << std::endl;
    return;
  }

  if (validateAssetFilename(assetFileName) == INVALID) { // check asset filename
    std::cout << "Invalid asset_filename: Must be alphanumeric, 24 characters "
                 "including a 3 character long extension"
              << std::endl;
    return;
  }

  if (validateStartValue(startValue) == INVALID) { // check start value
    std::cout << "Invalid start value: Must be a positive number up to 6 digits"
              << std::endl;
    return;
  }

  if (validateAuctionDuration(timeActive) == INVALID) {
    std::cout << "Invalid time active: Must be a positive number up to 5 digits"
              << std::endl;
    return;
  }

  if (validateFileSize(assetFileName) == INVALID) { // check file size
    std::cout << "Invalid file size: File must exist or be less than 10MB"
              << std::endl;
    return;
  }

  OpenAuctionRequest openAuctionRequest;
  openAuctionRequest.userID = state.getUserID();
  openAuctionRequest.password = state.getPassword();
  openAuctionRequest.auctionName = auctionName;
  openAuctionRequest.assetFileName = assetFileName;
  openAuctionRequest.startValue = (uint32_t)std::stoi(startValue);
  openAuctionRequest.timeActive = (uint32_t)std::stoi(timeActive);
  openAuctionRequest.assetSize = getFileSize(assetFileName);

  OpenAuctionResponse openAuctionResponse;
  state.sendTcpPacketAndWaitForReply(openAuctionRequest, openAuctionResponse);

  switch (openAuctionResponse.status) {
  case OpenAuctionResponse::status::OK:
    std::cout << "Open auction successful: " << openAuctionResponse.auctionID
              << std::endl;
    break;
  case OpenAuctionResponse::status::NOK:
    std::cout << "Open auction failed: Auction could not be started"
              << std::endl;
    break;
  case OpenAuctionResponse::status::NLG:
    std::cout << "Open auction failed: You are not logged in" << std::endl;
    break;
  case OpenAuctionResponse::status::ERR:
    std::cout << "Open auction failed: Server error" << std::endl;
    break;
  default:
    std::cout << "Unknown open auction status" << std::endl;
    break;
  }
}

void CloseAuctionCommand::handleCommand(std::string args, UserState &state) {
  std::vector<std::string> params = parse_args(args);
  size_t numArgs = params.size();

  if (numArgs != CLOSE_AUCTION_ARGS_NUM) { // check the number of arguments
    std::cout << "Invalid number of arguments: Expected "
              << CLOSE_AUCTION_ARGS_NUM << " , got " << numArgs << std::endl;
    return;
  }

  if (!state.isLoggedIn()) { // check if the user is logged in
    std::cout << "Close auction failed: You are not logged in" << std::endl;
    return;
  }

  std::string auctionID = params[0];

  if (validateAuctionID(auctionID) == INVALID) { // check auction id
    std::cout << "Invalid auction ID" << std::endl;
    return;
  }

  CloseAuctionRequest closeAuctionRequest;
  closeAuctionRequest.userID = state.getUserID();
  closeAuctionRequest.password = state.getPassword();
  closeAuctionRequest.auctionID = auctionID;

  CloseAuctionResponse closeAuctionResponse;
  state.sendTcpPacketAndWaitForReply(closeAuctionRequest, closeAuctionResponse);

  switch (closeAuctionResponse.status) {
  case CloseAuctionResponse::status::OK:
    std::cout << "Close auction successful!" << std::endl;
    break;
  case CloseAuctionResponse::status::NOK:
    std::cout << "Close auction failed: You are not the host of this auction"
              << std::endl;
    break;
  case CloseAuctionResponse::status::NLG:
    std::cout << "Close auction failed: You are not logged in" << std::endl;
    break;
  case CloseAuctionResponse::status::EAU:
    std::cout << "Close auction failed: The auction does not exist"
              << std::endl;
    break;
  case CloseAuctionResponse::status::EOW:
    std::cout << "Close auction failed: You do not own this auction"
              << std::endl;
    break;
  case CloseAuctionResponse::status::END:
    std::cout << "Close auction failed: The auction has already ended"
              << std::endl;
    break;
  case CloseAuctionResponse::status::ERR:
    std::cout << "Close auction failed: Server error" << std::endl;
    break;
  default:
    std::cout << "Unknown close auction status" << std::endl;
    break;
  }
}

void ListUserAuctionsCommand::handleCommand(std::string args,
                                            UserState &state) {
  std::vector<std::string> params = parse_args(args);
  size_t numArgs = params.size();

  if (numArgs != LIST_USER_AUCTIONS_ARGS_NUM) { // check the number of arguments
    std::cout << "Invalid number of arguments: Expected "
              << LIST_USER_AUCTIONS_ARGS_NUM << " , got " << numArgs
              << std::endl;
    return;
  }

  ListUserAuctionsRequest listUserAuctionsRequest;
  listUserAuctionsRequest.userID = state.getUserID();

  ListUserAuctionsResponse listUserAuctionsResponse;
  state.sendUdpPacketAndWaitForReply(listUserAuctionsRequest,
                                     listUserAuctionsResponse);

  switch (listUserAuctionsResponse.status) {
  case ListUserAuctionsResponse::status::OK:
    std::cout << "List user auctions successful!" << std::endl;
    printListUserAuctionsTable(listUserAuctionsResponse.auctions);
    break;
  case ListUserAuctionsResponse::status::NOK:
    std::cout << "List user auctions failed: You have no ongoing auctions"
              << std::endl;
    break;
  case ListUserAuctionsResponse::status::NLG:
    std::cout << "List user auctions failed: You are not logged in"
              << std::endl;
    break;
  case ListUserAuctionsResponse::status::ERR:
    std::cout << "List user auctions failed: Server error" << std::endl;
    break;
  default:
    std::cout << "Unknown list user auctions status" << std::endl;
    break;
  }
}

void ListUserBidsCommand::handleCommand(std::string args, UserState &state) {
  std::vector<std::string> params = parse_args(args);
  size_t numArgs = params.size();

  if (numArgs != LIST_USER_BIDS_ARGS_NUM) { // check the number of arguments
    std::cout << "Invalid number of arguments: Expected "
              << LIST_USER_BIDS_ARGS_NUM << " , got " << numArgs << std::endl;
    return;
  }

  ListUserBidsRequest listUserBidsRequest;
  listUserBidsRequest.userID = state.getUserID();

  ListUserBidsResponse listUserBidsResponse;
  state.sendUdpPacketAndWaitForReply(listUserBidsRequest, listUserBidsResponse);

  switch (listUserBidsResponse.status) {
  case ListUserBidsResponse::status::OK:
    std::cout << "List user auctions with bids successful!" << std::endl;
    printListUserBidsTable(listUserBidsResponse.auctions);
    break;
  case ListUserBidsResponse::status::NOK:
    std::cout << "List user auctions with bids failed: You have no ongoing bids"
              << std::endl;
    break;
  case ListUserBidsResponse::status::NLG:
    std::cout << "List user bids failed: You are not logged in" << std::endl;
    break;
  case ListUserBidsResponse::status::ERR:
    std::cout << "List user bids failed: Server error" << std::endl;
    break;
  default:
    std::cout << "Unknown list user bids status" << std::endl;
    break;
  }
}

void ListAuctionsCommand::handleCommand(std::string args, UserState &state) {
  std::vector<std::string> params = parse_args(args);
  size_t numArgs = params.size();

  if (numArgs != LIST_AUCTIONS_ARGS_NUM) { // check the number of arguments
    std::cout << "Invalid number of arguments: Expected "
              << LIST_AUCTIONS_ARGS_NUM << " , got " << numArgs << std::endl;
    return;
  }

  ListAuctionsRequest listAuctionsRequest;

  ListAuctionsResponse listAuctionsResponse;
  state.sendUdpPacketAndWaitForReply(listAuctionsRequest, listAuctionsResponse);

  switch (listAuctionsResponse.status) {
  case ListAuctionsResponse::status::OK:
    std::cout << "List auctions successful!" << std::endl;
    printListAuctionsTable(listAuctionsResponse.auctions);
    break;
  case ListAuctionsResponse::status::NOK:
    std::cout << "List auctions failed: There are no ongoing auctions"
              << std::endl;
    break;
  case ListAuctionsResponse::status::ERR:
    std::cout << "List auctions failed: Server error" << std::endl;
    break;
  default:
    std::cout << "Unknown list auctions status" << std::endl;
    break;
  }
}

void ShowAssetCommand::handleCommand(std::string args, UserState &state) {
  (void)state; // state is not used in this command
  std::vector<std::string> params = parse_args(args);
  size_t numArgs = params.size();

  if (numArgs != SHOW_ASSETS_ARGS_NUM) {
    std::cout << "Invalid number of arguments: Expected "
              << SHOW_ASSETS_ARGS_NUM << " , got " << numArgs << std::endl;
    return;
  }

  std::string auctionID = params[0];

  if (validateAuctionID(auctionID) == INVALID) { // check auction id
    std::cout << "Invalid auction ID: Must be a 3 digit postive number"
              << std::endl;
    return;
  }

  ShowAssetRequest showAssetRequest;
  showAssetRequest.auctionID = auctionID;

  ShowAssetResponse showAssetResponse;
  state.sendTcpPacketAndWaitForReply(showAssetRequest, showAssetResponse);

  switch (showAssetResponse.status) {
  case ShowAssetResponse::status::OK:
    std::cout << "Asset image downloaded successfully!" << std::endl;
    std::cout << "File name: " << showAssetResponse.assetFileName << std::endl;
    std::cout << "File saved to: " << std::filesystem::current_path() << "/"
              << showAssetResponse.assetFileName << std::endl;
    break;
  case ShowAssetResponse::status::NOK:
    std::cout << "Show asset failed: There is no auction file available"
              << std::endl;
    break;
  case ShowAssetResponse::status::ERR:
    std::cout << "Show asset failed: Server error" << std::endl;
    break;
  default:
    std::cout << "Unknown show asset status" << std::endl;
    break;
  }
}

void BidCommand::handleCommand(std::string args, UserState &state) {
  std::vector<std::string> params = parse_args(args);
  size_t numArgs = params.size();

  if (numArgs != BID_ARGS_NUM) { // check the number of arguments
    std::cout << "Invalid number of arguments: Expected " << BID_ARGS_NUM
              << " , got " << numArgs << std::endl;
    return;
  }

  if (!state.isLoggedIn()) { // check if the user is logged in
    std::cout << "Bid failed: You are not logged in" << std::endl;
    return;
  }

  std::string auctionID = params[0];
  std::string bidValue = params[1];

  if (validateAuctionID(auctionID) == INVALID) { // check auction id
    std::cout << "Invalid auction ID: Must be a 3 digit positive number"
              << std::endl;
    return;
  }

  if (validateBidValue(bidValue) == INVALID) { // check bid value
    std::cout
        << "Invalid bid value: Must be a positive number between 0 and 999999"
        << std::endl;
    return;
  }

  BidRequest bidRequest;
  bidRequest.userID = state.getUserID();
  bidRequest.password = state.getPassword();
  bidRequest.auctionID = auctionID;
  bidRequest.bidValue = (uint32_t)std::stoi(bidValue);

  BidResponse bidResponse;
  state.sendTcpPacketAndWaitForReply(bidRequest, bidResponse);

  switch (bidResponse.status) {
  case BidResponse::status::ACC:
    std::cout << "Bid successful!" << std::endl;
    break;
  case BidResponse::status::NLG:
    std::cout << "Bid failed: You are not logged in" << std::endl;
    break;
  case BidResponse::status::NOK:
    std::cout << "Bid failed: The auction is not active" << std::endl;
    break;
  case BidResponse::status::REF:
    std::cout << "Bid failed: A larger or equal bid has already been placed"
              << std::endl;
    break;
  case BidResponse::status::ILG:
    std::cout << "Bid failed: Can not bid on your own auction" << std::endl;
    break;
  case BidResponse::status::ERR:
    std::cout << "Bid failed: Server error" << std::endl;
    break;
  default:
    std::cout << "Unknown bid status" << std::endl;
    break;
  }
}

void ShowRecordCommand::handleCommand(std::string args, UserState &state) {
  std::vector<std::string> params = parse_args(args);
  size_t numArgs = params.size();

  if (numArgs != SHOW_RECORD_ARGS_NUM) {
    std::cout << "Invalid number of arguments: Expected "
              << SHOW_RECORD_ARGS_NUM << " , got " << numArgs << std::endl;
    return;
  }

  std::string auctionID = params[0];

  if (validateAuctionID(auctionID) == INVALID) { // check auction id
    std::cout << "Invalid auction ID: Must be a 3 digit positive number"
              << std::endl;
    return;
  }
  ShowRecordRequest showRecordRequest;
  showRecordRequest.auctionID = auctionID;

  ShowRecordResponse showRecordResponse;
  state.sendUdpPacketAndWaitForReply(showRecordRequest, showRecordResponse);

  switch (showRecordResponse.status) {
  case ShowRecordResponse::status::OK:
    printListShowRecordTable(
        showRecordResponse.hostUID, showRecordResponse.auctionName,
        showRecordResponse.assetFileName, showRecordResponse.startValue,
        showRecordResponse.startDate, showRecordResponse.timeActive,
        showRecordResponse.bids, showRecordResponse.end);
    std::cout << "Show record successful!" << std::endl;
    break;
  case ShowRecordResponse::status::NOK:
    std::cout << "Show record failed: Auction does not exist" << std::endl;
    break;
  case ShowRecordResponse::status::ERR:
    std::cout << "Show record failed: Server error" << std::endl;
    break;
  default:
    std::cout << "Unknown show record status" << std::endl;
    break;
  }
}
