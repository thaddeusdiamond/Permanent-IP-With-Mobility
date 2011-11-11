// Author: Thaddeus Diamond (diamond@cs.yale.edu)
//
// Deployment for any application (Echo Application the only one supported)

#include <cstdlib>
#include "Applications/Application.h"
#include "Applications/EchoApp.h"

#define MIN_ARGUMENTS 2
#define ECHO_NUM_ARGUMENTS 9

int main(int argc, char* argv[]) {
  if (argc < MIN_ARGUMENTS)
    Die("Must specify an application to use");

  if (!strcmp(argv[1], "EchoApp")) {
    if (argc != ECHO_NUM_ARGUMENTS)
      Die("Usage: ./RunApp EchoApp [Msg] [LA (Port)] [Peer (Port)] [DNS] [RS]");

    EchoApp* application =
      new EchoApp(string(argv[2]), string(argv[3]), atoi(argv[4]),
                  string(argv[5]), atoi(argv[6]), string(argv[7]),
                  string(argv[8]));
    return application->Start();
  }

  exit(EXIT_FAILURE);
}
