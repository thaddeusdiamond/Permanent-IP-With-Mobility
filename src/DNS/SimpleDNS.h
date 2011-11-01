// Author: Thaddeus Diamond (diamond@cs.yale.edu)
//
// This is an interface for a simple version of DNS

#ifndef _PERMANENTIP_DNS_SIMPLEDNS_H_
#define _PERMANENTIP_DNS_SIMPLEDNS_H_

#include <gtest/gtest.h>
#include <tr1/unordered_map>
#include <cassert>
#include "Common/Signal.h"
#include "DNS/DNS.h"

using std::tr1::unordered_map;

class SimpleDNS : public DNS {
 public:
  // The constructor simply needs a port to listen for lookups
  explicit SimpleDNS(unsigned short port) : port_(port) {}

  // Null destructor
  virtual ~SimpleDNS() {}

  // Server-like start and stop
  virtual bool Start();
  virtual bool ShutDown();

  // Declare friend tests for access to private methods
  friend class SimpleDNSTest;
  FRIEND_TEST(SimpleDNSTest, AddsAndLooksUp);

 protected:
  // Adding names in a simple DNS is easy, we merely update the map
  virtual bool AddName(LogicalAddress name, PhysicalAddress address);

  // The lookup name method in this is simple, using an unordered map for speed
  virtual PhysicalAddress LookupName(LogicalAddress name);

  // We specifically want to respond to connections given to us on the specified
  // port.  For a simple DNS, this merely involves listening in and then
  // accepting and answering requests.
  bool BeginListening();
  bool HandleRequests();

 private:
  // Privately, we keep a key-value store of logical addresses to physical
  // addresses that correspond to the rendezvous server that maintains that
  // logical address or the node itself.
  unordered_map<LogicalAddress, PhysicalAddress> registered_names_;

  // We maintain which port we are listening for incoming lookups on
  unsigned short port_;
};

#endif  // _PERMANENTIP_DNS_SIMPLEDNS_H_
