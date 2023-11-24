#include "protocol.hpp"

#include <sys/types.h>
#include <unistd.h>

#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

#include "../utils/utils.hpp"

std::stringstream LoginRequest::serialize() {
  std::stringstream buffer;
  buffer << LoginRequest::ID << " " << this->userID << " " << this->password
         << std::endl;
  return buffer;
}

void LoginRequest::deserialize(std::stringstream &buffer) {
  buffer >> this->userID >> this->password;
}

std::stringstream LogoutRequest::serialize() {
  std::stringstream buffer;
  buffer << LogoutRequest::ID << " " << this->userID << " " << this->password
         << std::endl;
  return buffer;
}

void LogoutRequest::deserialize(std::stringstream &buffer) {
  buffer >> this->userID >> this->password;
}

std::stringstream UnregisterRequest::serialize() {
  std::stringstream buffer;
  buffer << UnregisterRequest::ID << " " << this->userID << " "
         << this->password << std::endl;
  return buffer;
}

void UnregisterRequest::deserialize(std::stringstream &buffer) {
  buffer >> this->userID >> this->password;
}

std::stringstream ListUserAuctionsRequest::serialize() {
  std::stringstream buffer;
  buffer << ListUserAuctionsRequest::ID << " " << this->userID << std::endl;
  return buffer;
}

void ListUserAuctionsRequest::deserialize(std::stringstream &buffer) {
  buffer >> this->userID;
}

std::stringstream ListUserBidsRequest::serialize() {
  std::stringstream buffer;
  buffer << ListUserBidsRequest::ID << " " << this->userID << std::endl;
  return buffer;
}

void ListUserBidsRequest::deserialize(std::stringstream &buffer) {
  buffer >> this->userID;
}

std::stringstream ListAuctionsRequest::serialize() {
  std::stringstream buffer;
  buffer << ListAuctionsRequest::ID << std::endl;
  return buffer;
}

void ListAuctionsRequest::deserialize(std::stringstream &buffer) {}

std::stringstream ShowRecordRequest::serialize() {
  std::stringstream buffer;
  buffer << ShowRecordRequest::ID << " " << this->auctionID << std::endl;
  return buffer;
}

void ShowRecordRequest::deserialize(std::stringstream &buffer) {
  buffer >> this->auctionID;
}
