// Author: Thaddeus Diamond (diamond@cs.yale.edu)
//
// This is an abstract interface for an arbitrary implementation of DNS

#ifndef _PERMANENTIP_DNS_DNS_H_
#define _PERMANENTIP_DNS_DNS_H_

#include "Common/Types.h"

class DNS {
 public:
  virtual ~DNS() {}

  // We follow a server-like mechanism for start and shutdown (that also accepts
  // SIGINT for shutting down).  The shutdown method incidentally acts merely
  // as a SIGINT handler and cleanup.
  virtual bool Start() = 0;
  virtual bool ShutDown(const char* format, ...) = 0;

 protected:
  // Every DNS should have the ability to privately add a namespace to its
  // hierarchy
  virtual bool AddName(LogicalAddress name, PhysicalAddress address) = 0;

  // Given a specific logical address, the DNS should be able to lookup and
  // return its physical address.
  virtual PhysicalAddress LookupName(LogicalAddress name) = 0;
};

#endif  // _PERMANENTIP_DNS_DNS_H_
