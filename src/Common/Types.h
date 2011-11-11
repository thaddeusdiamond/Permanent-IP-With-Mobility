// Author: Thaddeus Diamond (diamond@cs.yale.edu)
//
// This is a standard header file for various connectivity switches

#ifndef _PERMANENTIP_COMMON_TYPES_H_
#define _PERMANENTIP_COMMON_TYPES_H_

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string>
#include <cctype>

#define LogicalAddress string
#define PhysicalAddress string
#define NetworkMsg string

#define LOCALHOST 16777343
#define GLOB_LOOKUP_PORT 16000
#define GLOB_REGIST_PORT 16001

#define IP_NAME_LENGTH 20
#define IP_PORT_LENGTH 10
#define IPv6_ADDR_LENGTH 16

#define MAX_ATTEMPTS 10
#define MAX_CONNECTIONS 64
#define FULL_SUBNET 16777215

using std::string;

#define UDP_APPLICATION
#define GLOB_TL UDP

enum TransportLayer {
  RAW = SOCK_RAW,
  TCP = SOCK_STREAM,
  UDP = SOCK_DGRAM,
  SCTP = SOCK_SEQPACKET,
};

#define IPv4_APPLICATION
#define GLOB_DOM NETv4

enum Domain {
  PACKET = AF_PACKET,
  NETv4 = PF_INET,
  NETv6 = PF_INET6,
};

#define PROTO_NO_APPLICATION
#define GLOB_PROTO NO_TYPE

enum Protocol {
  NO_TYPE = 0,
  UDP_PROTO = IPPROTO_UDP,
  SCTP_PROTO = IPPROTO_SCTP,
};

#endif  // _PERMANENTIP_COMMON_TYPES_H_
