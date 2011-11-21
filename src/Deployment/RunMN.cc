/**
 * @file
 * @author Thaddeus Diamond <diamond@cs.yale.edu>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * Dummy executable (MN not supported)
 **/

#include <cstdio>
#include "Common/Utils.h"

using Utils::Log;

int main(int argc, char* argv[]) {
  Log(stderr, FATAL, "MN not supported yet.");
  return 1;
}
