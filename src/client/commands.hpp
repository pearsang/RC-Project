#ifndef COMMANDS_H
#define COMMANDS_H

#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>


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


#endif