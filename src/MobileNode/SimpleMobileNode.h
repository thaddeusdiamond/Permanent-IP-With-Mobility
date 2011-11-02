// Author: Thaddeus Diamond (diamond@cs.yale.edu)
//
// This is an abstract interface for a simple implementation of the mobile node
// in Mobile IP

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
//#include "Common/Signal.h"
//#include "Common/Types.h"
#include "MobileNode/MobileNode.h"

//using std::map;
//using std::set;

class SimpleMobileNode : public MobileNode {
// public:
//  SimpleMobileNode(IPAddress home_ip_address, unsigned short home_port,
//                   unsigned short change_port, unsigned short data_port,
//                   unsigned short listener_port,
//                   TransportLayer transmission_type = TCP,
//                   Domain domain = NETv4, Protocol protocol = NO_TYPE) {
//    home_port_ = home_port;
//    change_port_ = change_port;
//    data_port_ = data_port;

//    transmission_type_ = transmission_type;
//    domain_ = domain;
//    protocol_ = protocol;
//    listener_port_ = listener_port;

//    home_ip_address_ = home_ip_address;
//    last_known_ip_address_ = GetCurrentIPAddress();

//    tunnel_name_ = new char[IFNAMSIZ + 1];
//    memset(tunnel_name_, 0, IFNAMSIZ + 1);
//  }
//  virtual ~SimpleMobileNode() {
//    delete tunnel_name_;
//  }

//  // We use a daemon-like "run" paradigm
//  virtual void Run();

//  // We also need a "shutdown" mechanism that we can access with a SIGINT
//  virtual void ShutDown(bool should_exit, const char* format, ...);

//  // Allow applications to know where to listen
//  virtual int GetPermanentAddress() { return permanent_address_; }

//  // Any application needs to register an open socket so that it can be
//  // intercepted
//  virtual bool RegisterPeer(int peer_address);

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

//  // Listed connection to home IP address
//  IPAddress home_ip_address_;
//  unsigned short home_port_;
//  unsigned short change_port_;
//  unsigned short data_port_;
//  
//  // We maintain what mailbox the home agent is keeping open for us
//  unsigned int permanent_address_;
//  unsigned short permanent_port_;

//  // The mobile node needs to know what port to accept incoming traffic on
//  // when the home agent receives something and forwards it to us.
//  unsigned short listener_port_;

//  // We keep the last known identity and change the interface if that's
//  // no longer our IP
//  int last_known_ip_address_;

//  // We must specify what type of transport layer protocol we are using
//  TransportLayer transmission_type_;
//  Domain domain_;
//  Protocol protocol_;

//  // We keep track of the virtual tunnel applications are using
//  char* tunnel_name_;
//  int tunnel_fd_;
//  int permanent_address;
//  set<int> peer_fds_;
};

#endif  // _PERMANENTIP_MOBILENODE_SIMPLEMOBILENODE_H_
