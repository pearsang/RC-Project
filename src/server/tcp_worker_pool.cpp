#include "tcp_worker_pool.hpp"

#include <iostream>
#include <unistd.h>

Worker::Worker() { thread = std::thread(&Worker::execute, this); }

Worker::~Worker() {
  lock.lock();
  is_exiting = true;
  cond.notify_one();
  lock.unlock();
  thread.join();
}

void Worker::execute() {
  while (!is_exiting) {
    std::unique_lock<std::mutex> unique_lock(lock);
    try {
      while (!to_execute && !is_exiting) {
        cond.wait(unique_lock);
      }

      if (is_exiting) {
        return;
      }

      std::string packet_id = read_packet_id(tcpSocketFD);

      pool->state.callTcpPacketHandler(packet_id, tcpSocketFD);

    } catch (InvalidPacketException &e) {
      try {
        ErrorTcpPacket error_packet;
        error_packet.send(tcpSocketFD);
      } catch (...) {
        std::cerr << "Failed to reply with ERR packet" << std::endl;
      }
    } catch (std::exception &e) {
      std::cerr << "Worker number" << workerID
                << " encountered an exception while running: " << e.what()
                << std::endl;
    } catch (...) {
      std::cerr << "Worker number" << workerID
                << " encountered an unknown exception while running."
                << std::endl;
    }

    pool->state.verbose << "Worker number " << workerID
                        << " Closing connection..." << std::endl;
    close(tcpSocketFD);
    to_execute = false;
    pool->freeWorker(workerID);
  }
}

TcpWorkerPool::TcpWorkerPool(AuctionServerState &auctionState)
    : state{auctionState} {
  for (uint32_t i = 0; i < POOL_SIZE; ++i) {
    busy_threads[i] = false;
    workers[i].pool = this;
    workers[i].workerID = i;
  }
}

void TcpWorkerPool::giveConnection(int fd) {
  std::scoped_lock<std::mutex> slock(busy_threads_lock);

  for (size_t i = 0; i < POOL_SIZE; ++i) {
    if (!busy_threads[i]) {
      // Found an available worker!
      std::scoped_lock<std::mutex> worker_lock(workers[i].lock);

      busy_threads[i] = true;
      workers[i].tcpSocketFD = fd;
      workers[i].to_execute = true;

      workers[i].cond.notify_one();

      state.verbose << "Sent to worker number " << i << std::endl;
      return;
    }
  }

  throw AllWorkersBusyException();
}

void TcpWorkerPool::freeWorker(uint32_t workerID) {
  std::scoped_lock<std::mutex> slock(busy_threads_lock);
  if (workerID < POOL_SIZE) {
    busy_threads[workerID] = false;
  }
}

std::string read_packet_id(int fd) {
  char id[PACKET_ID_LEN + 1];
  size_t to_read = PACKET_ID_LEN;

  while (to_read > 0) {
    ssize_t n = read(fd, &id[PACKET_ID_LEN - to_read], to_read);
    if (n <= 0) {
      std::cerr << "Invalid packet ID received" << std::endl;
      throw InvalidPacketException();
    }
    to_read -= (size_t)n;
  }

  id[PACKET_ID_LEN] = '\0';
  return std::string(id);
}