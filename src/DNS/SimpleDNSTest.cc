// Author: Thaddeus Diamond (diamond@cs.yale.edu)
//
// Test for a simple mobile node

#include <pthread.h>
#include <gtest/gtest.h>
#include "DNS/SimpleDNS.h"

// Non-member function required by PThread
static inline void* RunDNSThread(void* dns) {
  (reinterpret_cast<SimpleDNS*>(dns))->Start();
  return NULL;
}

// Dummy tester class
namespace {
  class SimpleDNSTest : public ::testing::Test {
   protected:
    // Create a DNS server before each test
    SimpleDNSTest() {
      dns_ = new SimpleDNS(16000);

      pthread_t dns_daemon;
      pthread_create(&dns_daemon, NULL, &RunDNSThread, dns_);
      sleep(1);
    }

    // Destroy DNS memory
    virtual ~SimpleDNSTest() {
      delete dns_;
    }

    // Unused virtual methods
    virtual void SetUp() {}
    virtual void TearDown() {}

    // Create a member variable for the dns
    SimpleDNS* dns_;
  };
}

// Start and stop test (basic functionality)
TEST_F(SimpleDNSTest, StartsAndStops) {
  ASSERT_TRUE(dns_->ShutDown());
}

// Ensure adding names and lookups are correct
TEST_F(SimpleDNSTest, AddsAndLooksUp) {
  ASSERT_TRUE(dns_->AddName("tick.cs.yale.edu", "128.36.232.50"));
  EXPECT_NE(dns_->LookupName("tick.cs.yale.edu"), "128.36.232.49");
  EXPECT_EQ(dns_->LookupName("tick.cs.yale.edu"), "128.36.232.50");
}

// TODO(Thad):  Ensure it handles network lookups
TEST_F(SimpleDNSTest, HandlesNetworkRequests) {
}

int main(int argc, char* argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
