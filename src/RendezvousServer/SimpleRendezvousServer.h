// Author: Thaddeus Diamond (diamond@cs.yale.edu)
//
// This is an interface for a simple version of DNS

#ifndef _PERMANENTIP_RENDEZVOUSSERVER_SIMPLERENDEZVOUSSERVER_H_
#define _PERMANENTIP_RENDEZVOUSSERVER_SIMPLERENDEZVOUSSERVER_H_

#include <gtest/gtest.h>
#include <tr1/unordered_map>
#include <fcntl.h>
#include <errno.h>

#include <cassert>
#include <cstdarg>
#include <set>

#include "Common/Utils.h"
#include "Common/Signal.h"
#include "RendezvousServer/RendezvousServer.h"

using std::tr1::unordered_map;
using std::set;

class SimpleRendezvousServer : public RendezvousServer {
 public:
  // The constructor simply needs a port to listen for lookups
  explicit SimpleRendezvousServer(unsigned short registration_port,
                                  unsigned short lookup_port) :
    registration_port_(registration_port), lookup_port_(lookup_port),
    domain_(GLOB_DOM), transport_layer_(GLOB_TL), protocol_(GLOB_PROTO) {}

  // Null destructor
  virtual ~SimpleRendezvousServer() {}

  // Server-like start and stop
  virtual bool Start();
  virtual bool ShutDown(const char* format, ...);

 protected:
  // We need to register and update addresses in the database
  virtual bool UpdateAddress(LogicalAddress name, PhysicalAddress address);

  // Adding/removing subscribers is key to the operation of a RS
  virtual PhysicalAddress ChangeSubscription(LogicalAddress subscriber,
                                             LogicalAddress client);

  // We specifically want to respond to connections given to us on the specified
  // port.  For a simple RS, this merely involves listening in and then
  // accepting and answering requests.
  int BeginListening(unsigned short port);
  bool HandleRequests(int listener_socket, bool lookup);

 private:
  // Privately, we keep a key-value store of logical addresses to physical
  // addresses that correspond to the physical address of each name, and also
  // which clients are subscribed to receive updates
  unordered_map<LogicalAddress, PhysicalAddress> registered_names_;
  unordered_map<LogicalAddress, set<LogicalAddress> > subscriptions_;

  // We maintain which port we are listening for incoming registrations on
  unsigned short registration_port_;
  int registration_listener_;

  // We maintain which port we are listening for incoming lookups on
  unsigned short lookup_port_;
  int lookup_listener_;

  // Keep connectivity member variables
  Domain domain_;
  TransportLayer transport_layer_;
  Protocol protocol_;

  // Declare friend tests for access to private methods
  friend class SimpleRendezvousServerTest;
  FRIEND_TEST(SimpleRendezvousServerTest, UpdatesAndHandlesSubscribers);
  FRIEND_TEST(SimpleRendezvousServerTest, HandlesNetworkRequests);
};

#endif  // _PERMANENTIP_RENDEZVOUSSERVER_SIMPLERENDEZVOUSSERVER_H_
