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
    SimpleDNSTest() :
        domain_(GLOB_DOM), transport_layer_(GLOB_TL), protocol_(GLOB_PROTO) {
      dns_ = new SimpleDNS(16000);

      pthread_create(&dns_daemon_, NULL, &RunDNSThread, dns_);
      sleep(1);
    }

    // Destroy DNS memory
    virtual ~SimpleDNSTest() {
      pthread_join(dns_daemon_, NULL);
      delete dns_;
    }

    // Unused virtual methods
    virtual void SetUp() {}
    virtual void TearDown() {}

    // Create a member variable for the thread
    pthread_t dns_daemon_;

    // Create a member variable for the dns
    SimpleDNS* dns_;

    // Create member variables for connectivity
    Domain domain_;
    TransportLayer transport_layer_;
    Protocol protocol_;
  };
}

// Start and stop test (basic functionality)
TEST_F(SimpleDNSTest, StartsAndStops) {
  ASSERT_FALSE(dns_->ShutDown());
}

// Ensure adding names and lookups are correct
TEST_F(SimpleDNSTest, AddsAndLooksUp) {
  ASSERT_TRUE(dns_->AddName("tick.cs.yale.edu", "128.36.232.50"));
  EXPECT_NE(dns_->LookupName("tick.cs.yale.edu"), "128.36.232.49");
  EXPECT_EQ(dns_->LookupName("tick.cs.yale.edu"), "128.36.232.50");

  ASSERT_FALSE(dns_->ShutDown());
}

// Ensure it handles network lookups
TEST_F(SimpleDNSTest, HandlesNetworkRequests) {
  int sender = socket(domain_, transport_layer_, protocol_);

  struct sockaddr_in server;
  memset(&server, 0, sizeof(server));
  server.sin_family = domain_;
  server.sin_addr.s_addr = 16777343;
  server.sin_port = htons(16000);
  socklen_t server_size = sizeof(server);


  int on;
  setsockopt(sender, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char*>(&on),
             sizeof(on));

  char buffer[4096] = "python.cs.yale.edu";
#ifdef UDP_APPLICATION
  sendto(sender, buffer, sizeof(buffer), 0,
         reinterpret_cast<struct sockaddr*>(&server),
         server_size);
  recvfrom(sender, buffer, sizeof(buffer), 0,
           reinterpret_cast<struct sockaddr*>(&server),
           &server_size);
#endif
#ifdef TCP_APPLICATION
  server_size = 0;
  fprintf(stderr, "TCP is not yet supported\n");
  ASSERT_TRUE(false);
#endif

  EXPECT_EQ(string(buffer), "128.36.232.46");
  ASSERT_FALSE(dns_->ShutDown());
}

int main(int argc, char* argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
