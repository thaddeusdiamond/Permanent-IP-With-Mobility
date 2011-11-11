# Modify global directives in global namespace
OBJDIRS += Applications
TESTS += ApplicationsTest

# relative to $(TOP), i.e. $(LOWERC_DIR)/*.cc
APPLICATIONS := Deployment/RunApp.cc
APPLICATIONS_SRCS := Applications/EchoApp.cc

# Set makefile template specific vars
UPPERC_DIR = APPLICATIONS
LOWERC_DIR = Applications
EXECUTABLE = RunApp
EXECUTABLE_OBJS = $(APPLICATIONS) $(MOBILENODE_OBJS)
EXTRA_TEST_OBJS = $(MOBILENODE_OBJS)

include $(MAKEFILE_TEMPLATE)

# Directive to make the test case
ApplicationsTest: $(APPLICATIONS_TESTS)
	@for a in $(APPLICATIONS_TESTS); do \
		echo == $$a ==; \
		$(LDLIBPATH) $$a 2>$(LOGDIR)$${a#$(BINDIR)}; \
	done
