// Author: Thaddeus Diamond (diamond@cs.yale.edu)
//
// This is an implementation for a simple version of DNS

#include "DNS/SimpleDNS.h"

bool SimpleDNS::Start() {
  assert(AddName("python.cs.yale.edu", "128.36.232.46"));
  assert(BeginListening());

  Signal::HandleSignalInterrupts();
  do {
    assert(HandleRequests());
  } while (Signal::ShouldContinue());
  return true;
}

bool SimpleDNS::ShutDown(const char* format, ...) {
  if (format != NULL) {
    fprintf(stdout, "Shutting Down DNS Server... ");

    va_list arguments;
    va_start(arguments, format);

    fprintf(stderr, format, arguments);
    perror(" ");
  }

  close(listener_);
  Signal::ExitProgram(0);

  if (format != NULL)
    fprintf(stdout, "OK\n");

  return true;
}

bool SimpleDNS::BeginListening() {
  listener_ = socket(domain_, transport_layer_, protocol_);
  if (listener_ < 0)
    ShutDown("Could not begin listening on DNS");

  struct sockaddr_in server;
  memset(&server, 0, sizeof(server));
  server.sin_family = domain_;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(port_);

  if (bind(listener_, reinterpret_cast<struct sockaddr*>(&server),
           sizeof(server)))
    ShutDown("Could not bind listening connection");

  if (transport_layer_ == TCP && listen(listener_, MAX_CONNECTIONS))
    ShutDown("Could not listen on DNS port");

  int on;
  if (setsockopt(listener_, SOL_SOCKET, SO_REUSEADDR,
                 reinterpret_cast<char*>(&on), sizeof(on)) < 0)
    ShutDown("Could not make the socket reusable");

  return true;
}

bool SimpleDNS::HandleRequests() {
  return true;
}

bool SimpleDNS::AddName(LogicalAddress name, PhysicalAddress address) {
  registered_names_[name] = address;
  return true;
}

PhysicalAddress SimpleDNS::LookupName(LogicalAddress name) {
  return (registered_names_.count(name) > 0 ? registered_names_[name] : "");
}
