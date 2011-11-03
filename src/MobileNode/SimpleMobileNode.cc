// Author: Thaddeus Diamond (diamond@cs.yale.edu)
//
// This is a simple implementation of the mobile node agent in our rendezvous
// server scheme

#include "MobileNode/SimpleMobileNode.h"

void SimpleMobileNode::Start() {
  do {
  } while (true);
}

bool SimpleMobileNode::ShutDown(const char* format, ...) {
  fprintf(stderr, "Shutting Down Mobile Node...\n");

  va_list arguments;
  va_start(arguments, format);

  Signal::ExitProgram(0);

  fprintf(stderr, "OK\n");
  return false;
}

struct sockaddr* SimpleMobileNode::RegisterPeer(int app_socket,
                                                LogicalAddress peer_addr,
                                                unsigned short peer_port) {
  return NULL;
}
