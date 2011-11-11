// Author: Thaddeus Diamond (diamond@cs.yale.edu)
//
// This is an abstract interface for a mobile node traveling on the network

#ifndef _PERMANENTIP_MOBILENODE_MOBILENODE_H_
#define _PERMANENTIP_MOBILENODE_MOBILENODE_H_

class MobileNode {
 public:
  virtual ~MobileNode() {}

  // We use a daemon-like "run" and "shutdown" paradigm
  virtual bool Start() = 0;
  virtual bool ShutDown(const char* format, ...) = 0;

  // Any application needs to register an open socket so that it can be
  // intercepted and virtually tunneled
  virtual struct sockaddr* RegisterPeer(int app_socket,
                                        LogicalAddress peer_addr) = 0;

 protected:
  // The mobile agent's major function is to automatically service out updates
  // in its location and poll the active subscriptions so that if any of our
  // peers have changed location we can reconnect with minimal packet loss.
  virtual void UpdateRendezvousServer() = 0;
  virtual void PollSubscriptions() = 0;
};

#endif  // _PERMANENTIP_MOBILENODE_MOBILENODE_H_
