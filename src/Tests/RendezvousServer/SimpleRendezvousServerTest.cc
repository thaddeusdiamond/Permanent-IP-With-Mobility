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
#include "RendezvousServer/SimpleRendezvousServer.h"

using Utils::GetCurrentIPAddress;

// Non-member function required by PThread
static inline void* RunRendezvousServerThread(void* rendezvous_server) {
  EXPECT_TRUE((reinterpret_cast<SimpleRendezvousServer*>(
               rendezvous_server))->Start());
  return NULL;
}

namespace {
  class SimpleRendezvousServerTest : public ::testing::Test {
   protected:
    // Create a RendezvousServer server before each test
    SimpleRendezvousServerTest() :
        domain_(GLOB_DOM), transport_layer_(GLOB_TL), protocol_(GLOB_PROTO) {
      rendezvous_server_ = new SimpleRendezvousServer();

      pthread_create(&rendezvous_server_daemon_, NULL,
                     &RunRendezvousServerThread, rendezvous_server_);
      sleep(1);
    }

    // Destroy RendezvousServer memory
    virtual ~SimpleRendezvousServerTest() {
      pthread_join(rendezvous_server_daemon_, NULL);
      delete rendezvous_server_;
    }

    // Unused virtual methods
    virtual void SetUp() {}
    virtual void TearDown() {}

    // Create a member variable for the thread
    pthread_t rendezvous_server_daemon_;

    // Create a member variable for the rendezvous_server
    SimpleRendezvousServer* rendezvous_server_;

    // Create member variables for connectivity
    Domain domain_;
    TransportLayer transport_layer_;
    Protocol protocol_;
  };
}

/**
 * @test    RS start and stop test (basic functionality)
 **/
TEST_F(SimpleRendezvousServerTest, StartsAndStops) {
  ASSERT_FALSE(rendezvous_server_->ShutDown("Normal termination"));
}


/**
 * @test    Ensure that the RS handles updates and changes in subscriptions
 *          (only over the local network)
 **/
TEST_F(SimpleRendezvousServerTest, UpdatesAndHandlesSubscribers) {
  ASSERT_TRUE(rendezvous_server_->UpdateAddress("tick.cs.yale.edu",
                                                "128.36.232.50"));

  EXPECT_EQ(rendezvous_server_->ChangeSubscription(
              pair<LogicalAddress, unsigned short>("thad.cs.yale.edu",
                                                   GLOB_REGIST_PORT),
              "tic.cs.yale.edu"),
            "");

  EXPECT_EQ(rendezvous_server_->ChangeSubscription(
              pair<LogicalAddress, unsigned short>("thad.cs.yale.edu",
                                                   GLOB_REGIST_PORT),
             "tick.cs.yale.edu"),
            "128.36.232.50");
  EXPECT_NE(rendezvous_server_->subscriptions_[
              "tick.cs.yale.edu"].find(
                pair<LogicalAddress, unsigned short>("thad.cs.yale.edu",
                                                     GLOB_REGIST_PORT)),
            rendezvous_server_->subscriptions_["tick.cs.yale.edu"].end());
  EXPECT_EQ(rendezvous_server_->ChangeSubscription(
              pair<LogicalAddress, unsigned short>("thad.cs.yale.edu",
                                                   GLOB_REGIST_PORT),
              "tick.cs.yale.edu"),
            "128.36.232.50");
  EXPECT_EQ(rendezvous_server_->subscriptions_[
              "tick.cs.yale.edu"].find(
                pair<LogicalAddress, unsigned short>("thad.cs.yale.edu",
                                                     GLOB_REGIST_PORT)),
            rendezvous_server_->subscriptions_["tick.cs.yale.edu"].end());

  ASSERT_FALSE(rendezvous_server_->ShutDown("Normal termination"));
}

/**
 * @test    Ensure that network requests (lookups and updates) are handled
 **/
