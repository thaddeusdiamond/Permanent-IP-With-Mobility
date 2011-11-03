OBJDIRS += MobileNode

# relative to $(TOP), i.e. MobileNode/*.cc
MOBILENODE_SRCS := MobileNode/SimpleMobileNode.cc
MOBILENODE :=

# Build files only if they exist.
MOBILENODE_SRCS := $(wildcard $(MOBILENODE_SRCS))
MOBILENODE_OBJS := $(patsubst %.cc, $(OBJDIR)/%.o, $(MOBILENODE_SRCS))
MOBILENODE_DEPLOY := $(patsubst %.cc, $(OBJDIR)/%, $(MOBILENODE))

MOBILENODE_TEST_SRCS := $(wildcard Tests/MobileNode/*Test.cc)
MOBILENODE_TEST_OBJS := $(patsubst %.cc, $(OBJDIR)/%Test.o, $(MOBILENODE_SRCS))
MOBILENODE_TESTS     := $(patsubst %.cc, $(OBJDIR)/%Test, $(MOBILENODE_SRCS))

TESTS += test-MobileNode

all: MobileNode-all

MobileNode-all: MobileNode MobileNode-tests
MobileNode: $(MOBILENODE_OBJS) $(MOBILENODE_DEPLOY)
MobileNode-tests: $(MOBILENODE_TESTS)

$(OBJDIR)/MobileNode/%.o: MobileNode/%.cc
	@echo + cxx $<
	@mkdir -p $(@D)
	$(V)$(CXX) $(CXXFLAGS) -o $@ -c $<
	
$(OBJDIR)/MobileNode/%Test.o: Tests/MobileNode/%Test.cc
	@echo + cxx $<
	@mkdir -p $(@D)
	@mkdir -p $(patsubst $(OBJDIR)%, $(LOGDIR)%, $(@D))
	$(V)$(CXX) $(CXXFLAGS) -o $@ -c $<

$(OBJDIR)/MobileNode/%Test: $(OBJDIR)/MobileNode/%Test.o $(MOBILENODE_OBJS)
	@echo + ld $@
	$(V)$(CXX) -o $@ $^ $(LDFLAGS)

$(MOBILENODE_DEPLOY): $(MOBILE_NODE) $(MOBILENODE_OBJS)
	@echo + ld $@
	@mkdir -p $(@D)
	$(V)$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS)
	
test-MobileNode: $(MOBILENODE_TESTS)
	@for a in $(MOBILENODE_TESTS); do \
		echo == $$a ==; \
		LD_LIBRARY_PATH=$(GTEST)/lib/.libs $$a 2>$(LOGDIR)$${a#$(OBJDIR)}; \
	done

.PHONY: MobileNode-all MobileNode MobileNode-tests test-MobileNode
