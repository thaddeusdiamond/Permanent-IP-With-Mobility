/**
 * @file
 * @author Thaddeus Diamond <diamond@cs.yale.edu>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * This is a simple implementation of the mobile node daemon in our scheme
 **/

#include "MobileNode/SimpleMobileNode.h"

bool SimpleMobileNode::Start() {
  ConnectToServer(rendezvous_server_, rendezvous_port_, logical_address_);

  do {
    sleep(1);
    if (last_known_ip_address_ != GetCurrentIPAddress())
      UpdateRendezvousServer();

    PollSubscriptions();
    last_known_ip_address_ = GetCurrentIPAddress();
  } while (Signal::ShouldContinue());

  return true;
}

bool SimpleMobileNode::ShutDown(const char* format, ...) {
  fprintf(stderr, "Shutting Down Mobile Node (");

  va_list arguments;
  va_start(arguments, format);
  fprintf(stderr, format, arguments);
  perror(")");

  Signal::ExitProgram(0);

  fprintf(stderr, "OK\n");
  return false;
}

void SimpleMobileNode::UpdateRendezvousServer() {
  fprintf(stderr, "Location has changed, sending an update to the RS... ");
  ConnectToServer(rendezvous_server_, rendezvous_port_, logical_address_);
  fprintf(stderr, "OK\n");
}

void SimpleMobileNode::PollSubscriptions() {
  // Listen on server addr
  struct sockaddr_in server;
  memset(&server, 0, sizeof(server));

  // Make sure none of the subscriptions have changed
  unordered_map<int, struct sockaddr*>::iterator it;
  for (it = app_sockets_.begin(); it != app_sockets_.end(); it++) {
    struct sockaddr_in* peer =
      reinterpret_cast<struct sockaddr_in*>(it->second);
    socklen_t server_size = sizeof(*peer);

    char buffer[4096];
    memset(buffer, 0, sizeof(buffer));
#ifdef UDP_APPLICATION
    int bytes_read = recvfrom(it->first, buffer, sizeof(buffer), MSG_PEEK,
                              reinterpret_cast<struct sockaddr*>(&server),
                              &server_size);
#elif TCP_APPLICATION
    int bytes_read = -1;
    ShutDown("TCP is not yet supported");
#endif

    // Update the sockets with a sockopt and update the peer structs
    if (bytes_read > 0 && server.sin_addr.s_addr ==
        static_cast<unsigned int>(IPStringToInt(rendezvous_server_))) {
      recvfrom(it->first, buffer, sizeof(buffer), 0,
               reinterpret_cast<struct sockaddr*>(&server), &server_size);
      fprintf(stderr, "Updating socket #%d's struct sockaddr from %d to %s\n",
              it->first, peer->sin_addr.s_addr, buffer);
      peer->sin_addr.s_addr = IPStringToInt(string(buffer));

    // Error on the socket
    } else if (bytes_read < 0 && errno != EWOULDBLOCK && errno != EAGAIN) {
      ShutDown("Error listening on socket");
    }
  }
}

struct sockaddr* SimpleMobileNode::RegisterPeer(int app_socket,
                                                LogicalAddress peer_addr) {
  // Establish connection to DNS and receive RS to contact
  PhysicalAddress rs_addr = ConnectToServer(dns_server_, GLOB_LOOKUP_PORT,
                                            peer_addr);
  if (rs_addr == "")
    return NULL;
  PhysicalAddress peer_loc = ConnectToServer(rs_addr, GLOB_LOOKUP_PORT,
                                             peer_addr, app_socket);
  if (peer_loc == "")
    return NULL;

  // Construct a container for the peer's real location
  struct sockaddr_in* peer_in = new sockaddr_in();
  memset(peer_in, 0, sizeof(*peer_in));
  peer_in->sin_addr.s_addr = IPStringToInt(peer_loc);

  // Register the peer in our list of app sockets and return container
  app_sockets_[app_socket] = reinterpret_cast<struct sockaddr*>(peer_in);
  return reinterpret_cast<struct sockaddr*>(peer_in);
}

NetworkMsg SimpleMobileNode::ConnectToServer(PhysicalAddress server_addr,
                                             unsigned short server_port,
                                             NetworkMsg information,
                                             int sender) {
  bool close_sender = (sender < 0 ? true : false);
  sender = (sender < 0 ? socket(domain_, transport_layer_, protocol_) : sender);

  // We need to ensure that the socket is blocking if it previously wasn't
  int opts;
  if ((opts = fcntl(sender, F_GETFL)) < 0)
    return "";
  if (fcntl(sender, F_SETFL, opts & ~O_NONBLOCK) < 0)
    return "";

  // Actually connect to the server
  struct sockaddr_in server;
  memset(&server, 0, sizeof(server));
  server.sin_family = domain_;
  server.sin_addr.s_addr = IPStringToInt(server_addr);
  server.sin_port = htons(server_port);
  socklen_t server_size = sizeof(server);

  int on;
  setsockopt(sender, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char*>(&on),
             sizeof(on));

  char buffer[4096];
  memset(buffer, 0, sizeof(buffer));

#ifdef UDP_APPLICATION
  sendto(sender, information.c_str(), information.length(), 0,
         reinterpret_cast<struct sockaddr*>(&server),
         server_size);
  recvfrom(sender, buffer, sizeof(buffer), 0,
           reinterpret_cast<struct sockaddr*>(&server),
           &server_size);
#elif TCP_APPLICATION
  server_size = 0;
  fprintf(stderr, "TCP is not yet supported\n");
#endif

  // Fix and close the socket we mutzed
  if (fcntl(sender, F_SETFL, opts) < 0)
    return "";
  if (close_sender)
    close(sender);

  return NetworkMsg(buffer);
}
