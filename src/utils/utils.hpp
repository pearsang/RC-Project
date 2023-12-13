#ifndef UTILS_H
#define UTILS_H

#include <csignal>
#include <cstring>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "constants.hpp"

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
 * signal. It sets the global variable `is_exiting` to true, which is
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

/**
 * @brief Checks if a string contains only digits.
 *
 * @param str A reference to a string to be checked.
 *
 * @return True if the string contains only digits, false otherwise.
 */
bool is_digits(const std::string &str);

/**
 * @brief Checks if a string is alphanumeric.
 *
 * @param str A reference to a string to be checked.
 *
 * @return True if the string is alphanumeric, false otherwise.
 */
bool is_alphanumeric(const std::string &str);

/**
 * @brief Parses the given string into a vector of arguments.
 *
 * @param args A reference to a string containing the arguments.
 */
std::vector<std::string> parse_args(std::string args);

/**
 * @brief Creates a directory with the given path.
 *
 * @param path A reference to a string containing the path of the directory to
 * be created.
 */
void create_new_directory(const std::string &path);

/**
 * @brief Creates a file with the given path.
 *
 * @param path A reference to a string containing the path of the file to be
 * created.
 */
void create_new_file(const std::string &path);

/**
 * @brief Deletes a file with the given path.
 *
 * @param path A reference to a string containing the path of the file to be
 * deleted.
 */
void delete_file(const std::string &path);

/**
 * @brief Deletes a directory with the given path.
 *
 * @param path A reference to a string containing the path of the directory to
 * be deleted.
 */
void delete_directory(const std::string &path);

/**
 * @brief Checks if a directory exists.
 *
 * @param path A reference to a string containing the path of the file to be
 * checked.
 *
 * @return Return -1 if the directory does not exist, 0 if it exists.
 */
int8_t directory_exists(const std::string &path);

/**
 * @brief Checks if a file exists.
 *
 * @param path A reference to a string containing the path of the file to be
 * checked.
 *
 * @return Return -1 if the file does not exist, 0 if it exists.
 */
int8_t file_exists(const std::string &path);

/**
 * @brief writes text on a file
 *
 * @param path A reference to a string containing the path of the file to be
 * written.
 * @param text A reference to a string containing the text to be written.
 */
void write_to_file(const std::string &path, const std::string &text);

/**
 * @brief reads text from a file
 *
 * @param path A reference to a string containing the path of the file to be
 * read.
 * @param text A reference to a string containing the text to be read.
 */
void read_from_file(const std::string &path, std::string &text);

/**
 *
 * @brief Validates the user ID from the given string.
 *
 */
int8_t validateUserID(std::string userID);

/**
 *
 * @brief Validate the user password from the given string.
 *
 */
int8_t validatePassword(std::string password);

/**
 *
 * @brief Validate the auction ID from the given string.
 *
 */
int8_t validateAuctionID(std::string auctionId);

/**
 *
 * @brief Validate the bid value from the given string.
 *
 */
int8_t validateBidValue(std::string bidValue);

/**
 *
 * @brief Validate the asset filename from the given string.
 */
int8_t validateAssetFilename(std::string assetFilename);

/**
 *
 * @brief Validate the start value from the given string.
 *
 */
int8_t validateStartValue(std::string startValue);

/**
 *
 * @brief Validate the auction duration from the given string.
 *
 */
int8_t validateAuctionDuration(std::string auctionDuration);

/**
 *
 * @brief Validate the asset name from the given string.
 *
 */
int8_t validateAuctionName(std::string assetName);

/**
 *
 * @brief Validate the file size of an asset.
 *
 */
int8_t validateAssetFileSize(std::string ass);

/**
 * @brief Validate a file based on its size.
 */
int8_t validateFileSize(std::string file_path);

std::string getUserPassword(std::string userID);

std::string getCurrentTimeFormated();

std::string getStartFullTime();

std::string intToStringWithZeros(int number);
#endif