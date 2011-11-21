/**
 * @file
 * @author Thaddeus Diamond <diamond@cs.yale.edu>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * This is an abstract interface for an arbitrary implementation of DNS
 **/

#ifndef _PERMANENTIP_DNS_DNS_H_
#define _PERMANENTIP_DNS_DNS_H_

#include "Common/Types.h"

class DNS {
 public:
  /**
   * The destructor for a DNS is in charge of freeing all memory aggregated
   * by the class
   **/
  virtual ~DNS() {}

  /**
   * The Start function for our DNS uses a "run" paradigm to add some sample
   * names (for use in the demo), begins listening for lookups on the
   * @ref GLOB_LOOKUP_PORT and then handles requests until there is a SIGINT
   *
   * @returns   True unless the setup encountered an error
   **/
  virtual bool Start() = 0;

  /**
   * The ShutDown method is typically triggered by a SIGINT.  It is responsible
   * for printing the exit message and closing the open listener socket
   *
   * @param     format    A string identifier to print upon exit
   * @param     ...       The arguments passed to the formatted message
   *
   * @returns   False always
   **/
  virtual bool ShutDown(const char* format, ...) = 0;

 protected:
  /**
   * Every DNS should have the ability to privately add a namespace to its
   * hierarchy.
   *
   * @param     name      The logical address being added to the DNS registrar
   * @param     address   The physical address of the RS in charge of this name
   *
   * @returns   True if the name was successfully added to the registrar, false
   *            otherwise.
   **/
  virtual bool AddName(LogicalAddress name, PhysicalAddress address) = 0;

  /**
   * Given a specific logical address, the DNS should be able to lookup and
   * return its physical address.
   *
   * @param     name      The logical address to be looked up
   *
   * @returns   The physical address of the RS in charge of that logical address
   *            if it is in the registrar (as a std::string), "" otherwise
   **/
  virtual PhysicalAddress LookupName(LogicalAddress name) = 0;
};

#endif  // _PERMANENTIP_DNS_DNS_H_
