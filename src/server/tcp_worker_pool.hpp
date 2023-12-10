#ifndef TCPWORKER_POOL_H
#define TCP_WORKER_POOL_H

#include <condition_variable>
#include <mutex>
#include <thread>

#include "../utils/constants.hpp"
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
  bool busy_threads[POOL_SIZE]; // change name to busy_workers
  std::mutex busy_threads_lock; // change name to busy_workers_lock

public:
  AuctionServerState &state;

  TcpWorkerPool(AuctionServerState &_state);
  void giveConnection(int fd);
  void freeWorker(uint32_t workerID);
};

std::string read_packet_id(int fd);

class AllWorkersBusyException : public std::runtime_error {
public:
  AllWorkersBusyException()
      : std::runtime_error("All TCP workers are busy, connection will not be") {
  }
};

#endif