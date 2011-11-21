/**
 * @file
 * @author Thaddeus Diamond <diamond@cs.yale.edu>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * This is a file defining the various types and statically defined
 * variables used throughout the application (this is Types and Configuration)
 **/

#ifndef _PERMANENTIP_COMMON_TYPES_H_
#define _PERMANENTIP_COMMON_TYPES_H_

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string>
#include <cctype>

using std::string;

/**
 * Addresses and messages sent across the network are defined as STL strings
 * for simplicity and power
 **/  
#define LogicalAddress string
#define PhysicalAddress string
#define NetworkMsg string

/**
 * We always lookup on the port specified by @ref GLOB_LOOKUP_PORT and register
 * on the port specified by @ref GLOB_REGIST_PORT to simplify our demo
 **/
#define GLOB_LOOKUP_PORT 16000
#define GLOB_REGIST_PORT 16001

/**
 * We limit the maximum possible traffic over a network to avoid accidental
 * DOS, rejection, etc.
 **/
#define MAX_ATTEMPTS 10
#define MAX_CONNECTIONS 64
#define FULL_SUBNET 16777215

/** @todo In the future we should support TCP & SCTP applications, but as of now
 *        we use this hash-define to say that we are using UDP always **/
#define UDP_APPLICATION
#define GLOB_TL UDP

/**
 * @enum TransportLayer
 *
 * This enumeration defines the various types that can be used for the
 * transport layer (raw socket, TCP, UDP [DEFAULT], or SCTP)
 **/
enum TransportLayer {
  RAW = SOCK_RAW,
  TCP = SOCK_STREAM,
  UDP = SOCK_DGRAM,
  SCTP = SOCK_SEQPACKET,
};

/** @todo In the future we should support IPv6 applications, but as of now
 *        we use this hash-define to say that we are using IPv4 always **/
#define IPv4_APPLICATION
#define GLOB_DOM NETv4

/**
 * @enum Domain
 *
 * The domain enumeration defines what domain is used while creating
 * sockets (IPv4 [DEFAULT], IPv6, raw packet)
 **/
enum Domain {
  PACKET = AF_PACKET,
  NETv4 = PF_INET,
  NETv6 = PF_INET6,
};

#define PROTO_NO_APPLICATION
#define GLOB_PROTO NO_TYPE

/**
 * @enum Protocol
 *
 * The protocol enumeration defines what protocol is used in initializing
 * a socket (NO_TYPE [DEFAULT], TCP, UDP, SCTP)
 **/
enum Protocol {
  NO_TYPE = 0,
  TCP_PROTO = IPPROTO_TCP,
  UDP_PROTO = IPPROTO_UDP,
  SCTP_PROTO = IPPROTO_SCTP,
};

#endif  // _PERMANENTIP_COMMON_TYPES_H_
