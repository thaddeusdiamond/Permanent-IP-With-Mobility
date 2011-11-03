OBJDIRS += RendezvousServer

# relative to $(TOP), i.e. RendezvousServer/*.cc
RENDEZVOUSSERVER_SRCS := RendezvousServer/SimpleRendezvousServer.cc
RENDEZVOUSSERVER :=

# Build files only if they exist.
RENDEZVOUSSERVER_SRCS := $(wildcard $(RENDEZVOUSSERVER_SRCS))
RENDEZVOUSSERVER_OBJS := $(patsubst %.cc, $(OBJDIR)/%.o, $(RENDEZVOUSSERVER_SRCS))
RENDEZVOUSSERVER_DEPLOY := $(patsubst %.cc, $(OBJDIR)/%, $(RENDEZVOUSSERVER))

RENDEZVOUSSERVER_TEST_SRCS := $(wildcard Tests/RendezvousServer/*Test.cc)
RENDEZVOUSSERVER_TEST_OBJS := $(patsubst %.cc, $(OBJDIR)/%Test.o, $(RENDEZVOUSSERVER_SRCS))
RENDEZVOUSSERVER_TESTS     := $(patsubst %.cc, $(OBJDIR)/%Test, $(RENDEZVOUSSERVER_SRCS))

TESTS += test-RendezvousServer

all: RendezvousServer-all

RendezvousServer-all: RendezvousServer RendezvousServer-tests
RendezvousServer: $(RENDEZVOUSSERVER_OBJS) $(RENDEZVOUSSERVER_DEPLOY)
RendezvousServer-tests: $(RENDEZVOUSSERVER_TESTS)

$(OBJDIR)/RendezvousServer/%.o: RendezvousServer/%.cc
	@echo + cxx $<
	@mkdir -p $(@D)
	$(V)$(CXX) $(CXXFLAGS) -o $@ -c $<
	
$(OBJDIR)/RendezvousServer/%Test.o: Tests/RendezvousServer/%Test.cc
	@echo + cxx $<
	@mkdir -p $(@D)
	@mkdir -p $(patsubst $(OBJDIR)%, $(LOGDIR)%, $(@D))
	$(V)$(CXX) $(CXXFLAGS) -o $@ -c $<

$(OBJDIR)/RendezvousServer/%Test: $(OBJDIR)/RendezvousServer/%Test.o $(RENDEZVOUSSERVER_OBJS)
	@echo + ld $@
	$(V)$(CXX) -o $@ $^ $(LDFLAGS)

$(RENDEZVOUSSERVER_DEPLOY): $(RENDEZVOUSSERVER) $(RENDEZVOUSSERVER_OBJS)
	@echo + ld $@
	@mkdir -p $(@D)
	$(V)$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS)

test-RendezvousServer: $(RENDEZVOUSSERVER_TESTS)
	@for a in $(RENDEZVOUSSERVER_TESTS); do \
		echo == $$a ==; \
		LD_LIBRARY_PATH=$(GTEST)/lib/.libs $$a 2>$(LOGDIR)$${a#$(OBJDIR)}; \
	done

.PHONY: RendezvousServer-all RendezvousServer RendezvousServer-tests test-RendezvousServer
