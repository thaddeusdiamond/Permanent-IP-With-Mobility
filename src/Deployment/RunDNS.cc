/**
 * @file
 * @author Thaddeus Diamond <diamond@cs.yale.edu>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * Deployment for any DNS (Simple DNS is the only one supported)
 **/

#include <cstdlib>
#include "DNS/DNS.h"
#include "DNS/SimpleDNS.h"

using Utils::Die;

#define MIN_ARGUMENTS 2
#define SRS_NUM_ARGUMENTS 2

int main(int argc, char* argv[]) {
  if (argc < MIN_ARGUMENTS)
    Die("Must specify an DNS to use");

  if (!strcmp(argv[1], "SDNS")) {
    if (argc != SRS_NUM_ARGUMENTS)
      Die("Usage: ./RunDNS SDNS");

    DNS* dns = new SimpleDNS();
    return dns->Start();
  }

  exit(EXIT_FAILURE);
}