TEST_F(SimpleRendezvousServerTest, HandlesNetworkRequests) {
  int sender = socket(domain_, transport_layer_, protocol_);
  struct sockaddr_in sending_info;
  sending_info.sin_family = domain_;
  sending_info.sin_addr.s_addr = INADDR_ANY;
  sending_info.sin_port = htons(rand() % 10 + 16002);
  socklen_t size_of_sender = sizeof(sending_info);
  ASSERT_FALSE(bind(sender, reinterpret_cast<struct sockaddr*>(&sending_info),
                    size_of_sender));

  struct sockaddr_in server;
  memset(&server, 0, sizeof(server));
  server.sin_family = domain_;
  server.sin_addr.s_addr = GetCurrentIPAddress();
  server.sin_port = htons(GLOB_REGIST_PORT);
  socklen_t server_size = sizeof(server);

  // Send the registration for, say tick, we should get our IP as a response
  int on;
  setsockopt(sender, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char*>(&on),
             sizeof(on));

  char buffer[4096] = "tick.cs.yale.edu";
#ifdef UDP_APPLICATION
  sendto(sender, buffer, sizeof(buffer), 0,
         reinterpret_cast<struct sockaddr*>(&server),
         server_size);
  recvfrom(sender, buffer, sizeof(buffer), 0,
           reinterpret_cast<struct sockaddr*>(&server),
           &server_size);
#elif TCP_APPLICATION
  fprintf(stderr, "TCP is not yet supported\n");
  ASSERT_TRUE(false);
#endif
  char check[4096];
  snprintf(check, sizeof(check), "%s %d", "tick.cs.yale.edu",
           GetCurrentIPAddress());
  EXPECT_EQ(string(buffer), string(check));
  sleep(1);

  // Send the lookup for, again say tick, it should match current IP Address
  server.sin_port = htons(GLOB_LOOKUP_PORT);
  char lookup_buffer[4096] = "tick.cs.yale.edu";
#ifdef UDP_APPLICATION
  sendto(sender, lookup_buffer, sizeof(lookup_buffer), 0,
         reinterpret_cast<struct sockaddr*>(&server),
         server_size);
  recvfrom(sender, lookup_buffer, sizeof(lookup_buffer), 0,
           reinterpret_cast<struct sockaddr*>(&server),
           &server_size);
#elif TCP_APPLICATION
  fprintf(stderr, "TCP is not yet supported\n");
  ASSERT_TRUE(false);
#endif
  EXPECT_EQ(string(lookup_buffer), IntToIPString(GetCurrentIPAddress()));

  EXPECT_NE(rendezvous_server_->subscriptions_[
              "tick.cs.yale.edu"].find(
                pair<LogicalAddress, unsigned short>(
                  IntToIPName(GetCurrentIPAddress()), sending_info.sin_port)),
            rendezvous_server_->subscriptions_[
              "tick.cs.yale.edu"].end());

  // Send the lookup a final time to unsubscribe
  strncpy(lookup_buffer, "tick.cs.yale.edu", sizeof(lookup_buffer));
#ifdef UDP_APPLICATION
  sendto(sender, lookup_buffer, sizeof(lookup_buffer), 0,
         reinterpret_cast<struct sockaddr*>(&server),
         server_size);
  recvfrom(sender, lookup_buffer, sizeof(lookup_buffer), 0,
           reinterpret_cast<struct sockaddr*>(&server),
           &server_size);
#elif TCP_APPLICATION
  fprintf(stderr, "TCP is not yet supported\n");
  ASSERT_TRUE(false);
#endif
  EXPECT_EQ(string(lookup_buffer), IntToIPString(GetCurrentIPAddress()));
  EXPECT_EQ(rendezvous_server_->subscriptions_[
              "tick.cs.yale.edu"].find(
                pair<LogicalAddress, unsigned short>(
                  IntToIPName(GetCurrentIPAddress()), sending_info.sin_port)),
            rendezvous_server_->subscriptions_[
              "tick.cs.yale.edu"].end());

  /** @todo  Need to check that updating peers works when the location moves **/
  fprintf(stderr, "We still don't support updating peers...\n");

  ASSERT_FALSE(rendezvous_server_->ShutDown("Normal termination"));
}

int main(int argc, char* argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
