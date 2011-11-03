// Author: Thaddeus Diamond (diamond@cs.yale.edu)
//
// This is a test for our simple echo application

#include <pthread.h>
#include <gtest/gtest.h>
#include "Applications/EchoApp.h"

// Non-member function required by PThread
static inline void* RunEchoAppThread(void* echo_app) {
  EXPECT_TRUE((reinterpret_cast<EchoApp*>(echo_app))->Start());
  return NULL;
}

// Dummy tester class
namespace {
  class EchoAppTest : public ::testing::Test {
   protected:
    // Create a DNS server before each test (Python is DNS lookup here)
    EchoAppTest() :
        domain_(GLOB_DOM), transport_layer_(GLOB_TL), protocol_(GLOB_PROTO) {
      echo_app_ = new EchoApp("Disco, dude!", "dolphin.cs.yale.edu", 16000,
                              "128.36.232.46", 16000, 16000);

      pthread_create(&echo_app_daemon_, NULL, &RunEchoAppThread, echo_app_);
      sleep(1);
    }

    // Destroy DNS memory
    virtual ~EchoAppTest() {
      pthread_join(echo_app_daemon_, NULL);
      delete echo_app_;
    }

    // Unused virtual methods
    virtual void SetUp() {}
    virtual void TearDown() {}

    // Create a member variable for the thread
    pthread_t echo_app_daemon_;

    // Create a member variable for the echo_app
    EchoApp* echo_app_;

    // Create member variables for connectivity
    Domain domain_;
    TransportLayer transport_layer_;
    Protocol protocol_;
  };
}

// Start and stop test (basic functionality)
TEST_F(EchoAppTest, StartsAndStops) {
  ASSERT_FALSE(echo_app_->ShutDown("Normal termination"));
}

int main(int argc, char* argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
