// Author: Thaddeus Diamond (diamond@cs.yale.edu)
//
// This is a sample echo app that merely opens a connection to a peer and begins
// a two-way echo chamber (assuming the peer is also running the echo app)

#ifndef _PERMANENTIP_APPLICATIONS_ECHOAPP_H_
#define _PERMANENTIP_APPLICATIONS_ECHOAPP_H_

#include <pthread.h>
#include <cassert>
#include <cstdarg>

#include "Common/Signal.h"
#include "Common/Types.h"
#include "Common/Utils.h"
#include "MobileNode/SimpleMobileNode.h"
#include "Applications/Application.h"

class EchoApp : public Application {
 public:
  EchoApp(string keyword,
          LogicalAddress peer_address, unsigned short peer_port,
          PhysicalAddress dns_server, unsigned short dns_port, 
          unsigned short app_port) :
      keyword_(keyword), peer_address_(peer_address), peer_port_(peer_port),
      app_port_(app_port), dns_server_(dns_server), dns_port_(dns_port),
      domain_(GLOB_DOM), transport_layer_(GLOB_TL), protocol_(GLOB_PROTO) {}
  virtual ~EchoApp() {}

  // Application-like "run" paradigm
  virtual void Run();

  // Need to have a "shutdown" method that can be called with a SIGINT handler
  virtual void ShutDown(const char* format, ...);

 protected:
  // This is the method that creates an instance of our mobile node delegate on
  // a thread so that we can register sockets with it and follow the Mobile IP
  // protocol
  virtual int CreateMobileNodeDelegate();

  // Initially, we need to contact the mobile node agent and connect to the
  // peer.  This is a separate method for conciseness.  It returns false when
  // a connection cannot be made, otherwise it assigns the appropriate member
  // variables.
  bool ConnectToPeer();

//  // We delegate a special create socket method for this type of app.  Although
//  // generally desirable, it is not strictly required for an application and
//  // is therefore left not virtual.
//  int CreateSocket(int peer_address = 0, int peer_port = 0,
//                   bool listener = false);

  // Whenever someone sends us data we want to have an abstracted manner of
  // handling that so we don't clog up the application pool.  We do this for
  // sending out data as well.
  void PrintReceivedData();
  void EchoMessage(string message, int peer_address = 0, int peer_port = 0);

  // We keep track of the keyword we are going to send out to the server so that
  // we don't de-dup our traffic.  We make the assumption no two echo
  // applications have the same keyword.
  string keyword_;

  // We also want to keep track of what host and port number we are
  // communicating with.  We instantiate the socket normally, but later register
  // it with the mobile node agent, which manipulates it's behavior.
  LogicalAddress peer_address_;
  unsigned short peer_port_;

  // We need to listen in on a port for traffic from our peers.
  int app_socket_;
  unsigned short app_port_;

  // We need to know which DNS server we are talking with initially
  PhysicalAddress dns_server_;
  unsigned short dns_port_;

  // We specify how we communicate with other people (by default TCP).
  Domain domain_;
  TransportLayer transport_layer_;
  Protocol protocol_;

  // Each application owns an instance of the mobile node daemon.  This is for
  // practical purposes of implementing a single mobile node agent with
  // sharable memory (and trying to avoid interprocess communication).  Better
  // implementations would look at interprocess communication or multiple
  // applications on the same process with thread-safe mobile_node_ pointers.
  MobileNode* mobile_node_;
};

static inline void* RunMobileAgentThread(void* agent) {
  (reinterpret_cast<MobileNode*>(agent))->Run();
  return NULL;
}

#endif  // _PERMANENTIP_APPLICATIONS_ECHOAPP_H_
