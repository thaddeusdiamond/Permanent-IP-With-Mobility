// Author: Thaddeus Diamond (diamond@cs.yale.edu)
//
// This is a standard header file for various utility functions

#ifndef _PERMANENTIP_COMMON_UTILS_H_
#define _PERMANENTIP_COMMON_UTILS_H_

#include <netinet/in.h>
#include <ifaddrs.h>
#include <cstdio>
#include <cstring>
#include <cstdlib>

static inline char* Trim(char* word) {
  if (!word || !word[0])
    return NULL;

  for (unsigned int i = 0; i < strlen(word) - 1; i++) {
    if (isspace(word[i]) && isspace(word[i + 1])) {
      word[i] = '\0';
      break;
    }
  }

  int end = strlen(word) - 1;
  if (isspace(word[end]))
    word[end] = '\0';

  return word;
}

static inline int GetCurrentIPAddress() {
  struct ifaddrs *if_address, *if_struct;
  getifaddrs(&if_struct);

  for (if_address = if_struct; if_address != NULL;
       if_address = if_address->ifa_next) {
    struct sockaddr_in* address = (struct sockaddr_in*) if_address->ifa_addr;
//    struct sockaddr_in* netmask =
//      (struct sockaddr_in*) if_address->ifa_netmask;
//    struct sockaddr_in* broad =
//      (struct sockaddr_in*) if_address->ifa_ifu.ifu_broadaddr;
//    struct sockaddr_in* dst =
//      (struct sockaddr_in*) if_address->ifa_ifu.ifu_dstaddr;

//    std::cout << if_address->ifa_name << std::endl;
//    if (address != NULL) {
//      std::cout << " ADDR(" << address->sin_addr.s_addr << ":" <<
//        address->sin_port << ")" << std::endl;
//    }
//    if (netmask != NULL) {
//      std::cout << " NETM(" << netmask->sin_addr.s_addr << ":" <<
//        netmask->sin_port << ")" << std::endl;
//    }
//    if (broad != NULL) {
//      std::cout << " BROAD(" << broad->sin_addr.s_addr << ":" <<
//        broad->sin_port << ")" << std::endl;
//    }
//    if (dst != NULL) {
//      std::cout << " DST(" << dst->sin_addr.s_addr << ":" <<
//        dst->sin_port << ")" << std::endl;
//    }

    if (address != NULL && ((address->sin_addr.s_addr << 24) >> 24) > 127) {
      int ip_number = address->sin_addr.s_addr;
      freeifaddrs(if_struct);
      return ip_number;
    }
  }

  return -1;
}

#endif  // _PERMANENTIP_COMMON_UTILS_H_
