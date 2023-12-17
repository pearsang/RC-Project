#ifndef TCPWORKER_POOL_H
#define TCP_WORKER_POOL_H

#include <condition_variable>
#include <mutex>
#include <thread>

#include "../utils/constants.hpp"
#include "../utils/protocol.hpp"
#include "server_state.hpp"

class TcpWorkerPool;

class Worker {
  std::thread thread;

  void execute();

public:
  int tcpSocketFD = -1;
  bool is_exiting = false;
  bool to_execute = false;
  TcpWorkerPool *pool;
  uint32_t workerID = 0;
  std::mutex lock;
  std::condition_variable cond;

  Worker();
  ~Worker();
};

class TcpWorkerPool {
  Worker workers[POOL_SIZE];
  bool busy_workers[POOL_SIZE];
  std::mutex busy_workers_lock;

public:
  AuctionServerState &state;

  TcpWorkerPool(AuctionServerState &auctionState);

  /**
   * @brief Attributes a socket to a worker that  is available.
   *
   * @param fd socket file descriptor.
   */
  void giveConnection(int fd);

  /**
   * @brief Frees a busy worker.
   *
   * @param workerID worker ID to be set free.
   */
  void freeWorker(uint32_t workerID);
};

/**
 * @brief Get the packet ID of a TCP message from a socket.
 *
 * @param fd TCP socket file descriptor.
 * @return packet ID.
 */
std::string read_packet_id(int fd);

class AllWorkersBusyException : public std::runtime_error {
public:
  AllWorkersBusyException()
      : std::runtime_error("All TCP workers are busy, connection will not be") {
  }
};

#endif