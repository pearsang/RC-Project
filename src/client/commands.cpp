#include "commands.hpp"

#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>

void CommandManager::addCommand(std::shared_ptr<CommandHandler> handler)
{
  // Add the handler to the list of handlers
  handlerList.push_back(handler);

  // Add the handler to the map of handlers
  handlers[handler->name] = handler;

  // If the handler has an alias, add the alias to the map of handlers
  if (handler->alias.has_value())
  {
    handlers[*handler->alias] = handler;
  }
}



void CommandManager::waitForCommand()
{
  std::cout << "> ";

  std::string line;
  std::getline(std::cin, line);

  if (std::cin.eof())
  {
    return;
  }

  // Split the line into the command name and the arguments
  auto splitIndex = line.find(' ');

  std::string commandName;
  // If there is no space, then the entire line is the command name
  if (splitIndex == std::string::npos)
  {
    commandName = line;
    line = "";
  }
  else
  {
    commandName = line.substr(0, splitIndex);
    line.erase(0, splitIndex + 1);
  }

  if (commandName.length() == 0)
  {
    return;
  }

  // Find the command handler for the given command name
  auto handler = handlers.find(commandName);
  if (handler == handlers.end())
  {
    std::cout << "Invalid command: " << commandName << std::endl;
    return;
  }

  std::cout << "Command: " << commandName << std::endl;
  std ::cout << "Arguments: " << line << std::endl;

  // extract fiorst argumnet from line
  std::string userID = line.substr(line.find(' ') + 1);

  std::cout << "First argument: " << userID << std::endl;
}

void LoginCommand::handleCommand(std::string args)
{

  // parse args
  uint32_t user_id;

  try
  {
    user_id = parseUserId(args);
  }
  catch (...)
  {
    std::cout << "Invalid user ID: Must be a positive 6 digit number" << std::endl;
    return;
  }

  // send login request to server
  // wait for response
  // if response is success, then set user_id
  // else print error message
  // return

  std::cout << "Login command" << std::endl;
}


void LogoutCommand::handleCommand(std::string args)
{

  std::cout << "Logout command" << std::endl;
}

void UnregisterCommand::handleCommand(std::string args)
{

  std::cout << "Unregister command" << std::endl;
}

void ExitCommand::handleCommand(std::string args)
{

  std::cout << "Exit command" << std::endl;
}

void OpenAuctionCommand::handleCommand(std::string args)
{

  std::cout << "Open command" << std::endl;
}

void CloseAuctionCommand::handleCommand(std::string args)
{

  std::cout << "Close command" << std::endl;
}

void ListUserAuctionsCommand::handleCommand(std::string args)
{

  std::cout << "List user auctions command" << std::endl;
}

void ListUserBidsCommand::handleCommand(std::string args)
{

  std::cout << "List user bids command" << std::endl;
}

void ListAuctionsCommand::handleCommand(std::string args)
{

  std::cout << "List auctions command" << std::endl;
}

void ShowAssetCommand::handleCommand(std::string args)
{

  std::cout << "Show asset command" << std::endl;
}

void BidCommand::handleCommand(std::string args)
{

  std::cout << "Bid command" << std::endl;
}

void ShowRecordCommand::handleCommand(std::string args)
{

  std::cout << "Show record command" << std::endl;
}



uint32_t parseUserId(std::string args)
{

  std::string temp = args.substr(0, args.find(' '));

  if (!is_digits(temp))
  {
    throw std::runtime_error("Invalid user ID");
  }

  long userID = std::stol(args.substr(0, args.find(' ')), nullptr, 10);
  if (userID < 0 || userID > USER_ID_MAX)
  {
    throw std::runtime_error("Invalid user ID");
  }

  return (int32_t)userID;
}

std::string parsePassword(std::string args)
{
  std::string userPassword = args.substr(args.find(' ') + 1);

  if (userPassword.length() != 8 || !is_alphanumeric(userPassword))
  {
    throw std::runtime_error("Invalid password: Must be alphanumeric and 8 characters long");
  }

  return userPassword;
  
}
