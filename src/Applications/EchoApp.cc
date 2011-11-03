// Author: Thaddeus Diamond (diamond@cs.yale.edu)
//
// This is the implementation for our sample echo app

#include "Applications/EchoApp.h"

bool EchoApp::Start() {
  assert(CreateMobileNodeDelegate() == 0);
  if (!ConnectToPeer())
    return false;

  Signal::HandleSignalInterrupts();
  do {
    PrintReceivedData();
    EchoMessage(keyword_);
    sleep(1);
  } while (Signal::ShouldContinue());
  return true;
}

bool EchoApp::ShutDown(const char* format, ...) {
  fprintf(stderr, "Shutting Down Echo App...\n");

  va_list arguments;
  va_start(arguments, format);

  close(app_socket_);
  mobile_node_->ShutDown(format, arguments);
  Signal::ExitProgram(0);

  fprintf(stderr, "OK\n");
  return false;
}

bool EchoApp::ConnectToPeer() {
  // Make a vanilla socket
  app_socket_ = socket(domain_, transport_layer_, protocol_);
  if (app_socket_ < 0)
    return ShutDown("Could not create a socket");

  // Set the socket to be reusable
  int on = 1;
  if (setsockopt(app_socket_, SOL_SOCKET, SO_REUSEADDR,
                 reinterpret_cast<char*>(&on), sizeof(on)) < 0)
    return ShutDown("Could not make the socket reusable");

  // Set the socket to be non-blocking
  int opts;
  if ((opts = fcntl(app_socket_, F_GETFL)) < 0)
    return ShutDown("Error getting the socket options");
  if (fcntl(app_socket_, F_SETFL, opts | O_NONBLOCK) < 0)
    return ShutDown("Error setting the socket to nonblocking");

  // Connect to the peer with registration at the mobile node daemon
  fprintf(stderr, "Connecting to peer, please do not disconnect...\n");
  peer_info_ = mobile_node_->RegisterPeer(app_socket_, peer_addr_, peer_port_);
  if (peer_info_ == NULL)
    return ShutDown("There was an error connecting to the peer");

  fprintf(stderr, " OK.\n");
  return true;
}

bool EchoApp::PrintReceivedData() {
  struct sockaddr_in request_src;
  socklen_t request_src_size = sizeof(request_src);

  char buffer[4096];
  memset(buffer, 0, sizeof(buffer));
#ifdef UDP_APPLICATION
  int bytes_read = recvfrom(app_socket_, buffer, sizeof(buffer), 0,
                            reinterpret_cast<struct sockaddr*>(&request_src),
                            &request_src_size);
#endif
#ifdef TCP_APPLICATION
  int bytes_read = -1;
  return ShutDown("TCP is not yet supported in the application");
#endif

  // Received a communication
  if (bytes_read > 0) {
    // Echo back the peer's communication
    if (string(buffer) != keyword_) {
      fprintf(stderr, "Received message '%s' from %d:%d\n", buffer,
              request_src.sin_addr.s_addr, ntohs(request_src.sin_port));
#ifdef UDP_APPLICATION
      EchoMessage(buffer, reinterpret_cast<struct sockaddr*>(&request_src));
#endif
#ifdef TCP_APPLICATION
      request_src_size = -1;
#endif

    // Received back our own, bury it...
    } else {
      fprintf(stderr, "Received back our own communication. Burying...\n");
    }

  // An error occurred in reading
  } else if (bytes_read < 0 && errno != EWOULDBLOCK && errno != EAGAIN) {
    return ShutDown("Error listening on socket");
  }

  return true;
}

bool EchoApp::EchoMessage(string message, struct sockaddr* peer_info) {
  peer_info = (peer_info == NULL ? peer_info_ : peer_info);
  socklen_t peer_size = sizeof(*peer_info);

#ifdef UDP_APPLICATION
  sendto(app_socket_, message.c_str(), message.length() + 1, 0,
         peer_info, peer_size);
#endif
#ifdef TCP_APPLICATION
  return ShutDown("TCP is not yet supported in the DNS");
#endif

  // Report out to the user that we're sending
  fprintf(stderr, "Sending %s to our friend...\n", message.c_str());
  return true;
}

int EchoApp::CreateMobileNodeDelegate() {
  pthread_t mobile_node_daemon;
  mobile_node_ = new SimpleMobileNode(dns_server_, dns_port_);

  int thread_status = pthread_create(&mobile_node_daemon, NULL,
                                     &RunMobileAgentThread, mobile_node_);
  return thread_status;
}
