#ifndef PROTOCOL_HPP
#define PROTOCOL_HPP

#include <sstream>
#include <string>
#include <sys/socket.h>

#include <cstdint>
#include <filesystem>
#include <memory>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <vector>

/**
 * @class UnexpectedPacketException
 *
 * @brief Represents an exception thrown when an unexpected packet is received.
 *
 */
class UnexpectedPacketException : public std::runtime_error {
public:
  UnexpectedPacketException()
      : std::runtime_error(
            "The server did not reply with the expected response, so it was "
            "ignored. Please try again or connect to a different game "
            "server.") {}
};

/**
 * @class InvalidPacketException
 *
 * @brief Represents an exception thrown when an invalid packet is received.
 *
 */
class InvalidPacketException : public std::runtime_error {
public:
  InvalidPacketException()
      : std::runtime_error(
            "The response given by the server is not correctly structured, so "
            "it was ignored. Please try again or connect to a different game "
            "server.") {}
};

/**
 * @class ConnectionTimeoutException
 *
 * @brief Represents an exception thrown when a connection timeout occurs.
 *
 */
class ConnectionTimeoutException : public std::runtime_error {
public:
  ConnectionTimeoutException()
      : std::runtime_error("Could not connect to the auction server, please "
                           "check your internet "
                           "connection and try again.") {}
};

/**
 * @class OperationCancelledException
 *
 * @brief Represents an exception thrown when an operation is cancelled by the
 * user.
 *
 */
class OperationCancelledException : public std::runtime_error {
public:
  OperationCancelledException()
      : std::runtime_error("Operation cancelled by user") {}
};

/**
 * @class PacketDeserializationException
 *
 * @brief Represents an exception thrown when a packet could not be
 * deserialized.
 *
 */
class PacketSerializationException : public std::runtime_error {
public:
  PacketSerializationException()
      : std::runtime_error(
            "There was an error while preparing a request, "
            "please try again and restart the application if the problem "
            "persists.") {}
};

/**
 * @class UdpPacket
 *
 * @brief Represents a base class for implementing UDP packets.
 *
 */
class UdpPacket {

private:
  /**
   * @brief Reads a character from the buffer and checks if it is equal to the
   * given character.
   *
   * @param buffer The buffer to read from.
   * @param chr The character to check for.
   */
  void readChar(std::stringstream &buffer, char chr);

protected:
  /**
   * @brief Reads the packet ID from the buffer and checks if it is equal to the
   * given ID.
   *
   * @param buffer The buffer to read from.
   * @param id The ID to check for.
   */
  void readPacketId(std::stringstream &buffer, const char *id);

  /**
   * @brief Reads a space character from the buffer.
   *
   * @param buffer The buffer to read from.
   */
  void readSpace(std::stringstream &buffer);

  /**
   * @brief Reads a character from the buffer.
   *
   * @param buffer The buffer to read from.
   * @return The character that was read.
   */
  char readChar(std::stringstream &buffer);

  /**
   * @brief Reads an alphabetical character from the buffer.
   *
   * @param buffer The buffer to read from.
   * @return The character that was read.
   */
  char readAlphabeticalChar(std::stringstream &buffer);

  /**
   * @brief Reads the packet delimiter from the buffer.
   *
   * @param buffer The buffer to read from.
   */
  void readPacketDelimiter(std::stringstream &buffer);

  /**
   * @brief Reads a string from the buffer.
   *
   * @param buffer The buffer to read from.
   * @param max_len The maximum length of the string.
   * @return The string that was read.
   */
  std::string readString(std::stringstream &buffer, uint32_t max_len);

  /**
   * @brief Reads an alphabetical string from the buffer.
   *
   * @param buffer The buffer to read from.
   * @param max_len The maximum length of the string.
   * @return The string that was read.
   */
  std::string readAlphabeticalString(std::stringstream &buffer,
                                     uint32_t max_len);

  /**
   * @brief Reads an integer from the buffer.
   *
   * @param buffer  The buffer to read from.
   * @return The integer that was read.
   */
  uint32_t readInt(std::stringstream &buffer);

public:
  /**
   * @brief Serializes the packet into a stringstream.
   *
   * @return The serialized packet.
   */
  virtual std::stringstream serialize() = 0;

