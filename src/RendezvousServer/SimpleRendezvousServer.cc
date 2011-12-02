/**
 * @file
 * @author Thaddeus Diamond <diamond@cs.yale.edu>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * This is an implementation of a simple version of the RS in our scheme
 **/

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
  Log(stderr, DEBUG, "Shutting Down Rendezvous Server (");

  va_list arguments;
  va_start(arguments, format);
  Log(stderr, WARNING, format, arguments);
  perror(")");

  close(registration_listener_);
  close(lookup_listener_);
  Signal::ExitProgram(0);

  Log(stderr, SUCCESS, "OK");
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

  Log(stderr, SUCCESS, "Now listening for requests on port %d", port);

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
    // The string sent is of the form subscriber|subscribee so we need to
    // parse it out in order to update the subscription
    NetworkMsg request = NetworkMsg(buffer);
    LogicalAddress subscriber = request.substr(0, request.find("|"));
    LogicalAddress subscribee = request.substr(request.find("|") + 1);

    const char* peer = ChangeSubscription(
      pair<LogicalAddress, unsigned short>(subscriber, request_src.sin_port),
      subscribee).c_str();

    Log(stderr, SUCCESS, "Sending RS lookup of <%s, %s> to (%d:%d)",
        subscriber.c_str(), peer, source_address, ntohs(request_src.sin_port));
    snprintf(buffer, sizeof(buffer), "%s", peer);

  // Handle address updating
  } else if (bytes_read > 0) {
    Log(stderr, WARNING, "Updating RS registration of <%s> from (%d:%d)",
        buffer, source_address, ntohs(request_src.sin_port));
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
    /**
     * @todo  For now we assume all communications deal with the same RS. In the
     *        future we need to do a full round robin to the DNS
     **/
    struct sockaddr_in subscriber;
    subscriber.sin_family = domain_;
    subscriber.sin_addr.s_addr =
      IPStringToInt(registered_names_[i->first]);
    subscriber.sin_port = i->second;
    socklen_t subscriber_size = sizeof(subscriber);

    // Send out the actual update
    Log(stderr, WARNING, "Sending update of %s<%s> to %s(%s:%d)", name.c_str(),
            address.c_str(), i->first.c_str(),
            registered_names_[i->first].c_str(), ntohs(i->second));
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
