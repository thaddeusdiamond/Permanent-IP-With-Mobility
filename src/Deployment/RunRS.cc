/**
 * @file
 * @author Thaddeus Diamond <diamond@cs.yale.edu>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * Deployment for any RS (SimpleRendezvousServer the only one supported)
 **/

#include <cstdlib>
#include "RendezvousServer/RendezvousServer.h"
#include "RendezvousServer/SimpleRendezvousServer.h"

using Utils::Die;

#define MIN_ARGUMENTS 2
#define SRS_NUM_ARGUMENTS 2

int main(int argc, char* argv[]) {
  if (argc < MIN_ARGUMENTS)
    Die("Must specify an RS to use");

  if (!strcmp(argv[1], "SRS")) {
    if (argc != SRS_NUM_ARGUMENTS)
      Die("Usage: ./RunRS SRS");

    RendezvousServer* rendezvous_server =
      new SimpleRendezvousServer();
    return rendezvous_server->Start();
  }

  exit(EXIT_FAILURE);
}
