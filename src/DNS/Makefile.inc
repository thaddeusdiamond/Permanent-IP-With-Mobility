# Modify global directives in global namespace
OBJDIRS += DNS
TESTS += DNSTest

# relative to $(TOP), i.e. DNS/*.cc
DNS := Deployment/RunDNS.cc
DNS_SRCS := DNS/SimpleDNS.cc

# Set makefile template specific vars
UPPERC_DIR = DNS
LOWERC_DIR = DNS
EXECUTABLE = RunDNS
EXECUTABLE_OBJS = $(DNS)
EXTRA_TEST_OBJS =

include $(MAKEFILE_TEMPLATE)

# Directive to make the test case
DNSTest: $(DNS_TESTS)
	@for a in $(DNS_TESTS); do \
		echo == $$a ==; \
		$(LDLIBPATH) $$a 2>$(LOGDIR)$${a#$(BINDIR)}; \
	done
