// Author: Thaddeus Diamond (diamond@cs.yale.edu)
//
// This is an implementation for a simple version of DNS

#include "DNS/SimpleDNS.h"

bool SimpleDNS::Start() {
  assert(AddName("python", "128.36.232.37"));  // RS on Cobra
  assert(AddName("tick", "128.36.232.37"));    // RS on Cobra
  assert(BeginListening());

  Signal::RestartProgram();
  Signal::HandleSignalInterrupts();
  do {
    assert(HandleRequests());
    usleep(500);
  } while (Signal::ShouldContinue());
  return true;
}

bool SimpleDNS::ShutDown(const char* format, ...) {
  fprintf(stderr, "Shutting Down DNS Server (");

  va_list arguments;
  va_start(arguments, format);
  fprintf(stderr, format, arguments);
  perror(")");

  close(listener_);
  Signal::ExitProgram(0);

  fprintf(stderr, "OK\n");
  return false;
}

bool SimpleDNS::BeginListening() {
  listener_ = socket(domain_, transport_layer_, protocol_);
  if (listener_ < 0)
    return ShutDown("Could not begin listening on DNS");

  struct sockaddr_in server;
  memset(&server, 0, sizeof(server));
  server.sin_family = domain_;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(port_);

  if (bind(listener_, reinterpret_cast<struct sockaddr*>(&server),
           sizeof(server)))
    return ShutDown("Could not bind listening connection");

#ifdef TCP_APPLICATION
  if (listen(listener_, MAX_CONNECTIONS))
    return ShutDown("Could not listen on DNS port");
#endif

  int on;
  if (setsockopt(listener_, SOL_SOCKET, SO_REUSEADDR,
                 reinterpret_cast<char*>(&on), sizeof(on)) < 0)
    return ShutDown("Could not make the socket reusable");

  int opts;
  if ((opts = fcntl(listener_, F_GETFL)) < 0)
    return ShutDown("Error getting the socket options");
  if (fcntl(listener_, F_SETFL, opts | O_NONBLOCK) < 0)
    return ShutDown("Error setting the socket to nonblocking");

  fprintf(stderr, "Now listening for requests on port %d\n", port_);

  return true;
}

bool SimpleDNS::HandleRequests() {
  struct sockaddr_in request_src;
  socklen_t request_src_size = sizeof(request_src);

  char buffer[4096];
  memset(buffer, 0, sizeof(buffer));

#ifdef UDP_APPLICATION
  int bytes_read = recvfrom(listener_, buffer, sizeof(buffer), 0,
                            reinterpret_cast<struct sockaddr*>(&request_src),
                            &request_src_size);
#elif TCP_APPLICATION
  int bytes_read = -1;
  ShutDown("TCP is not yet supported in the DNS");
#endif

  if (bytes_read > 0) {
    fprintf(stderr, "Sending DNS lookup of <%s, %s> to (%d:%d)\n", buffer,
            LookupName(buffer).c_str(), request_src.sin_addr.s_addr,
            ntohs(request_src.sin_port));
    snprintf(buffer, sizeof(buffer), "%s", LookupName(buffer).c_str());

#ifdef UDP_APPLICATION
    sendto(listener_, buffer, sizeof(buffer), 0,
           reinterpret_cast<struct sockaddr*>(&request_src),
           request_src_size);
#elif TCP_APPLICATION
    request_src_size = -1;
#endif
  } else if (bytes_read < 0 && errno != EWOULDBLOCK && errno != EAGAIN) {
    return ShutDown("Error listening on socket");
  }

  return true;
}

bool SimpleDNS::AddName(LogicalAddress name, PhysicalAddress address) {
  registered_names_[name] = address;
  return true;
}

PhysicalAddress SimpleDNS::LookupName(LogicalAddress name) {
  return (registered_names_.count(name) > 0 ? registered_names_[name] : "");
}
