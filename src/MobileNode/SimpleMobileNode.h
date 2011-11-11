// Author: Thaddeus Diamond (diamond@cs.yale.edu)
//
// This is an abstract interface for a simple implementation of the mobile node
// agent in our rendezvous server scheme

#ifndef _PERMANENTIP_MOBILENODE_SIMPLEMOBILENODE_H_
#define _PERMANENTIP_MOBILENODE_SIMPLEMOBILENODE_H_

#include <netdb.h>
#include <fcntl.h>
#include <tr1/unordered_map>
#include <cerrno>
#include <cassert>
#include <cstdarg>
#include <string>

#include "Common/Signal.h"
#include "Common/Types.h"
#include "Common/Utils.h"
#include "MobileNode/MobileNode.h"

using std::tr1::unordered_map;
using std::string;

class SimpleMobileNode : public MobileNode {
 public:
  SimpleMobileNode(LogicalAddress logical_address, PhysicalAddress dns_server,
                   PhysicalAddress rendezvous_server) :
    logical_address_(logical_address),
    last_known_ip_address_(GetCurrentIPAddress()), dns_server_(dns_server),
    rendezvous_server_(rendezvous_server), rendezvous_port_(GLOB_REGIST_PORT),
    domain_(GLOB_DOM), transport_layer_(GLOB_TL), protocol_(GLOB_PROTO) {}
  virtual ~SimpleMobileNode() {}

  // We use a daemon-like "run" and "shutdown" paradigm
  virtual bool Start();
  virtual bool ShutDown(const char* format, ...);

  // Any application needs to register an open socket so that it can be
  // intercepted
  virtual struct sockaddr* RegisterPeer(int app_socket,
                                        LogicalAddress peer_addr);

 protected:
  // A mobile agent needs to connect to arbitrary servers to gain information
  NetworkMsg ConnectToServer(PhysicalAddress server_addr,
                             unsigned short server_port,
                             NetworkMsg information, int sender = -1);

  // A mobile node needs to update the home agent when it's IP changes and
  // poll existing subscriptions
  virtual void UpdateRendezvousServer();
  virtual void PollSubscriptions();

  // We keep the last known identity and change the interface if that's
  // no longer our IP
  LogicalAddress logical_address_;
  int last_known_ip_address_;

  // Listed connection to home RS and global DNS server
  PhysicalAddress dns_server_;
  PhysicalAddress rendezvous_server_;
  unsigned short rendezvous_port_;

  // We must specify what type of transport layer protocol we are using
  Domain domain_;
  TransportLayer transport_layer_;
  Protocol protocol_;

  // We keep track of the applications with open sockets
  unordered_map<int, struct sockaddr*> app_sockets_;
};

#endif  // _PERMANENTIP_MOBILENODE_SIMPLEMOBILENODE_H_
