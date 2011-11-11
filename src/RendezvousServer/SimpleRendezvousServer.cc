// Author: Thaddeus Diamond (diamond@cs.yale.edu)
//
// This is an implementation for a simple version of DNS

#include "RendezvousServer/SimpleRendezvousServer.h"

bool SimpleRendezvousServer::Start() {
  registration_listener_ = BeginListening(registration_port_);
  lookup_listener_ = BeginListening(lookup_port_);

  Signal::RestartProgram();
  Signal::HandleSignalInterrupts();
  do {
    assert(HandleRequests(lookup_listener_, true));
    assert(HandleRequests(registration_listener_, false));
    usleep(500);
  } while (Signal::ShouldContinue());
  return true;
}

bool SimpleRendezvousServer::ShutDown(const char* format, ...) {
  fprintf(stderr, "Shutting Down Rendezvous Server (");

  va_list arguments;
  va_start(arguments, format);
  fprintf(stderr, format, arguments);
  perror(")");

  close(registration_listener_);
  close(lookup_listener_);
  Signal::ExitProgram(0);

  fprintf(stderr, "OK\n");
  return false;
}

int SimpleRendezvousServer::BeginListening(unsigned short port) {
  int listener = socket(domain_, transport_layer_, protocol_);
  if (listener < 0)
    return ShutDown("Could not begin listening on RS");

  struct sockaddr_in server;
  memset(&server, 0, sizeof(server));
  server.sin_family = domain_;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(port);

  if (bind(listener, reinterpret_cast<struct sockaddr*>(&server),
           sizeof(server)))
    return ShutDown("Could not bind listening connection");

#ifdef TCP_APPLICATION
  if (listen(listener, MAX_CONNECTIONS))
    return ShutDown("Could not listen on RS port");
#endif

  int on;
  if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR,
                 reinterpret_cast<char*>(&on), sizeof(on)) < 0)
    return ShutDown("Could not make the socket reusable");

  int opts;
  if ((opts = fcntl(listener, F_GETFL)) < 0)
    return ShutDown("Error getting the socket options");
  if (fcntl(listener, F_SETFL, opts | O_NONBLOCK) < 0)
    return ShutDown("Error setting the socket to nonblocking");

  fprintf(stderr, "Now listening for requests on port %d\n", port);

  return listener;
}

bool SimpleRendezvousServer::HandleRequests(int listening_socket, bool lookup) {
  struct sockaddr_in request_src;
  socklen_t request_src_size = sizeof(request_src);

  char buffer[4096];
  memset(buffer, 0, sizeof(buffer));

#ifdef UDP_APPLICATION
  int bytes_read = recvfrom(listening_socket, buffer, sizeof(buffer), 0,
                            reinterpret_cast<struct sockaddr*>(&request_src),
                            &request_src_size);
#elif TCP_APPLICATION
  int bytes_read = -1;
  ShutDown("TCP is not yet supported in the RS");
#endif

  int source_address = request_src.sin_addr.s_addr;

  // Handle address lookup
  if (bytes_read > 0 && lookup) {
    const char* peer = ChangeSubscription(
      pair<LogicalAddress, unsigned short>(IntToIPName(source_address),
                                           request_src.sin_port),
      buffer).c_str();

    fprintf(stderr, "Sending RS lookup of <%s, %s> to (%d:%d)\n", buffer,
            peer, source_address, ntohs(request_src.sin_port));
    snprintf(buffer, sizeof(buffer), "%s", peer);

  // Handle address updating
  } else if (bytes_read > 0) {
    fprintf(stderr, "Updating RS registration of <%s> from (%d:%d)\n", buffer,
            source_address, ntohs(request_src.sin_port));
    UpdateAddress(buffer, IntToIPString(source_address));

    char switcher[4096];
    strncpy(switcher, buffer, sizeof(buffer));
    snprintf(buffer, sizeof(buffer), "%s %d", switcher, source_address);

  // Error on the socket
  } else if (bytes_read < 0 && errno != EWOULDBLOCK && errno != EAGAIN) {
    return ShutDown("Error listening on socket");
  }

  if (bytes_read > 0) {
#ifdef UDP_APPLICATION
    sendto(listening_socket, buffer, sizeof(buffer), 0,
           reinterpret_cast<struct sockaddr*>(&request_src), request_src_size);
#elif TCP_APPLICATION
    request_src_size = -1;
#endif
  }

  return true;
}

bool SimpleRendezvousServer::UpdateAddress(LogicalAddress name,
                                           PhysicalAddress address) {
  registered_names_[name] = address;

  int update_socket = socket(domain_, transport_layer_, protocol_);
  set< pair<LogicalAddress, unsigned short> >::iterator i;

  for (i = subscriptions_[name].begin(); i != subscriptions_[name].end(); i++) {
    // TODO(Thad): Improve this, we just use a gethostbyname lookup for now
    struct hostent* subscriber_host = gethostbyname(i->first.c_str());

    struct sockaddr_in subscriber;
    subscriber.sin_family = domain_;
    subscriber.sin_addr.s_addr =
      reinterpret_cast<struct in_addr*>(subscriber_host->h_addr)->s_addr;
    subscriber.sin_port = i->second;
    socklen_t subscriber_size = sizeof(subscriber);

    // Send out the actual update
    fprintf(stderr, "Sending update of %s<%s> to %s\n", name.c_str(),
            address.c_str(), i->first.c_str());
#ifdef UDP_APPLICATION
    sendto(update_socket, address.c_str(), address.length() + 1, 0,
           reinterpret_cast<struct sockaddr*>(&subscriber), subscriber_size);
#elif TCP_APPLICATION
    return false;
#endif
  }

  return true;
}

PhysicalAddress SimpleRendezvousServer::ChangeSubscription(
    pair<LogicalAddress, unsigned short> subscriber,
    LogicalAddress client) {
  if (registered_names_.count(client) > 0) {
    if (subscriptions_[client].find(subscriber) == subscriptions_[client].end())
      subscriptions_[client].insert(subscriber);
    else
      subscriptions_[client].erase(subscriber);
  }

  return (registered_names_.count(client) > 0 ? registered_names_[client] : "");
}
