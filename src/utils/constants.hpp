#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <cmath>

// Default values
#define DEFAULT_HOSTNAME "tejo.tecnico.ulisboa.pt"
#define DEFAULT_PORT "58011"
#define INVALID (int8_t) - 1

// Protocol constants
#define USER_ID_LENGTH 6
#define USER_ID_MAX ((uint32_t)pow(10, USER_ID_LENGTH) - 1)
#define AUCTION_ID_LENGTH 3
#define FILENAME_MAX_LENGTH 24
#define ASSET_NAME_MAX 10
#define START_VALUE_MAX 6
#define AUCTION_DURATION_MAX 5
#define FILESIZE_MAX 8

// UDP constants
#define UDP_RESEND_TRIES 3
#define UDP_TIMEOUT_SECONDS 5
#define SOCKET_BUFFER_LEN 8192

// TCP constants
#define TCP_WRITE_TIMEOUT_SECONDS (20 * 60)
#define TCP_READ_TIMEOUT_SECONDS 15

// Server constants
#define SERVER_TIMEOUT 3
#define EXCEPTION_RETRY_MAX_TRIALS 3
#define PACKET_ID_LEN 3
#define FILE_BUFFER_LEN 512

// TCP thread management
#define POOL_SIZE 50
#define TCP_MAX_CONNECTIONS 5

// Directories
#define ASDIR "AS-DB"
#define USERDIR "AS-DB/USERS"
#define AUCTIONDIR "AS-DB/AUCTIONS"

// Commands arguments number
#define LOGIN_ARGS_NUM 2
#define LOGOUT_ARGS_NUM 0
#define UNREGISTER_ARGS_NUM 0
#define OPEN_AUCTION_ARGS_NUM 4
#define CLOSE_AUCTION_ARGS_NUM 1
#define LIST_USER_AUCTIONS_ARGS_NUM 0
#define LIST_USER_BIDS_ARGS_NUM 0
#define LIST_AUCTIONS_ARGS_NUM 0
#define SHOW_ASSETS_ARGS_NUM 0
#define BID_ARGS_NUM 2
#define SHOW_RECORD_ARGS_NUM 1

#endif
