#ifndef PROTOCOL_HPP
#define PROTOCOL_HPP

#include <sstream>
#include <string>

/**
 * @class UdpPacket
 *
 * @brief Represents a base class for implementing UDP packets.
 *
 */
class UdpPacket {

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
   * @param buffer The stringstream to deserialize from.
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

#endif