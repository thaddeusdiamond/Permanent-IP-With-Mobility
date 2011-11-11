// Author: Thaddeus Diamond (diamond@cs.yale.edu)
//
// This is a sample echo app that merely opens a connection to a peer and begins
// a two-way echo chamber (assuming the peer is also running the echo app)

#ifndef _PERMANENTIP_APPLICATIONS_ECHOAPP_H_
#define _PERMANENTIP_APPLICATIONS_ECHOAPP_H_

#include <fcntl.h>
#include <pthread.h>
#include <errno.h>

#include <cassert>
#include <cstdarg>
#include <string>

#include "Common/Signal.h"
#include "Common/Types.h"
#include "Common/Utils.h"
#include "MobileNode/SimpleMobileNode.h"
#include "Applications/Application.h"

class EchoApp : public Application {
 public:
  EchoApp(string keyword,
          LogicalAddress logical_address, unsigned short app_port,
          LogicalAddress peer_addr, unsigned short peer_port,
          PhysicalAddress dns_server, PhysicalAddress rendezvous_server) :
      keyword_(keyword), received_(true), logical_address_(logical_address),
      peer_addr_(peer_addr), peer_port_(peer_port), app_port_(app_port),
      dns_server_(dns_server), rendezvous_server_(rendezvous_server),
      domain_(GLOB_DOM), transport_layer_(GLOB_TL), protocol_(GLOB_PROTO) {}
  virtual ~EchoApp() {
    delete mobile_node_;
  }

  // Application-like "run" and "shutdown" paradigm
  virtual bool Start();
  virtual bool ShutDown(const char* format, ...);

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

  // Whenever someone sends us data we want to have an abstracted manner of
  // handling that so we don't clog up the application pool.  We do this for
  // sending out data as well.
  bool PrintReceivedData();
  bool EchoMessage(string message, struct sockaddr* peer_info = NULL);

  // We keep track of the keyword we are going to send out to the server so that
  // we don't de-dup our traffic.  We make the assumption no two echo
  // applications have the same keyword.  Also, we want to make sure we don't
  // overload so we're going to wait until we get our message back before
  // retransmitting.
  string keyword_;
  bool received_;

  // Current node's logical address
  LogicalAddress logical_address_;

  // We also want to keep track of what host and port number we are
  // communicating with.  We instantiate the socket normally, but later register
  // it with the mobile node agent, which manipulates it's behavior.
  LogicalAddress peer_addr_;
  unsigned short peer_port_;
  struct sockaddr* peer_info_;

  // We need to listen in on a port for traffic from our peers.
  int app_socket_;
  unsigned short app_port_;

  // We need to know which DNS and RS server we are talking with initially
  PhysicalAddress dns_server_;
  PhysicalAddress rendezvous_server_;

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
  (reinterpret_cast<MobileNode*>(agent))->Start();
  return NULL;
}

#endif  // _PERMANENTIP_APPLICATIONS_ECHOAPP_H_
