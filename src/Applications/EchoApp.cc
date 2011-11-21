/**
 * @file
 * @author Thaddeus Diamond <diamond@cs.yale.edu>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * This is the implementation for our sample echo app
 **/

#include "Applications/EchoApp.h"

bool EchoApp::Start() {
  assert(CreateMobileNodeDelegate() == 0);
  if (!ConnectToPeer())
    return false;

  sleep(5);
  Signal::HandleSignalInterrupts();
  do {
    PrintReceivedData();
    SendMessage(keyword_);
    sleep(1);
  } while (Signal::ShouldContinue());
  return true;
}

bool EchoApp::ShutDown(const char* format, ...) {
  Log(stderr, DEBUG, "Shutting Down Echo App (");

  va_list arguments;
  va_start(arguments, format);
  Log(stderr, WARNING, format, arguments);
  perror(")");

  close(app_socket_);
  mobile_node_->ShutDown(format, arguments);
  Signal::ExitProgram(0);

  Log(stderr, SUCCESS, "OK");
  return false;
}

bool EchoApp::ConnectToPeer() {
  // Make a vanilla socket
  app_socket_ = socket(domain_, transport_layer_, protocol_);
  if (app_socket_ < 0)
    return ShutDown("Could not create a socket");

  // Bind the socket to listen
  struct sockaddr_in server;
  memset(&server, 0, sizeof(server));
  server.sin_family = domain_;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(app_port_);

  if (bind(app_socket_, reinterpret_cast<struct sockaddr*>(&server),
           sizeof(server)))
    return ShutDown("Could not bind listening connection");

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
  Log(stderr, WARNING, "Connecting to peer, please do not disconnect...");
  peer_info_ = mobile_node_->RegisterPeer(app_socket_, peer_addr_);
  int backoff = 1;
  while (peer_info_ == NULL) {
    if (backoff > MAX_ATTEMPTS)
      return ShutDown("Peer lookup failed");

    sleep(backoff++);
    Log(stderr, ERROR, "Could not connect to peer, trying again.");
    peer_info_ = mobile_node_->RegisterPeer(app_socket_, peer_addr_);
  }

  // Modify sockaddr to have domain and a port
  struct sockaddr_in* peer_container =
    reinterpret_cast<struct sockaddr_in*>(peer_info_);
  peer_container->sin_family = domain_;
  peer_container->sin_port = htons(peer_port_);

  Log(stderr, SUCCESS, "OK");
  return true;
}

bool EchoApp::PrintReceivedData() {
  struct sockaddr_in request_src;
  socklen_t request_src_size = sizeof(request_src);

  char buffer[4096];
  memset(buffer, 0, sizeof(buffer));
#ifdef UDP_APPLICATION
  int bytes_read = recvfrom(app_socket_, buffer, sizeof(buffer), MSG_PEEK,
                            reinterpret_cast<struct sockaddr*>(&request_src),
                            &request_src_size);
#elif TCP_APPLICATION
  int bytes_read = -1;
  return ShutDown("TCP is not yet supported in the application");
#endif

  // Received a communication
  if (bytes_read > 0 && request_src.sin_addr.s_addr !=
      static_cast<unsigned int>(IPStringToInt(rendezvous_server_))) {
    // Clear the peek buffer
    recvfrom(app_socket_, buffer, sizeof(buffer), 0,
             reinterpret_cast<struct sockaddr*>(&request_src),
             &request_src_size);

    // Echo back the peer's communication
    if (string(buffer) != keyword_) {
      Log(stderr, SUCCESS, "Received message '%s' from %d:%d", &buffer[0],
          request_src.sin_addr.s_addr, ntohs(request_src.sin_port));
      SendMessage(buffer, reinterpret_cast<struct sockaddr*>(&request_src));

    // Received back our own, bury it...
    } else {
      Log(stderr, WARNING, "Received back our own communication. Burying...");
      received_ = true;
    }

  // An error occurred in reading
  } else if (bytes_read < 0 && errno != EWOULDBLOCK && errno != EAGAIN) {
    return ShutDown("Error listening on socket");
  }

  return true;
}

bool EchoApp::SendMessage(string message, struct sockaddr* peer_info) {
  // Avoid sending dups and set the sentinel if this is our heartbeat going out
  if (message == keyword_ && !received_)
    return false;
  if (message == keyword_)
    received_ = false;

  peer_info = (peer_info == NULL ? peer_info_ : peer_info);
  socklen_t peer_size = sizeof(*peer_info);

#ifdef UDP_APPLICATION
  sendto(app_socket_, message.c_str(), message.length() + 1, 0,
         peer_info, peer_size);
#elif TCP_APPLICATION
  return ShutDown("TCP is not yet supported in the DNS");
#endif

  // Report out to the user that we're sending
  Log(stderr, DEBUG, "Sending %s to our friend...", message.c_str());
  return true;
}

int EchoApp::CreateMobileNodeDelegate() {
  pthread_t mobile_node_daemon;
  mobile_node_ = new SimpleMobileNode(logical_address_, dns_server_,
                                      rendezvous_server_);

  int thread_status = pthread_create(&mobile_node_daemon, NULL,
                                     &RunMobileAgentThread, mobile_node_);
  return thread_status;
}
