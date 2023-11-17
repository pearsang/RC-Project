#ifndef UTILS_H
#define UTILS_H

#include <cstring>
#include <stdexcept>

/**
 * @class FatalError
 *
 * @brief Represents a fatal error that can be thrown during runtime.
 *
 * This class is derived from std::runtime_error and is used to signal fatal
 * and unrecoverable errors within the application. It allows specifying an
 * optional error number to provide additional details about the error.
 */
class FatalError : public std::runtime_error {
public:
  /**
   * @brief Constructs a new FatalError object.
   *
   * @param cause A string containing the cause of the error.
   */
  FatalError(const std::string &cause) : std::runtime_error(cause) {}

  /**
   * @brief Constructs a new FatalError object.
   *
   * @param cause A string containing the cause of the error.
   * @param _errno The error number.
   */
  FatalError(const std::string &cause, const int _errno)
      : std::runtime_error(cause + ": " + strerror(_errno)) {}
};

/**
 * @brief Sets up the signal handlers.
 *
 * This function sets up the signal handlers for SIGINT, SIGTERM and SIGPIPE.
 * SIGINT and SIGTERM are handled by the `terminate_signal_handler` function,
 * while SIGPIPE is ignored.
 *
 * @throws FatalError If an error occurs while setting up the signal handlers.
 */
void setup_custom_signal_handlers();

/**
 * @brief Handles the SIGINT and SIGTERM signals.
 *
 * This function is called when the application receives a SIGINT or SIGTERM
 * signal. It sets the global variable `is_shutting_down` to true, which is
 * checked by the main loop to determine whether the application should shut
 * down.
 *
 * @param sig The signal number.
 */
void terminate_signal_handler(const int sig);

/**
 * @brief Validates the given port number.
 *
 * This function ensures that the provided string is a valid port number,
 * consisting only of digits and falls within the valid port range (1 to
 * 65535).
 *
 * @param port_number A reference to a string representing the port number to be
 * validated.
 *
 * @throws FatalError If the port number is not a valid number or is
 * out of range.
 */
void validate_port_number(const std::string &port_number);

#endif