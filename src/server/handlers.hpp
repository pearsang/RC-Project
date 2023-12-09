#pragma once
#ifndef HANDLERS_H
#define HANDLERS_H

#include <sstream>

#include "../utils/constants.hpp"
#include "server_state.hpp"
#include "server_user.hpp"

// UDP handlers

/**
 * @brief Handles a login request.
 *
 * @param state The current server state.
 * @param buf The packet buffer.
 * @param addressFrom The address of the sender.
 */
void handleLogin(AuctionServerState &state, std::stringstream &buf,
                 SocketAddress &addressFrom);

/**
 * @brief Handles a logout request.
 *
 * @param state The current server state.
 * @param buf The packet buffer.
 * @param addressFrom The address of the sender.
 */
void handleLogout(AuctionServerState &state, std::stringstream &buf,
                  SocketAddress &addressFrom);

/**
 * @brief Handles an unregister request.
 *
 * @param state The current server state.
 * @param buf The packet buffer.
 * @param addressFrom The address of the sender.

*/
void handleUnregister(AuctionServerState &state, std::stringstream &buf,
                      SocketAddress &addressFrom);

/**
 *  @brief Handles a list user auctions request.
 *
 * @param state The current server state.
 * @param buf The packet buffer.
 * @param addressFrom The address of the sender.
 */
void handleListUserAuctions(AuctionServerState &state, std::stringstream &buf,
                            SocketAddress &addressFrom);

/**
 * @brief Handles a list user bids request.
 *
 * @param state The current server state.
 * @param buf The packet buffer.
 * @param addressFrom The address of the sender.
 */
void handleListUserBids(AuctionServerState &state, std::stringstream &buf,
                        SocketAddress &addressFrom);

/**
 * @brief Handles a list auction request.
 *
 * @param state The current server state.
 * @param buf The packet buffer.
 * @param addressFrom The address of the sender.
 */
void handleListAuctions(AuctionServerState &state, std::stringstream &buf,
                        SocketAddress &addressFrom);

/**
 * @brief Handles a show record of an aution request.
 *
 * @param state The current server state.
 * @param buf The packet buffer.
 * @param addressFrom The address of the sender.
 */
void handleShowRecord(AuctionServerState &state, std::stringstream &buf,
                      SocketAddress &addressFrom);

// TCP handlers

/**
 * @brief Handles an open auction request.
 *
 * @param state The current server state.
 * @param buf The packet buffer.
 * @param addressFrom The address of the sender.
 */
void handleOpenAuction(AuctionServerState &state, int fd);
/**
 * @brief Handles a close auction request.
 *
 * @param state The current server state.
 * @param buf The packet buffer.
 * @param addressFrom The address of the sender.
 */
void handleCloseAuction(AuctionServerState &state, int fd);

/**
 * @brief Handles a show auction asset request.
 *
 * @param state The current server state.
 * @param buf The packet buffer.
 * @param addressFrom The address of the sender.
 */
void handleShowAsset(AuctionServerState &state, int fd);

/**
 * @brief Handles an auction bid request.
 *
 * @param state The current server state.
 * @param buf The packet buffer.
 * @param addressFrom The address of the sender.
 */
void handleBid(AuctionServerState &state, int fd);

#endif