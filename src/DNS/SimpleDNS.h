/**
 * @file
 * @author Thaddeus Diamond <diamond@cs.yale.edu>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * This is an interface for an simple version of DNS
 **/

#ifndef _PERMANENTIP_DNS_SIMPLEDNS_H_
#define _PERMANENTIP_DNS_SIMPLEDNS_H_

#include <gtest/gtest.h>
#include <tr1/unordered_map>
#include <fcntl.h>
#include <errno.h>

#include <cassert>
#include <cstdarg>

#include "Common/Utils.h"
#include "Common/Signal.h"
#include "DNS/DNS.h"

using Utils::Die;

using std::tr1::unordered_map;

class SimpleDNS : public DNS {
 public:
  /**
   * The constructor simply needs a port to listen for lookups
   **/
  explicit SimpleDNS() : port_(GLOB_LOOKUP_PORT),
    domain_(GLOB_DOM), transport_layer_(GLOB_TL), protocol_(GLOB_PROTO) {}

  /**
   * The SimpleDNS destructor does not have to free any memory as none was
   * aggregated
   **/
  virtual ~SimpleDNS() {}

  virtual bool Start();
  virtual bool ShutDown(const char* format, ...);

 protected:
  virtual bool AddName(LogicalAddress name, PhysicalAddress address);
  virtual PhysicalAddress LookupName(LogicalAddress name);

  /**
   * We specifically want to respond to connections given to us on the specified
   * port.  For this, we first need to listen.
   **/
  bool BeginListening();
  /**
   * After the socket has begun listening on that port, we handle requests.
   **/
  bool HandleRequests();

 private:
  /**
   * Privately, we keep a key-value store of logical addresses to physical
   * addresses that correspond to the rendezvous server that maintains that
   * logical address or the node itself.
   **/
  unordered_map<LogicalAddress, PhysicalAddress> registered_names_;

  /** We maintain which port we are listening for incoming lookups on **/
  unsigned short port_;
  int listener_;

  /** Keep connectivity member variables **/
  Domain domain_;
  TransportLayer transport_layer_;
  Protocol protocol_;

  // Declare friend tests for access to private methods
  friend class SimpleDNSTest;
  FRIEND_TEST(SimpleDNSTest, AddsAndLooksUp);
};

#endif  // _PERMANENTIP_DNS_SIMPLEDNS_H_
