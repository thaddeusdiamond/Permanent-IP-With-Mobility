/**
 * @file
 * @author Thaddeus Diamond <diamond@cs.yale.edu>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * Testing for a simple Mobile Node
 **/

#include <pthread.h>
#include <gtest/gtest.h>
#include "MobileNode/SimpleMobileNode.h"

/** @todo These tests will hang because they do not override typical
          integrated functions into dummy black boxes. **/

// Non-member function required by PThread
static inline void* RunMobileNodeThread(void* mobile_node) {
  EXPECT_TRUE((reinterpret_cast<SimpleMobileNode*>(mobile_node))->Start());
  return NULL;
}

namespace {
  class SimpleMobileNodeTest : public ::testing::Test {
   protected:
    // Create a MobileNode server before each test
    SimpleMobileNodeTest() :
        domain_(GLOB_DOM), transport_layer_(GLOB_TL), protocol_(GLOB_PROTO) {
      mobile_node_ = new SimpleMobileNode("localhost", "127.0.0.1",
                                          "127.0.0.1");

      pthread_create(&mobile_node_daemon_, NULL, &RunMobileNodeThread,
                     mobile_node_);
      sleep(1);
    }

    // Destroy MobileNode memory
    virtual ~SimpleMobileNodeTest() {
      pthread_join(mobile_node_daemon_, NULL);
      delete mobile_node_;
    }

    // Unused virtual methods
    virtual void SetUp() {}
    virtual void TearDown() {}

    // Create a member variable for the thread
    pthread_t mobile_node_daemon_;

    // Create a member variable for the MobileNode
    SimpleMobileNode* mobile_node_;

    // Create member variables for connectivity
    Domain domain_;
    TransportLayer transport_layer_;
    Protocol protocol_;
  };
}

/**
 * @test    Mobile Node start and stop test (basic functionality)
 **/
TEST_F(SimpleMobileNodeTest, StartsAndStops) {
  ASSERT_FALSE(mobile_node_->ShutDown("Normal termination"));
}

int main(int argc, char* argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
