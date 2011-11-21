/**
 * @file
 * @author Thaddeus Diamond <diamond@cs.yale.edu>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * This is a generic utility library to provide the programmers convenient
 * methods while developing
 **/

#ifndef _PERMANENTIP_COMMON_UTILS_H_
#define _PERMANENTIP_COMMON_UTILS_H_

#include <netdb.h>
#include <netinet/in.h>
#include <ifaddrs.h>
#include <string>
#include <cstdio>
#include <cstring>
#include <cstdlib>

#include "Common/Types.h"

/**
 * The Utils namespace defines several utility functions that ease application
 * writing including trimming strings and automatic exit.
 *
 * @addtogroup  Utilities
 **/
namespace Utils {
  /**
   * We want an easy, generic, macro-like function for exiting from anywhere
   * in the program.
   *
   * @param   format            The formatted message to print upon exiting.
   **/
  static inline void Die(const char* format, ...) {
    va_list arguments;
    va_start(arguments, format);

    fprintf(stderr, format, arguments);
    fprintf(stderr, "\n");

    exit(1);
  }

  /**
   * The C library doesn't provide a great built-in way to trim whitespace
   * from a character array, so we provide the Trim utility to do so. For
   * example, " abc\n" => "abc"
   *
   * @param   word              The character array (string) to be trimmed
   * @returns A pointer to that same character array, now trimmed
   **/
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

  /**
   * IntToIPName() provides a utility to convert a standard IPv4 integer-based
   * address given from sockaddr_in.sin_addr.s_addr to an IP name provided
   * by getnameinfo [i.e. "python.zoo.cs.yale.edu"] (see man)
   *
   * @param   physical_address  The integer based IP address to convert
   * @returns The IP String corresponding to the provided integer IP address
   **/
  static inline LogicalAddress IntToIPName(int physical_address) {
    struct sockaddr_in lookup;
    lookup.sin_family = GLOB_DOM;
    lookup.sin_addr.s_addr = physical_address;
    lookup.sin_port = htons(10000);
    socklen_t lookup_size = sizeof(lookup);

    char buffer[4096];
    getnameinfo(reinterpret_cast<struct sockaddr*>(&lookup), lookup_size,
                buffer, sizeof(buffer), NULL, 0, 0);
    return string(buffer);
  }

  /**
   * IntToIPString() provides a utility to convert a standard IPv4 integer-based
   * address given from sockaddr_in.sin_addr.s_addr to an IP string
   * (i.e. "127.0.0.1")
   *
   * @param   physical_address  The integer based IP address to convert
   * @returns The IP String corresponding to the provided integer IP address
   **/
  static inline PhysicalAddress IntToIPString(int physical_address) {
    int low_order = physical_address & 255;
    int low_middle_order = (physical_address >> 8) & 255;
    int high_middle_order = (physical_address >> 16) & 255;
    int high_order = (physical_address >> 24) & 255;

    char buffer[100];
    snprintf(buffer, sizeof(buffer), "%d.%d.%d.%d", low_order, low_middle_order,
             high_middle_order, high_order);
    return string(buffer);
  }

  /**
   * IPStringToInt() provides a reverse utility to take a physical address
   * specified in a string, and convert that to an integer that can be used
   * by a sockaddr_in struct
   *
   * @param   physical_address  The string representing an IP address to convert
   * @returns The integer representation of the provided IP address
   **/
  static inline int IPStringToInt(PhysicalAddress physical_address) {
    unsigned int address = 0;
    int offset = 0;

    // Fix fencepost issue (hack)
    physical_address += ".";
    do {
      address += atoi(physical_address.c_str()) << offset;
      offset += 8;
      physical_address =
        physical_address.substr(physical_address.find(".") + 1);
    } while (physical_address.find(".") != string::npos);

    return address;
  }

  /**
   * The GetCurrentIPAddress() method returns the lowest specified interface
   * currently available on the machine as specified by getifaddrs (see man).
   *
   * @returns An integer representing the current IPv4 address to be used
   *          by a struct sockaddr_in
   **/
  static inline int GetCurrentIPAddress() {
    struct ifaddrs *if_address, *if_struct;
    getifaddrs(&if_struct);

    for (if_address = if_struct; if_address != NULL;
         if_address = if_address->ifa_next) {
      struct sockaddr_in* address = (struct sockaddr_in*) if_address->ifa_addr;
      if (address != NULL && ((address->sin_addr.s_addr << 24) >> 24) > 127) {
        int ip_number = address->sin_addr.s_addr;
        freeifaddrs(if_struct);
        return ip_number;
      }
    }

    return -1;
  }
}

#endif  // _PERMANENTIP_COMMON_UTILS_H_
