# Modify global directives in global namespace
OBJDIRS += MobileNode
TESTS += MobileNodeTest

# relative to $(TOP), i.e. MobileNode/*.cc
MOBILENODE := Deployment/RunMN.cc
MOBILENODE_SRCS := MobileNode/SimpleMobileNode.cc

# Set makefile template specific vars
UPPERC_DIR = MOBILENODE
LOWERC_DIR = MobileNode
EXECUTABLE = RunMN
EXECUTABLE_OBJS = $(MOBILENODE)
EXTRA_TEST_OBJS =

include $(MAKEFILE_TEMPLATE)

# Directive to make the test case
MobileNodeTest: $(MOBILENODE_TESTS)
	@for a in $(MOBILENODE_TESTS); do \
		echo == $$a ==; \
		$(LDLIBPATH) $$a 2>$(LOGDIR)$${a#$(BINDIR)}; \
	done