  /**
   * @brief Deserializes the packet from a stringstream.
   *
   * @param buffer The stringstream containing the packet data.
   */
  virtual void deserialize(std::stringstream &buffer) = 0;

  /**
   * @brief Destroys the UdpPacket object.
   *
   */
  virtual ~UdpPacket() = default;
};

/**
 * @class LoginRequest
 *
 * @brief Represents a UDP packet for logging in to the server.
 * The packet has the following format:
 * LIN <user_id> <password>
 *
 */
class LoginRequest : public UdpPacket {
public:
  static constexpr const char *ID = "LIN";
  std::string userID;
  std::string password;

  std::stringstream serialize();
  void deserialize(std::stringstream &buffer);
};

/**
 * @class LoginResponse
 *
 * @brief Represents a UDP packet for responding to a login request.
 * The packet has the following format:
 * RLI <status>
 *
 */
class LoginResponse : public UdpPacket {
public:
  enum status { OK, NOK, REG, ERR };
  static constexpr const char *ID = "RLI";
  status status;

  std::stringstream serialize();
  void deserialize(std::stringstream &buffer);
};

/**
 * @class LogoutRequest
 *
 * @brief Represents a UDP packet for logging out of the server.
 * The packet has the following format:
 * LOU <user_id> <password>
 *
 */
class LogoutRequest : public UdpPacket {
public:
  static constexpr const char *ID = "LOU";
  std::string userID;
  std::string password;

  std::stringstream serialize();
  void deserialize(std::stringstream &buffer);
};

/**
 * @class UnregisterRequest
 *
 * @brief Represents a UDP packet for unregistering from the server.
 * The packet has the following format:
 * UNR <user_id> <password>
 *
 */
class UnregisterRequest : public UdpPacket {
public:
  static constexpr const char *ID = "UNR";
  std::string userID;
  std::string password;

  std::stringstream serialize();
  void deserialize(std::stringstream &buffer);
};

/**
 * @class ListUserAuctionsRequest
 *
 * @brief Represents a UDP packet for listing the auctions of a user.
 * The packet has the following format:
 * LMA <user_id>
 *
 */
class ListUserAuctionsRequest : public UdpPacket {
public:
  static constexpr const char *ID = "LMA";
  std::string userID;

  std::stringstream serialize();
  void deserialize(std::stringstream &buffer);
};

/**
 * @class ListUserBidsRequest
 *
 * @brief Represents a UDP packet for listing the bids of a user.
 * The packet has the following format:
 * LMB <user_id>
 *
 */
class ListUserBidsRequest : public UdpPacket {
public:
  static constexpr const char *ID = "LMB";
  std::string userID;

  std::stringstream serialize();
  void deserialize(std::stringstream &buffer);
};

/**
 * @class ListAuctionsRequest
 *
 * @brief Represents a UDP packet for listing all auctions.
 * The packet has the following format:
 * LST
 *
 */
class ListAuctionsRequest : public UdpPacket {
public:
  static constexpr const char *ID = "LST";

  std::stringstream serialize();
  void deserialize(std::stringstream &buffer);
};

/**
 * @class ShowAssetRequest
 *
 * @brief Represents a UDP packet for showing the asset of an auction.
 * The packet has the following format:
 * SHT <auction_id>
 *
 */
class ShowRecordRequest : public UdpPacket {
public:
  static constexpr const char *ID = "SRC";
  std::string auctionID;

  std::stringstream serialize();
  void deserialize(std::stringstream &buffer);
};

/**
 * @brief Sends a UDP packet.
 *
 * @param packet The packet to send.
 * @param socket The socket to send the packet on.
 * @param address The address to send the packet to.
 * @param addrlen The length of the address.
 */
void send_packet(UdpPacket &packet, int socket, struct sockaddr *address,
                 socklen_t addrlen);

/**
 * @brief waits for a UDP packet.
 *
 * @param packet  The packet to receive.
 * @param socket  The socket to receive the packet on.
 */
void wait_for_packet(UdpPacket &packet, int socket);

#endif