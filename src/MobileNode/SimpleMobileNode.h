/**
 * @file
 * @author Thaddeus Diamond <diamond@cs.yale.edu>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * This is an interface for a simple mobile node delegate running on a device
 **/

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

using Utils::Die;
using Utils::Log;
using Utils::IPStringToInt;
using Utils::GetCurrentIPAddress;

using std::tr1::unordered_map;
using std::string;

class SimpleMobileNode : public MobileNode {
 public:
  /**
   * The constructor for a mobile node is responsible for initializing
   * the various member variables
   **/
  SimpleMobileNode(LogicalAddress logical_address, PhysicalAddress dns_server,
                   PhysicalAddress rendezvous_server) :
    logical_address_(logical_address),
    last_known_ip_address_(GetCurrentIPAddress()), dns_server_(dns_server),
    rendezvous_server_(rendezvous_server), rendezvous_port_(GLOB_REGIST_PORT),
    domain_(GLOB_DOM), transport_layer_(GLOB_TL), protocol_(GLOB_PROTO) {}

  /**
   * The simple mobile node implementation destructor does not need to free
   * any data because none is aggregated
   **/
  virtual ~SimpleMobileNode() {}

  virtual bool Start();
  virtual bool ShutDown(const char* format, ...);
  virtual struct sockaddr* RegisterPeer(int app_socket,
                                        LogicalAddress peer_addr);

 protected:
  virtual void UpdateRendezvousServer();
  virtual void PollSubscriptions();

  /**
   * A mobile agent needs to connect to arbitrary servers to gain information.
   * However, this separation is not strictly required, we provide it for a
   * simple mobile node as a means of conciseness
   *
   * @param     server_addr     The physical address of the server to connect to
   * @param     server_port     The server port we will attempt the connection
   * @param     information     The message we wish to send to the server
   * @param     sender          The socket to send the message on (if there
   *                            isn't a special one we have in mind we create
   *                            a vanilla one)
   *
   * @returns   The message we received back from the server, or "" if an
   *            error occurred in connection
   **/
  NetworkMsg ConnectToServer(PhysicalAddress server_addr,
                             unsigned short server_port,
                             NetworkMsg information, int sender = -1);

  /**
   * We keep the current node's logical address stashed...
   **/
  LogicalAddress logical_address_;

  /**
   * ...and change the IP interface if we roamed
   **/
  int last_known_ip_address_;

  /**
   * Listed connection to the main DNS
   **/
  PhysicalAddress dns_server_;

  /**
   * Listed connection the main RS...
   **/
  PhysicalAddress rendezvous_server_;

  /**
   * ...and the port we connect to it on
   **/
  unsigned short rendezvous_port_;

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
   * We keep track of the applications with open sockets (a mapping of
   * app socket to the peer sockaddr struct it connects to)
   **/
  unordered_map<int, struct sockaddr*> app_sockets_;
};

#endif  // _PERMANENTIP_MOBILENODE_SIMPLEMOBILENODE_H_
