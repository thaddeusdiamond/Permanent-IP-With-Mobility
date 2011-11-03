// Author: Thaddeus Diamond (diamond@cs.yale.edu)
//
// This is an abstract interface for a simple implementation of the mobile node
// agent in our rendezvous server scheme

#ifndef _PERMANENTIP_MOBILENODE_SIMPLEMOBILENODE_H_
#define _PERMANENTIP_MOBILENODE_SIMPLEMOBILENODE_H_

//#include <fcntl.h>
//#include <net/if.h>
//#include <linux/if_tun.h>
//#include <sys/ioctl.h>
//#include <netdb.h>
//#include <unistd.h>
//#include <cerrno>
//#include <cassert>
//#include <map>
//#include <set>
#include <cstdarg>

#include "Common/Signal.h"
#include "Common/Types.h"
#include "Common/Utils.h"
#include "MobileNode/MobileNode.h"

//using std::map;
//using std::set;

class SimpleMobileNode : public MobileNode {
 public:
  explicit SimpleMobileNode(PhysicalAddress dns_server,
                            unsigned short dns_port) :
    dns_server_(dns_server), dns_port_(dns_port), domain_(GLOB_DOM),
    transport_layer_(GLOB_TL), protocol_(GLOB_PROTO) {}
  virtual ~SimpleMobileNode() {}

  // We use a daemon-like "run" paradigm
  virtual void Start();

  // We also need a "shutdown" mechanism that we can access with a SIGINT
  virtual bool ShutDown(const char* format, ...);

  // Any application needs to register an open socket so that it can be
  // intercepted
  virtual struct sockaddr* RegisterPeer(int app_socket,
                                        LogicalAddress peer_addr,
                                        unsigned short peer_port);

// protected:
//  // A mobile agent needs to instantiate a connection to the home agent
//  virtual void ConnectToHome(unsigned short port, char* data = NULL,
//                             bool initial = false);

//  // The primary thing a mobile node does is intercept outgoing traffic on
//  // the virtual tunnels and forwards along all of the data to the home agent's
//  // data port.
//  virtual void CollectOutgoingTraffic();

//  // A mobile node needs to update the home agent when it's IP changes
//  virtual void ChangeHomeIdentity();

//  // A Mobile node should create tunnels, but it's not strictly a requirement
//  int CreateTunnel(char* tunnel_name, int tunnel_address);

  // Listed connection to home DNS server
  PhysicalAddress dns_server_;
  unsigned short dns_port_;

//  // We maintain what mailbox the home agent is keeping open for us
//  unsigned int permanent_address_;
//  unsigned short permanent_port_;

//  // The mobile node needs to know what port to accept incoming traffic on
//  // when the home agent receives something and forwards it to us.
//  unsigned short listener_port_;

//  // We keep the last known identity and change the interface if that's
//  // no longer our IP
//  int last_known_ip_address_;

  // We must specify what type of transport layer protocol we are using
  Domain domain_;
  TransportLayer transport_layer_;
  Protocol protocol_;

//  // We keep track of the virtual tunnel applications are using
//  char* tunnel_name_;
//  int tunnel_fd_;
//  int permanent_address;
//  set<int> peer_fds_;
};

#endif  // _PERMANENTIP_MOBILENODE_SIMPLEMOBILENODE_H_
