OBJDIRS += DNS

# relative to $(TOP), i.e. DNS/*.cc
DNS_SRCS := DNS/SimpleDNS.cc
DNS :=

# Build files only if they exist.
DNS_SRCS := $(wildcard $(DNS_SRCS))
DNS_OBJS := $(patsubst %.cc, $(OBJDIR)/%.o, $(DNS_SRCS))
DNS_DEPLOY := $(patsubst %.cc, $(OBJDIR)/%, $(DNS))

DNS_TEST_SRCS := $(wildcard Tests/DNS/*Test.cc)
DNS_TEST_OBJS := $(patsubst %.cc, $(OBJDIR)/%Test.o, $(DNS_SRCS))
DNS_TESTS     := $(patsubst %.cc, $(OBJDIR)/%Test, $(DNS_SRCS))

TESTS += test-DNS

all: DNS-all

DNS-all: DNS DNS-tests
DNS: $(DNS_OBJS) $(DNS_DEPLOY)
DNS-tests: $(DNS_TESTS)

$(OBJDIR)/DNS/%.o: DNS/%.cc
	@echo + cxx $<
	@mkdir -p $(@D)
	$(V)$(CXX) $(CXXFLAGS) -o $@ -c $<
	
$(OBJDIR)/DNS/%Test.o: Tests/DNS/%Test.cc
	@echo + cxx $<
	@mkdir -p $(@D)
	@mkdir -p $(patsubst $(OBJDIR)%, $(LOGDIR)%, $(@D))
	$(V)$(CXX) $(CXXFLAGS) -o $@ -c $<
	
$(OBJDIR)/DNS/%Test: $(OBJDIR)/DNS/%Test.o $(DNS_OBJS)
	@echo + ld $@
	$(V)$(CXX) -o $@ $^ $(LDFLAGS)

$(DNS_DEPLOY): $(DNS) $(DNS_OBJS)
	@echo + ld $@
	@mkdir -p $(@D)
	$(V)$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS)

test-DNS: $(DNS_TESTS)
	@for a in $(DNS_TESTS); do \
		echo == $$a ==; \
		LD_LIBRARY_PATH=$(GTEST)/lib/.libs $$a 2>$(LOGDIR)$${a#$(OBJDIR)}; \
	done

.PHONY: DNS-all DNS DNS-tests test-DNS