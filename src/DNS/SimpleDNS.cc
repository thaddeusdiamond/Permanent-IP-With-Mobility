// Author: Thaddeus Diamond (diamond@cs.yale.edu)
//
// This is an implementation for a simple version of DNS

#include "DNS/SimpleDNS.h"

bool SimpleDNS::Start() {
  assert(AddName("python.cs.yale.edu", "128.36.232.46"));
  assert(BeginListening());

  Signal::HandleSignalInterrupts();
  do {
    assert(HandleRequests());
  } while (Signal::ShouldContinue());
  return true;
}

bool SimpleDNS::ShutDown() {
  Signal::ExitProgram(0);
  return true;
}

bool SimpleDNS::BeginListening() {
  return true;
}

bool SimpleDNS::HandleRequests() {
  return true;
}

bool SimpleDNS::AddName(LogicalAddress name, PhysicalAddress address) {
  registered_names_[name] = address;
  return true;
}

PhysicalAddress SimpleDNS::LookupName(LogicalAddress name) {
  return (registered_names_.count(name) > 0 ? registered_names_[name] : "");
}
