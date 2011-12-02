/**
 * @file
 * @author Thaddeus Diamond <diamond@cs.yale.edu>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * This is an abstract interface for any mobile node delegate running on a
 * device
 **/

#ifndef _PERMANENTIP_MOBILENODE_MOBILENODE_H_
#define _PERMANENTIP_MOBILENODE_MOBILENODE_H_

#include "Common/Types.h"

class MobileNode {
 public:
  /**
   * The destructor for a mobile node is responsible for freeing all memory
   * associated with the mobile node daemon
   **/
  virtual ~MobileNode() {}

  /**
   * The mobile node follows a typical server-like mechanism, where the Start()
   * function is responsible for registering the current IP address at the
   * designated rendezvous server, constantly updating the RS and polling
   * the existing subscriptions for updates.
   *
   * @returns  True unless there was an error connecting to the RS
    **/
  virtual bool Start() = 0;

  /**
   * In order to kill the mobile node daemon, we use a SIGINT, which calls the
   * ShutDown() method (closing open sockets and telling the program to exit)
   *
   * @param   format        The format of the message to print before exiting
   * @param   ...           The list of arguments to pass to the exit message
   *
   * @returns False always
   **/
  virtual bool ShutDown(const char* format, ...) = 0;

  /**
   * Any application needs to register an open socket so that it can be
   * intercepted and updated/polled
   *
   * @param   app_socket    The app socket being registered for the new peer
   * @param   peer_addr     The logical address of the peer being registered
   *
   * @returns A pointer to a generic sockaddr struct that can be reinterpreted
   *          by the application to send messages to the peer. The mobile
   *          node retains this pointer so that it can transparently
   *          update the peer's location
   **/
  virtual struct sockaddr* RegisterPeer(int app_socket,
                                        LogicalAddress peer_addr) = 0;
  /**
   * Any application needs to notify the mobile node client when a message is
   * sent so that it can pack it into a buffer and resend if necessary.
   *
   * @param   app_socket    The app socket on which to send the message
   * @param   message       The message being sent
   **/
  virtual void MessageSent(int app_socket, NetworkMsg message) = 0;

  /**
   * Any application needs to notify the mobile node client when a message is
   * received so that it can free it from the buffer pool and not resend in
   * the case of failure.
   *
   * @param   app_socket    The app socket on which to send the message
   * @param   message       The message for which ACK was received
   **/
  virtual void MessageReceived(int app_socket, NetworkMsg message) = 0;

 protected:
  /**
   * The mobile agent's major function is to automatically service out updates
   * in its location.
   */
  virtual void UpdateRendezvousServer() = 0;

  /**
   * The mobile node also polls the active subscriptions so that if any of our
   * peers have changed location we can reconnect with minimal packet loss.
   **/
  virtual void PollSubscriptions() = 0;
};

#endif  // _PERMANENTIP_MOBILENODE_MOBILENODE_H_
