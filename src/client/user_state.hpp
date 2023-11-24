#ifndef USER_STATE_H
#define USER_STATE_H

#include <netdb.h>
#include <string>

#include "../utils/protocol.hpp"

/**
 * @class UserState
 *
 * @brief Represents the state of a user.
 *
 * The UserState class encapsulates the state of a user, including the user's
 * username, password, and UDP and TCP handles.
 */
class UserState {
  std::string userID;
  std::string password;
  int udpSocketFD = -1;
  int tcpSocketFD = -1;
  struct addrinfo *serverUdpAddr = NULL;
  struct addrinfo *serverTcpAddr = NULL;

  /**
   * @brief Sets up a UDP socket.
   *
   */
  void setupUdpSocket();

  /**
   * @brief Resolves the server address.
   *
   * @param hostname The hostname of the server.
   * @param port The port of the server.
   */
  void resolveServerAddress(std::string &hostname, std::string &port);

public:
  /**
   * @brief Constructs a new UserState object.
   *
   * @param hostname The hostname of the server.
   * @param port The port of the server.
   */
  UserState(std::string &hostname, std::string &port);

  /**
   * @brief Destroys the UserState object.
   *
   */
  ~UserState();

  /**
   * @brief Gets the UDP socket file descriptor.
   *
   * @return The UDP socket file descriptor.
   */
  int getUdpSocketFD() { return this->udpSocketFD; }

  /**
   * @brief Gets the TCP socket file descriptor.
   *
   * @return The TCP socket file descriptor.
   */
  int getTcpSocketFD() { return this->tcpSocketFD; }

  /**
   * @brief Gets the server UDP address.
   *
   * @return The server UDP address.
   */
  struct addrinfo *getServerUdpAddr() {
    return this->serverUdpAddr;
  }

  /**
   * @brief Gets the server TCP address.
   *
   * @return The server TCP address.
   */
  struct addrinfo *getServerTcpAddr() {
    return this->serverTcpAddr;
  }

  /**
   * @brief Gets the user ID.
   *
   * @return The user ID.
   */
  std::string getUserID() { return this->userID; }

  /**
   * @brief Gets the password.
   *
   * @return The password.
   */
  std::string getPassword() { return this->password; }

  /**
   * @brief Sets the user ID.
   *
   * @param userID The user ID.
   */
  void setUserID(std::string &userID) { this->userID = userID; }

  /**
   * @brief Sets the password.
   *
   * @param password The password.
   */
  void setPassword(std::string &password) { this->password = password; }
};

#endif