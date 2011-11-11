# Modify global directives in global namespace
OBJDIRS += RendezvousServer
TESTS += RendezvousServerTest

# relative to $(TOP), i.e. RendezvousServer/*.cc
RENDEZVOUSSERVER := Deployment/RunRS.cc
RENDEZVOUSSERVER_SRCS := RendezvousServer/SimpleRendezvousServer.cc

# Set makefile template specific vars
UPPERC_DIR = RENDEZVOUSSERVER
LOWERC_DIR = RendezvousServer
EXECUTABLE = RunRS
EXECUTABLE_OBJS = $(RENDEZVOUSSERVER)
EXTRA_TEST_OBJS =

include $(MAKEFILE_TEMPLATE)

# Directive to make the test case
RendezvousServerTest: $(RENDEZVOUSSERVER_TESTS)
	@for a in $(RENDEZVOUSSERVER_TESTS); do \
		echo == $$a ==; \
		$(LDLIBPATH) $$a 2>$(LOGDIR)$${a#$(BINDIR)}; \
	done
