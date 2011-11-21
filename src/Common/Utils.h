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
#include <cstdarg>
#include <cstring>
#include <cstdlib>

#include "Common/Types.h"

/**
 * We represent the various levels of the log as global integers so that
 * we can perform a fast switch when writing out
 **/
#define NUMBER_OF_LEVELS 5
enum Level {
  SUCCESS = 0,
  DEBUG = 1,
  WARNING = 2,
  ERROR = 3,
  FATAL = 4
};

/**
 * We represent the files of the output as an array for fast switching
 **/
#define LOGDIR        string("logs/")
#define LOGFILE       string("log")
static string level_files[] = {
  "log.success",
  "log.debug",
  "log.warning",
  "log.error",
  "log.fatal"
};

/**
 * We represent the colors of the output as an array for fast switching
 **/
#define COLOR_OUT
#define CLEAR_COLOR   "\033[0m"
static string level_colors[] = {
  "\033[1;32m",
  "\033[1;34m",
  "\033[1;33m",
  "\033[1;31m",
  "\033[1;39m"
};

/**
 * We may want to prepend the log level to the actual print out message
 **/
#define PREPENDED
static string level_descriptors[] = {
  "SUCCESS",
  "DEBUG",
  "WARNING",
  "ERROR",
  "FATAL"
};

/**
 * The Utils namespace defines several utility functions that ease application
 * writing including trimming strings, logging and automatic exit.
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

  /** @cond PRIVATE_NAMESPACE_MEMBERS **/
    /**
     * We can keep the log files open as static variables to prevent having to
     * slow down from opening and closing them on every log write
     **/
    static FILE* log_file = NULL;
    static FILE* level_file_handles[] = { NULL, NULL, NULL, NULL, NULL };

    /**
     * If all of the logfiles are closed we will make a call to the OpenLogFiles
     * function internally to open all the logfiles and assign static handles to
     * them.
     **/
    static void OpenLogFiles() {
      log_file = fopen((LOGDIR + LOGFILE).c_str(), "w");

      for (int i = 0; i < NUMBER_OF_LEVELS; i++) {
        level_file_handles[i] = fopen((LOGDIR + level_files[i]).c_str(), "w");
        if (!level_file_handles[i])
          Die("There was an error opening the log files for writing");
      }
    }
  /** @endcond **/

  /**
   * The generic Log function takes a specific level and, based on what it
   * was given, writes out the information to the proper file.  No matter what
   * the information is always written to the root log.
   *
   * @param       level     The level of the debug information
   * @param       format    The format of the message to print
   * @param       ...       The contents of the format used with args lib
   **/
  static inline void Log(Level level, const char* format, ...) {
    va_list arguments;
    va_start(arguments, format);
    
    if (log_file == NULL)
      OpenLogFiles();

    // Prepend level statement and add color to the output if macros defined
    char log_format[4096];
    memset(log_format, 0, sizeof(log_format));
#ifdef PREPENDED
  #ifdef COLOR_OUT
    snprintf(log_format, sizeof(log_format), "%s[%s]%s ",
             level_colors[level].c_str(), level_descriptors[level].c_str(),
             CLEAR_COLOR);
  #else
    snprintf(log_format, sizeof(log_format), "[%s] ",
             level_descriptors[level].c_str());
  #endif
#endif

    // There are issues with printing using va_args so we use vsprintf
    char buffer[4096];
    memset(buffer, 0, sizeof(buffer));
    vsprintf(buffer, format, arguments);
    strncat(log_format, buffer, strlen(buffer));

    // Make sure it's appended with a newline
    fprintf(log_file, log_format);
    fprintf(log_file, "\n");
    fprintf(level_file_handles[level], log_format);
    fprintf(level_file_handles[level], "\n");
    va_end(arguments);
  }

  /**
   * We overload the log function to take a file descriptor so that we can do
   * what we do normally, but to a specific file (used with stderr logging).
   *
   * @param       filedes   File descriptor to have log dumped to
   * @param       level     The level of the debug information
   * @param       format    The format of the message to print
   * @param       ...       The contents of the format used with args lib
   *
   * @return      True on successful finish, false if error
   **/
  static inline bool Log(FILE* filedes, Level level,
                         const char* format, ...) {
#ifdef PREPENDED
  #ifdef COLOR_OUT
    fprintf(filedes, "%s[%s]%s ", level_colors[level].c_str(),
            level_descriptors[level].c_str(), CLEAR_COLOR);
  #else
    fprintf(filedes, "[%s] ", level_descriptors[level].c_str());
  #endif
#endif

    va_list arguments;
    va_start(arguments, format);
    
    char buffer[4096];
    memset(buffer, 0, sizeof(buffer));
    vsprintf(buffer, format, arguments);
    
    fprintf(filedes, buffer);
    fprintf(filedes, "\n");
    va_end(arguments);

    return true;
  }

  /**
   * We overload the log function to take a filename so that we can do
   * what we do normally, but to a specific file.
   *
   * @param       filename  File to have log dumped to
   * @param       level     The level of the debug information
   * @param       format    The format of the message to print
   * @param       ...       The contents of the format used with args lib
   *
   * @return      True on successful finish, false if error
   **/
  static inline bool Log(const string& filename, Level level,
                         const char* format, ...) {
    FILE* log;
    log = fopen(filename.c_str(), "w");
    if (log == NULL)
      return false;

    va_list arguments;
    va_start(arguments, format);

    char buffer[4096];
    memset(buffer, 0, sizeof(buffer));
    vsprintf(buffer, format, arguments);

    return Log(log, level, buffer);
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
