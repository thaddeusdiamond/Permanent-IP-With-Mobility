/**
 * @file
 * @author Thaddeus Diamond <diamond@cs.yale.edu>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * Testing for a simple application
 **/

#include <pthread.h>
#include <gtest/gtest.h>
#include "Applications/EchoApp.h"

/** @todo These tests will hang because they do not override typical
          integrated functions into dummy black boxes. **/

// Non-member function required by PThread
static inline void* RunEchoAppThread(void* echo_app) {
  EXPECT_TRUE((reinterpret_cast<EchoApp*>(echo_app))->Start());
  return NULL;
}

namespace {
  class EchoAppTest : public ::testing::Test {
   protected:
    // Create a DNS server before each test (Python is DNS lookup here)
    EchoAppTest() :
        domain_(GLOB_DOM), transport_layer_(GLOB_TL), protocol_(GLOB_PROTO) {
      // Run on tick (LA), with peer@dolphin DNS@python and RS@termite
      echo_app_ =
        new EchoApp("Disco, dude!", "tick.cs.yale.edu", 16000,
                    "dolphin.cs.yale.edu", 16000, "128.36.232.46",
                    "128.36.232.28");

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
/**
 * @test    Echo App start and stop test (basic functionality)
 **/
TEST_F(EchoAppTest, StartsAndStops) {
  ASSERT_FALSE(echo_app_->ShutDown("Normal termination"));
}

int main(int argc, char* argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
