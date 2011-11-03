OBJDIRS += Applications

# relative to $(TOP), i.e. Applications/*.cc
APPLICATIONS_SRCS := Applications/EchoApp.cc
APPLICATIONS := Deployment/RunApp.cc

# Build files only if they exist.
APPLICATIONS_SRCS := $(wildcard $(APPLICATIONS_SRCS))
APPLICATIONS_OBJS := $(patsubst %.cc, $(OBJDIR)/%.o, $(APPLICATIONS_SRCS))
APPLICATIONS_DEPLOY := $(patsubst %.cc, $(OBJDIR)/%, $(APPLICATIONS))

APPLICATIONS_TEST_SRCS := $(wildcard Tests/Applications/*Test.cc)
APPLICATIONS_TEST_OBJS := $(patsubst %.cc, $(OBJDIR)/%Test.o, $(APPLICATIONS_SRCS))
APPLICATIONS_TESTS     := $(patsubst %.cc, $(OBJDIR)/%Test, $(APPLICATIONS_SRCS))

TESTS += test-Applications

all: Applications-all

Applications-all: Applications Applications-tests
Applications: $(APPLICATIONS_OBJS) $(APPLICATIONS_DEPLOY)
Applications-tests: $(APPLICATIONS_TESTS)

$(OBJDIR)/Applications/%.o: Applications/%.cc
	@echo + cxx $<
	@mkdir -p $(@D)
	$(V)$(CXX) $(CXXFLAGS) -o $@ -c $<
	
$(OBJDIR)/Applications/%Test.o: Tests/Applications/%Test.cc
	@echo + cxx $<
	@mkdir -p $(@D)
	@mkdir -p $(patsubst $(OBJDIR)%, $(LOGDIR)%, $(@D))
	$(V)$(CXX) $(CXXFLAGS) -o $@ -c $<

$(OBJDIR)/Applications/%Test: $(OBJDIR)/Applications/%Test.o $(APPLICATIONS_OBJS) $(MOBILENODE_OBJS)
	@echo + ld $@
	$(V)$(CXX) -o $@ $^ $(LDFLAGS)

$(APPLICATIONS_DEPLOY): $(APPLICATIONS) $(APPLICATIONS_OBJS) $(MOBILENODE_OBJS)
	@echo + ld $@
	@mkdir -p $(@D)
	$(V)$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS)

test-Applications: $(APPLICATIONS_TESTS)
	@for a in $(APPLICATIONS_TESTS); do \
		echo == $$a ==; \
		LD_LIBRARY_PATH=$(GTEST)/lib/.libs $$a 2>$(LOGDIR)$${a#$(OBJDIR)}; \
	done

.PHONY: Applications-all Applications Applications-tests test-Applications