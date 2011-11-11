// Author: Thaddeus Diamond (diamond@cs.yale.edu)
//
// This is an abstract interface for an arbitrary implementation of an RS

#ifndef _PERMANENTIP_RENDEZVOUSSERVER_RENDEZVOUSSERVER_H_
#define _PERMANENTIP_RENDEZVOUSSERVER_RENDEZVOUSSERVER_H_

#include <utility>
#include "Common/Types.h"

using std::pair;

class RendezvousServer {
 public:
  virtual ~RendezvousServer() {}

  // We follow a server-like mechanism for start and shutdown (that also accepts
  // SIGINT for shutting down).  The shutdown method incidentally acts merely
  // as a SIGINT handler and cleanup.
  virtual bool Start() = 0;
  virtual bool ShutDown(const char* format, ...) = 0;

 protected:
  // Every Rendezvous Server has the ability to register a new address and,
  // when that node is mobile, update that address
  virtual bool UpdateAddress(LogicalAddress name, PhysicalAddress address) = 0;

  // When a peer is trying to connect to the user, it must request to be added
  // to the subscriber list, and there must be some mechanism to unsubscribe the
  // user from that list at some point.  When the subscriber is added to the
  // list the user's last known physical address is returned for the connecting
  // user to make a connection to.
  virtual PhysicalAddress ChangeSubscription(
      pair<LogicalAddress, unsigned short>,
      LogicalAddress client) = 0;
};

#endif  // _PERMANENTIP_RENDEZVOUSSERVER_RENDEZVOUSSERVER_H_
