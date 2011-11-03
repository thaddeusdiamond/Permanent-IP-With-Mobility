// Author: Thaddeus Diamond (diamond@cs.yale.edu)
//
// This is an abstract interface for a mobile node traveling on the network

#ifndef _PERMANENTIP_MOBILENODE_MOBILENODE_H_
#define _PERMANENTIP_MOBILENODE_MOBILENODE_H_

class MobileNode {
 public:
  virtual ~MobileNode() {}

  // We use a daemon-like "run" and "shutdown" paradigm
  virtual void Start() = 0;
  virtual bool ShutDown(const char* format, ...) = 0;

  // Any application needs to register an open socket so that it can be
  // intercepted and virtually tunneled
  virtual struct sockaddr* RegisterPeer(int app_socket,
                                        LogicalAddress peer_addr,
                                        unsigned short peer_port) = 0;

 protected:
//  // A mobile agent needs to instantiate a connection to the home agent
//  virtual void ConnectToHome(unsigned short port, char* data, bool initial) = 0;

//  // A mobile node needs to update the home agent when it's IP changes
//  virtual void ChangeHomeIdentity() = 0;
};

#endif  // _PERMANENTIP_MOBILENODE_MOBILENODE_H_
