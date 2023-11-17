#include "utils.hpp"

#include <csignal>

// Flag to indicate whether the application is shutting down
bool is_shutting_down = false;

void validate_port_number(const std::string &port_number) {
  // Ensure that the port number is a valid number
  for (char c : port_number) {
    if (!std::isdigit(static_cast<unsigned char>(c))) {
      throw FatalError("Invalid port: not a number");
    }
  }

  try {
    // Attempt to convert the string to an integer
    int32_t port_number_int = std::stoi(port_number);
    // Ensure that the port number is within the valid range
    if (port_number_int < 1 || port_number_int > 65535) {
      throw std::runtime_error("");
    }
  } catch (...) {
    throw FatalError("Invalid port: it must be a number between 1 and 65535");
  }
}

void setup_custom_signal_handlers() {
  struct sigaction signal_action;
  // set the custom signal handler
  signal_action.sa_handler = terminate_signal_handler;
  // clear the signal set, so that no other signals are blocked
  sigemptyset(&signal_action.sa_mask);
  // no special flags
  signal_action.sa_flags = 0;

  if (sigaction(SIGINT, &signal_action, NULL) == -1) {
    throw FatalError("Setting SIGINT signal handler", errno);
  }
  if (sigaction(SIGTERM, &signal_action, NULL) == -1) {
    throw FatalError("Setting SIGTERM signal handler", errno);
  }

  // ignore SIGPIPE
  signal(SIGPIPE, SIG_IGN);
}

void terminate_signal_handler(int sig) {
  // ignore the signal if the application is already shutting down
  (void)sig;
  if (is_shutting_down) {
    exit(EXIT_SUCCESS);
  }
  is_shutting_down = true;
}