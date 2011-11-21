/**
 * @file
 * @author Thaddeus Diamond <diamond@cs.yale.edu>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * This is an abstract interface for any application running on top of our
 * IP implementation
 **/

#ifndef _PERMANENTIP_APPLICATIONS_APPLICATION_H_
#define _PERMANENTIP_APPLICATIONS_APPLICATION_H_

class Application {
 public:
  /**
   * The destructor is responsible for freeing any memory accumulated during
   * the course of the application's running
   **/
  virtual ~Application() {}

  /**
   * The application follows a typical server-like mechanism, where the Start()
   * function is responsible for creating a Mobile Node delegate, connecting
   * to the peer, and exchanging messages.
   *
   * @returns   True unless there was an error connecting to the peer
   **/
  virtual bool Start() = 0;

  /**
   * In order to exit the application, we use a SIGINT, which calls the
   * ShutDown() method (closing open sockets and telling the program to exit)
   *
   * @param     format    The format of the message to print before exiting
   * @param     ...       The list of arguments to pass to the exit message
   *
   * @returns   False always
   **/
  virtual bool ShutDown(const char* format, ...) = 0;

 protected:
  /**
   * All applications must create an @ref MobileNode to monitor our
   * location and update the rendezvous server
   *
   * @returns   The status of the thread spawned to run the mobile node delegate
   **/
  virtual int CreateMobileNodeDelegate() = 0;
};

#endif  // _PERMANENTIP_APPLICATIONS_APPLICATION_H_
