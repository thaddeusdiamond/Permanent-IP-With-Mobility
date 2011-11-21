/**
 * @file
 * @author Thaddeus Diamond <diamond@cs.yale.edu>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * This is a library for handling signal interrupts using a server-like
 * application lifecycle
 **/

#ifndef _PERMANENTIP_COMMON_SIGNAL_H_
#define _PERMANENTIP_COMMON_SIGNAL_H_

#include <signal.h>

/**
 * The Signal namespace controls the lifecycle of the various server-like
 * applications we have created.  Because SIGINT's may occur in various points
 * of the execution, we create one location for handle SIGINT's that do
 * then control the execution on the server.
 *
 * @addtogroup  Signal
 **/
namespace Signal {
  /**
   * The exit flag determines whether or not the application should exit
   **/
  static bool exit_flag = false;

  /**
   * ShouldContinue() will always return yes unless ExitProgram() has
   * been previously called
   *
   * @returns   True unless ExitProgram() has been called at any point
   **/
  static inline bool ShouldContinue() {
    return !exit_flag;
  }

  /**
   * ExitProgram() is automatically called on a signal interrupt and alerts
   * the program we should exit ASAP.
   *
   * @param     parameter   Default parameter passed in by signal handler
   **/
  static inline void ExitProgram(int parameter) {
    exit_flag = true;
  }

  /**
   * Sometimes we may want to restart the program (ensure that there is
   * no dirty-ness associated with the exit flag).  This method does that.
   **/
  static inline void RestartProgram() {
    exit_flag = false;
  }

  /**
   * HandleSignalInterrupts() is called before we begin the server lifecycle
   * so that we can initiate the signal handlers to respond to users
   **/
  static inline void HandleSignalInterrupts() {
    signal(SIGINT, &ExitProgram);
    signal(SIGTERM, &ExitProgram);
  }
}

#endif  // _PERMANENTIP_COMMON_SIGNAL_H_
