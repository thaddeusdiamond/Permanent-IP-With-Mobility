/**
 * @file
 * @author Thaddeus Diamond <diamond@cs.yale.edu>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * This is an abstract interface for an arbitrary implementation of an RS
 **/

#ifndef _PERMANENTIP_RENDEZVOUSSERVER_RENDEZVOUSSERVER_H_
#define _PERMANENTIP_RENDEZVOUSSERVER_RENDEZVOUSSERVER_H_

#include <utility>
#include "Common/Types.h"

using std::pair;

class RendezvousServer {
 public:
  /**
   * The destructor for a rendezvous server must free any memory aggregated
   * by the RS.
   **/
  virtual ~RendezvousServer() {}

  /**
   * The rendezvous server follows a typical server-like mechanism, where the
   * Start() function is responsible for opening socket listeners for address
   * lookups and registrations, and then continually cycling through those two
   * sockets and polling for lookups and registrations.
   *
   * @returns  True unless there was an error connecting to the RS
    **/
  virtual bool Start() = 0;

  /**
   * In order to kill the rendezvous server, we use a SIGINT, which calls the
   * ShutDown() method (closing open sockets and telling the program to exit)
   *
   * @param   format        The format of the message to print before exiting
   * @param   ...           The list of arguments to pass to the exit message
   *
   * @returns False always
   **/
  virtual bool ShutDown(const char* format, ...) = 0;

 protected:
  /**
   * Every Rendezvous Server has the ability to register a new address and,
   * when that node is mobile, update that address.
   *
   * @param   name          The logical address to be updated
   * @param   address       The new physical address to assign to name
   *
   * @returns True unless there was an error listening on either port
   **/
  virtual bool UpdateAddress(LogicalAddress name, PhysicalAddress address) = 0;

  /**
   * When a peer is trying to connect to the user, it must request to be added
   * to the subscriber list, and there must be some mechanism to unsubscribe the
   * user from that list at some point.  When the subscriber is added to the
   * list the user's last known physical address is returned for the connecting
   * user to make a connection to.
   *
   * @param   subscriber    The logical-address|port combination of the
   *                        subscriber connected to the RS
   * @param   client        The name of the client the subscriber is changing
   *                        its subscription with
   *
   * @returns The last known physical address of the client subscribed to
   **/
  virtual PhysicalAddress ChangeSubscription(
      pair<LogicalAddress, unsigned short> subscriber,
      LogicalAddress client) = 0;
};

#endif  // _PERMANENTIP_RENDEZVOUSSERVER_RENDEZVOUSSERVER_H_
