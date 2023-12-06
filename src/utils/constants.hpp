#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <cmath>

#define DEFAULT_HOSTNAME "tejo.tecnico.ulisboa.pt"
#define DEFAULT_PORT "58011"

#define INVALID (int8_t) - 1

#define USER_ID_LENGTH 6
#define USER_ID_MAX ((uint32_t)pow(10, USER_ID_LENGTH) - 1)
#define AUCTION_ID_LENGTH 3

#define UDP_RESEND_TRIES 3
#define UDP_TIMEOUT_SECONDS 5
#define SOCKET_BUFFER_LEN 8192

#define FILENAME_MAX_LENGTH 24
#define ASSET_NAME_MAX 10
#define START_VALUE_MAX 6
#define AUCTION_DURATION_MAX 5

#define FILESIZE_MAX 8
#define FILE_BUFFER_LEN 512

#define TCP_WRITE_TIMEOUT_SECONDS (20 * 60) // 20 minutes
#define TCP_READ_TIMEOUT_SECONDS 20
#define PROGRESS_BAR_STEP_SIZE 10

#endif
