#ifndef COMMANDS_H
#define COMMANDS_H

#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include "../utils/constants.hpp"
#include "../utils/utils.hpp"


class CommandHandler;

/**
 * @class CommandManager
 *
 * @brief Manages and handles commands through a list of registered
 * CommandHandlers.
 *
 * The CommandManager class maintains a list of CommandHandlers and provides a
 * method to wait for and handle incoming commands. CommandHandlers are
 * registered with the manager and are responsible for defining the behavior of
 * specific commands.
 */
class CommandManager {
  // List of registered command handlers.
  std::vector<std::shared_ptr<CommandHandler>> handlerList;
  // Map of command names to command handlers.
  std::unordered_map<std::string, std::shared_ptr<CommandHandler>> handlers;

 public:
   /**
    * @brief Waits for and handles incoming commands.
    */
   void waitForCommand();
   void addCommand(std::shared_ptr<CommandHandler> handler);
};

/**
 * @class CommandHandler
 *
 * @brief Represents a base class for implementing command-specific behavior.
 *
 * CommandHandler define the properties and behavior of specific commands that
 * the CommandManager can handle. Each CommandHandler has a unique name, an
 * optional alias, optional usage information, and a description of the
 * command's purpose.
 */
class CommandHandler {
 protected:
   /**
    * @brief Constructs a CommandHandler with specified attributes.
    *
    * @param name The unique name of the command.
    * @param alias An optional alias for the command.
    * @param usage An optional string describing the usage of the command.
    * @param description The description of the command's purpose.
    */
   CommandHandler(const char *name, const std::optional<const char *> alias,
                  const std::optional<const char *> usage,
                  const char *description)
       : name{name}, alias{alias}, usage{usage}, description{description} {}

 public:
  const char* name;
  const std::optional<const char*> alias;
  const std::optional<const char*> usage;
  const char* description;
};


/**
 * @class LoginCommand
 * 
 * @brief Represents a command for logging in to the server.
 * 
*/
class LoginCommand : public CommandHandler {
  void handleCommand(std::string args);

  public:
    LoginCommand() : CommandHandler("login", std::nullopt, "LIN", "Login") {}
};

/**
 * @class LogoutCommand
 * 
 * @brief Represents a command for logging out of the server.
 * 
*/
class LogoutCommand : public CommandHandler {
  void handleCommand(std::string args);

  public:
    LogoutCommand() : CommandHandler("logout", std::nullopt, "LOU", "Logout") {}
};


/**
 * @class UnresgisterCommand
 * 
 * @brief Represents a command for unregistering from the server.
*/
class UnregisterCommand : public CommandHandler {
  void handleCommand(std::string args);

  public:
    UnregisterCommand() : CommandHandler("unregister", std::nullopt, "UNR", "Unregister") {}
};


/**
 * @class ExitCommand
 * 
 * @brief Represents a command for exiting the application.
*/
class ExitCommand : public CommandHandler {
  void handleCommand(std::string args);

  // The Exit command has no usage 
  public:
    ExitCommand() : CommandHandler("exit", std::nullopt, std::nullopt, "Exit") {}
};

/**
 * @class OpenCommand
 * 
 * @brief Represents a command for opening a new auction.
*/
class OpenAuctionCommand : public CommandHandler {
  void handleCommand(std::string args);

  public:
    OpenAuctionCommand() : CommandHandler("open", std::nullopt, "OPA", "Open Auction") {}
};

/**
 * @class CloseCommand
 * 
 * @brief Represents a command for closing an auction.
*/
class CloseAuctionCommand : public CommandHandler {
  void handleCommand(std::string args);

  public:
    CloseAuctionCommand() : CommandHandler("close", std::nullopt, "CLS", "Close Auction") {}
};

/**
 * @class ListUserAuctionsCommand
 * 
 * @brief Represents a command for listing all auctions for a user.
*/
class ListUserAuctionsCommand : public CommandHandler {
  void handleCommand(std::string args);

  public:
    ListUserAuctionsCommand() : CommandHandler("myauctions", "ma", "LMA", "List User Auctions") {}
};

/**
 * @class ListUserBidsCommand
 * 
 * @brief Represents a command for listing all auctions that the user bidded.
*/
class ListUserBidsCommand : public CommandHandler {
  void handleCommand(std::string args);

  public:
    ListUserBidsCommand() : CommandHandler("mybids", "mb", "LMB", "List User Bids") {}
};

/**
 * @class ListAuctionsCommand
 * 
 * @brief Represents a command for listing all active auctions.
*/
class ListAuctionsCommand : public CommandHandler {
  void handleCommand(std::string args);

  public:
    ListAuctionsCommand() : CommandHandler("list", "l", "LST", "List Currently Active Auctions") {}
};


/**
 * @class ShowAssetCommand
 * 
 * @brief Represents a command for showing the image of the asset of an auction.
*/
class ShowAssetCommand : public CommandHandler {
  void handleCommand(std::string args);

  public:
    ShowAssetCommand() : CommandHandler("show_asset", "sa", "SAS", "Show Asset") {}
};

/**
 * @class BidCommand
 * 
 * @brief Represents a command for bidding on an auction.
*/
class BidCommand : public CommandHandler {
  void handleCommand(std::string args);

  public:
    BidCommand() : CommandHandler("bid", "b", "BID", "Place a Bid") {}
};

/**
 * @class ShowRecordCommand
 * 
 * @brief Represents a command for showing the record of an auction.
*/
class ShowRecordCommand : public CommandHandler {
  void handleCommand(std::string args);

  public:
    ShowRecordCommand() : CommandHandler("show_record", "sr", "SRC", "Show Auction Record") {}
};

/**
 * 
 * @brief Parses the user ID from the given string.
 * 
*/
uint32_t parseUserId(std::string args);

/**
 * 
 * @brief Parses the user password from the given string.
 * 
*/
std::string parsePassword(std::string args);




#endif