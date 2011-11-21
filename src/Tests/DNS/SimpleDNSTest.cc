/**
 * @file
 * @author Thaddeus Diamond <diamond@cs.yale.edu>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * Testing for a simple DNS
 **/

#include <pthread.h>
#include <gtest/gtest.h>
#include "DNS/SimpleDNS.h"

using Utils::GetCurrentIPAddress;
using Utils::Log;

// Non-member function required by PThread
static inline void* RunDNSThread(void* dns) {
  EXPECT_TRUE((reinterpret_cast<SimpleDNS*>(dns))->Start());
  return NULL;
}

namespace {
  class SimpleDNSTest : public ::testing::Test {
   protected:
    // Create a DNS server before each test
    SimpleDNSTest() :
        domain_(GLOB_DOM), transport_layer_(GLOB_TL), protocol_(GLOB_PROTO) {
      dns_ = new SimpleDNS();

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

/**
 * @test    DNS start and stop test (basic functionality)
 **/
TEST_F(SimpleDNSTest, StartsAndStops) {
  ASSERT_FALSE(dns_->ShutDown("Normal termination"));
}

/**
 * @test    DNS Domain additions and lookups (not over the network)
 **/
TEST_F(SimpleDNSTest, AddsAndLooksUp) {
  ASSERT_TRUE(dns_->AddName("tick.cs.yale.edu", "128.36.232.50"));
  EXPECT_NE(dns_->LookupName("tick.cs.yale.edu"), "128.36.232.49");
  EXPECT_EQ(dns_->LookupName("tick.cs.yale.edu"), "128.36.232.50");

  ASSERT_FALSE(dns_->ShutDown("Normal termination"));
}

/**
 * @test    DNS lookups for nameservers (over the network)
 **/
TEST_F(SimpleDNSTest, HandlesNetworkRequests) {
  int sender = socket(domain_, transport_layer_, protocol_);

  struct sockaddr_in server;
  memset(&server, 0, sizeof(server));
  server.sin_family = domain_;
  server.sin_addr.s_addr = GetCurrentIPAddress();
  server.sin_port = htons(GLOB_LOOKUP_PORT);
  socklen_t server_size = sizeof(server);


  int on;
  setsockopt(sender, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char*>(&on),
             sizeof(on));

  char buffer[19] = "python";
#ifdef UDP_APPLICATION
  sendto(sender, buffer, sizeof(buffer), 0,
         reinterpret_cast<struct sockaddr*>(&server),
         server_size);
  recvfrom(sender, buffer, sizeof(buffer), 0,
           reinterpret_cast<struct sockaddr*>(&server),
           &server_size);
#elif TCP_APPLICATION
  server_size = 0;
  Log(stderr, FATAL, "TCP is not yet supported");
  ASSERT_TRUE(false);
#endif
  EXPECT_EQ(string(buffer), "128.36.232.37");

  char failed_buffer[19] = "monkey.cs.yale.edu";
#ifdef UDP_APPLICATION
  sendto(sender, failed_buffer, sizeof(failed_buffer), 0,
         reinterpret_cast<struct sockaddr*>(&server),
         server_size);
  recvfrom(sender, failed_buffer, sizeof(failed_buffer), 0,
           reinterpret_cast<struct sockaddr*>(&server),
           &server_size);
#elif TCP_APPLICATION
  server_size = 0;
  Log(stderr, FATAL, "TCP is not yet supported");
  ASSERT_TRUE(false);
#endif
  EXPECT_EQ(string(failed_buffer), "");

  ASSERT_FALSE(close(sender));
  ASSERT_FALSE(dns_->ShutDown("Normal termination"));
}

int main(int argc, char* argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
