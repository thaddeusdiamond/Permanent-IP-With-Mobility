/**
 * @file
 * @author Thaddeus Diamond <diamond@cs.yale.edu>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * This is an interface for a simple RS used in our basic scheme
 **/

#ifndef _PERMANENTIP_RENDEZVOUSSERVER_SIMPLERENDEZVOUSSERVER_H_
#define _PERMANENTIP_RENDEZVOUSSERVER_SIMPLERENDEZVOUSSERVER_H_

#include <gtest/gtest.h>
#include <tr1/unordered_map>
#include <fcntl.h>
#include <errno.h>

#include <cassert>
#include <cstdarg>
#include <set>
#include <utility>

#include "Common/Utils.h"
#include "Common/Signal.h"
#include "RendezvousServer/RendezvousServer.h"

using Utils::Die;
using Utils::Log;
using Utils::IntToIPName;
using Utils::IntToIPString;
using Utils::IPStringToInt;

using std::tr1::unordered_map;
using std::set;
using std::pair;

class SimpleRendezvousServer : public RendezvousServer {
 public:
  /**
   * The constructor instantiates default member variables
   **/
  explicit SimpleRendezvousServer() :
    registration_port_(GLOB_REGIST_PORT), lookup_port_(GLOB_LOOKUP_PORT),
    domain_(GLOB_DOM), transport_layer_(GLOB_TL), protocol_(GLOB_PROTO) {}

  /**
   * The destructor doesn't need to free nay memory because none is aggregated
   **/
  virtual ~SimpleRendezvousServer() {}

  virtual bool Start();
  virtual bool ShutDown(const char* format, ...);

 protected:
  virtual bool UpdateAddress(LogicalAddress name, PhysicalAddress address);
  virtual PhysicalAddress ChangeSubscription(
      pair<LogicalAddress, unsigned short> subscriber,
      LogicalAddress client);

  /**
   * We specifically want to respond to connections given to us on the specified
   * port only.
   *
   * @param     port            The port to listen for incoming requests on
   *                            (called for both listener and registration port)
   * @returns   The socket that the application is now listening to that port
   *            on unless there was an error (in which case ShutDown() called)
   **/
  int BeginListening(unsigned short port);

  /**
   * We make a call to handle incoming requests on a given socket every server
   * cycle.  We can specify whether this is the lookup or registration port
   * via a simple boolean
   *
   * @param     listener_socket The socket to poll data from
   * @param     lookup          Whether this is the lookup port (true) or the
   *                            registration port (false).
   **/
  bool HandleRequests(int listener_socket, bool lookup);

 private:
  /**
   * Privately, we keep a key-value store of logical addresses to physical
   * addresses that correspond to the nodes that have already registered at
   * this RS.
   **/
  unordered_map<LogicalAddress, PhysicalAddress> registered_names_;

  /**
   * In addition, we keep a key-value store of logical addresses to a set
   * of logical-address|port combinations so that when any logical address's
   * physical address is updated, we can send subscription updates
   * to every subscribed node
   **/
  unordered_map<LogicalAddress, set< pair<LogicalAddress, unsigned short> > >
    subscriptions_;

  /**
   * We maintain which port we are listening for incoming registrations on...
   **/
  unsigned short registration_port_;

  /**
   * ... and the actual socket that is open for registrations.
   **/
  int registration_listener_;

  /**
   * We maintain which port we are listening for incoming lookups on...
   **/
  unsigned short lookup_port_;

  /**
   * ... and the actual socket that is open for lookups.
   **/
  int lookup_listener_;

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

  // Declare friend tests for access to private methods
  friend class SimpleRendezvousServerTest;
  FRIEND_TEST(SimpleRendezvousServerTest, UpdatesAndHandlesSubscribers);
  FRIEND_TEST(SimpleRendezvousServerTest, HandlesNetworkRequests);
};

#endif  // _PERMANENTIP_RENDEZVOUSSERVER_SIMPLERENDEZVOUSSERVER_H_
