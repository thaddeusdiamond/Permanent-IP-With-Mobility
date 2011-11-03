// Author: Thaddeus Diamond (diamond@cs.yale.edu)
//
// This is an abstract interface for any application that wants to run using
// Mobile IP

#ifndef _PERMANENTIP_APPLICATIONS_APPLICATION_H_
#define _PERMANENTIP_APPLICATIONS_APPLICATION_H_

class Application {
 public:
  virtual ~Application() {}

  // Application-like "run" and "shutdown" paradigm
  virtual bool Start() = 0;
  virtual bool ShutDown(const char* format, ...) = 0;

 protected:
  // Create a mobile node to monitor our location/update rendezvous server
  virtual int CreateMobileNodeDelegate() = 0;
};

#endif  // _PERMANENTIP_APPLICATIONS_APPLICATION_H_
