/**
 * @file
 * @author Thaddeus Diamond <diamond@cs.yale.edu>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * This is a sample echo app that merely opens a connection to a peer and begins
 * a two-way echo chamber (assuming the peer is also running the echo app)
 **/

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

using Utils::Die;
using Utils::Log;

class EchoApp : public Application {
 public:
  /**
   * The constructor for an application is responsible for initializing
   * the member variables of the application class (enumerated in the class
   * definition)
   **/
  EchoApp(string keyword,
          LogicalAddress logical_address, unsigned short app_port,
          LogicalAddress peer_addr, unsigned short peer_port,
          PhysicalAddress dns_server, PhysicalAddress rendezvous_server) :
      keyword_(keyword), received_(true), logical_address_(logical_address),
      peer_addr_(peer_addr), peer_port_(peer_port), app_port_(app_port),
      dns_server_(dns_server), rendezvous_server_(rendezvous_server),
      domain_(GLOB_DOM), transport_layer_(GLOB_TL), protocol_(GLOB_PROTO) {}

  /**
   * The destructor is responsible for freeing the memory associated with
   * the application's mobile node delegate
   **/
  virtual ~EchoApp() { delete mobile_node_; }

  virtual bool Start();
  virtual bool ShutDown(const char* format, ...);

 protected:
  virtual int CreateMobileNodeDelegate();

  /**
   * Initially, we need to contact the mobile node agent and connect to the
   * peer.  This is a separate method for conciseness.  It returns false when
   * a connection cannot be made, otherwise it assigns the appropriate member
   * variables.
   *
   * @returns   True if the node can connect to the peer in less than
   *            @ref MAX_ATTEMPTS
   **/
  bool ConnectToPeer();

  /**
   * Whenever someone sends us data we want to have an abstracted manner of
   * handling that so we don't clog up the application pool.
   *
   * @returns   True unless there was an error with the socket read from for
   *            the peer
   **/
  bool PrintReceivedData();

  /**
   * We create an abstracted way to send some message out to an arbitrary
   * peer.
   *
   * @param     message     The message to be sent
   * @param     peer_info   The peer socket we are sending the message to
   *                        (Default: NULL)
   *
   * @returns   True if the message was successfully sent
   **/
  bool SendMessage(string message, struct sockaddr* peer_info = NULL);

  /**
   * We keep track of the keyword we are going to send out to the server so that
   * we don't dup our traffic.  We make the assumption no two echo
   * applications have the same keyword.
   **/
  string keyword_;

  /**
   * We want to make sure we don't overload so we're going to wait until we
   * get our message back before retransmitting.
   **/
  bool received_;

  /**
   * Keep track of the current node's logical address
   **/
  LogicalAddress logical_address_;

  /**
   * We also want to keep track of what host address...
   **/
  LogicalAddress peer_addr_;

  /**
   * ..port number...
   **/
  unsigned short peer_port_;

  /**
   * ...and the actual socket structure we are communicating with. We instantiate
   * the socket normally, but later register it with the mobile node agent,
   * which manipulates it's behavior.
   **/
  struct sockaddr* peer_info_;

  /**
   * We need to listen in on a specific socket...
   **/
  int app_socket_;

  /**
   * ...and port for traffic from our peers.
   **/
  unsigned short app_port_;

  /**
   * We need to know which DNS...
   **/
  PhysicalAddress dns_server_;

  /**
   * ...and RS server we are talking with initially
   **/
  PhysicalAddress rendezvous_server_;

  /**
   * We specify how we communicate with other people via domain
   * (Default: @ref GLOB_DOM)...
   **/
  Domain domain_;

  /**
   * ...transport layer (Default: @ref GLOB_TL)...
   **/
  TransportLayer transport_layer_;

  /**
   * ...and protocol (Default: @ref GLOB_PROTO)...
   **/
  Protocol protocol_;

  /**
   * Each application owns an instance of the mobile node daemon.  This is for
   * practical purposes of implementing a single mobile node agent with
   * sharable memory (and trying to avoid interprocess communication).  Better
   * implementations would look at interprocess communication or multiple
   * applications on the same process with thread-safe mobile_node_ pointers.
   **/
  MobileNode* mobile_node_;
};

/** Separate non-class method required by pthread **/
static inline void* RunMobileAgentThread(void* agent) {
  (reinterpret_cast<MobileNode*>(agent))->Start();
  return NULL;
}

#endif  // _PERMANENTIP_APPLICATIONS_ECHOAPP_H_
