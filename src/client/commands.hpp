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
#include "./utils/protocol.hpp"
#include "user_state.hpp"

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
   *
   * @param userState The current user state.
   */
  void waitForCommand(UserState &userState);

  /**
   * @brief Registers a command handler with the manager.
   *
   * @param handler The command handler to register.
   */
  void addCommand(std::shared_ptr<CommandHandler> handler);
};

/**
 * @class CommandHandler
 *
 * @brief Represents a base class for implementing command-specific behavior.
 *
 * CommandHandler define the properties and behavior of specific commands that
 * the CommandManager can handle. Each CommandHandler has a unique name and
 * optionally an alias.
 */
class CommandHandler {
protected:
  /**
   * @brief Constructs a CommandHandler with specified attributes.
   *
   * @param name The unique name of the command.
   * @param alias An optional alias for the command.
   */
  CommandHandler(const char *__name, const std::optional<const char *> __alias)
      : name{__name}, alias{__alias} {}

public:
  const char *name;
  const std::optional<const char *> alias;
  virtual void handleCommand(std::string args, UserState &userState) = 0;
};

/**
 * @class LoginCommand
 *
 * @param args The arguments passed to the command.
 * @param userState The current user state.
 *
 * @brief Represents a command for logging in to the server.
 *
 */
class LoginCommand : public CommandHandler {
  void handleCommand(std::string args, UserState &userState);

public:
  LoginCommand() : CommandHandler("login", std::nullopt) {}
};

/**
 * @class LogoutCommand
 *
 * @param args The arguments passed to the command.
 * @param userState The current user state.
 *
 * @brief Represents a command for logging out of the server.
 *
 */
class LogoutCommand : public CommandHandler {
  void handleCommand(std::string args, UserState &userState);

public:
  LogoutCommand() : CommandHandler("logout", std::nullopt) {}
};

/**
 * @class UnregisterCommand
 *
 * @param args The arguments passed to the command.
 * @param userState The current user state.
 *
 * @brief Represents a command for unregistering from the server.
 */
class UnregisterCommand : public CommandHandler {
  void handleCommand(std::string args, UserState &userState);

public:
  UnregisterCommand() : CommandHandler("unregister", std::nullopt) {}
};

/**
 * @class ExitCommand
 *
 * @param args The arguments passed to the command.
 * @param userState The current user state.
 *
 * @brief Represents a command for exiting the application.
 */
class ExitCommand : public CommandHandler {
  void handleCommand(std::string args, UserState &userState);

  // The Exit command has no usage
public:
  ExitCommand() : CommandHandler("exit", std::nullopt) {}
};

/**
 * @class OpenAuctionCommand
 *
 * @param args The arguments passed to the command.
 * @param userState The current user state.
 *
 * @brief Represents a command for opening a new auction.
 */
class OpenAuctionCommand : public CommandHandler {
  void handleCommand(std::string args, UserState &userState);

public:
  OpenAuctionCommand() : CommandHandler("open", std::nullopt) {}
};

/**
 * @class CloseAuctionCommand
 *
 * @param args The arguments passed to the command.
 * @param userState The current user state.
 *
 * @brief Represents a command for closing an auction.
 */
class CloseAuctionCommand : public CommandHandler {
  void handleCommand(std::string args, UserState &userState);

public:
  CloseAuctionCommand() : CommandHandler("close", std::nullopt) {}
};

/**
 * @class ListUserAuctionsCommand
 *
 * @param args The arguments passed to the command.
 * @param userState The current user state.
 *
 * @brief Represents a command for listing all auctions for a user.
 */
class ListUserAuctionsCommand : public CommandHandler {
  void handleCommand(std::string args, UserState &userState);

public:
  ListUserAuctionsCommand() : CommandHandler("myauctions", "ma") {}
};

/**
 * @class ListUserBidsCommand
 *
 * @param args The arguments passed to the command.
 * @param userState The current user state.
 *
 * @brief Represents a command for listing all auctions that the user bidded.
 */
class ListUserBidsCommand : public CommandHandler {
  void handleCommand(std::string args, UserState &userState);

public:
  ListUserBidsCommand() : CommandHandler("mybids", "mb") {}
};

/**
 * @class ListAuctionsCommand
 *
 * @param args The arguments passed to the command.
 * @param userState The current user state.
 *
 * @brief Represents a command for listing all active auctions.
 */
class ListAuctionsCommand : public CommandHandler {
  void handleCommand(std::string args, UserState &userState);

public:
  ListAuctionsCommand() : CommandHandler("list", "l") {}
};

/**
 * @class ShowAssetCommand
 *
 * @param args The arguments passed to the command.
 * @param userState The current user state.
 *
 * @brief Represents a command for showing the image of the asset of an
 * auction.
 */
class ShowAssetCommand : public CommandHandler {
  void handleCommand(std::string args, UserState &userState);

public:
  ShowAssetCommand() : CommandHandler("show_asset", "sa") {}
};

/**
 * @class BidCommand
 *
 * @param args The arguments passed to the command.
 * @param userState The current user state.
 *
 * @brief Represents a command for bidding on an auction.
 */
class BidCommand : public CommandHandler {
  void handleCommand(std::string args, UserState &userState);

public:
  BidCommand() : CommandHandler("bid", "b") {}
};

/**
 * @class ShowRecordCommand
 *
 * @param args The arguments passed to the command.
 * @param userState The current user state.
 *
 * @brief Represents a command for showing the record of an auction.
 */
class ShowRecordCommand : public CommandHandler {
  void handleCommand(std::string args, UserState &userState);

public:
  ShowRecordCommand() : CommandHandler("show_record", "sr") {}
};

#endif
