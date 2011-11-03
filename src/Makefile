#
# This makefile system follows the structuring conventions
# recommended by Peter Miller in his excellent paper:
#
#       Recursive Make Considered Harmful
#       http://aegis.sourceforge.net/auug97.pdf
#
# Later pilfered from Alex Thomson's Deterministic DBMS project

OBJDIR := ../build
LOGDIR := ../logs
TOP = .
GTEST = ../ext/googletest

# '$(V)' controls whether the lab makefiles print verbose commands (the
# actual shell commands run by Make), as well as the "overview" commands
# (such as '+ cc lib/readline.c').
#
# For overview commands only, the line should read 'V = @'.
# For overview and verbose commands, the line should read 'V ='.
V = @

CXX    := g++

CXXFLAGS := -g -I$(TOP) -I$(OBJDIR) -I$(GTEST)/include -MD -Wall -Werror
LDFLAGS := -lpthread -lgtest -L$(GTEST)/lib/.libs

# Lists that the */Makefrag makefile fragments will add to
OBJDIRS := Deployment
TESTS :=

# Make sure that 'all' is the first target
all:

# Makefile fragments for library code
include DNS/Makefile.inc
include RendezvousServer/Makefile.inc
include MobileNode/Makefile.inc
include Applications/Makefile.inc

test: $(TESTS)

clean:
	rm -rf $(OBJDIR) $(LOGDIR)
	
# This magic automatically generates makefile dependencies
# for header files included from C source files we compile,
# and keeps those dependencies up-to-date every time we recompile.
# See 'mergedep.pl' for more information.
$(OBJDIR)/.deps: $(foreach dir, $(OBJDIRS), $(wildcard $(OBJDIR)/$(dir)/*.d))
	@mkdir -p $(@D)
	@cat $^ /dev/null > $@
#	@$(PERL) mergedep.pl $@ $^
-include $(OBJDIR)/.deps

always:
	@:

# Eliminate default suffix rules
.SUFFIXES:

# make it so that no intermediate .o files are ever deleted
.PRECIOUS: $(foreach dir, $(OBJDIRS), $(OBJDIR)/$(dir)/%.o) \
           $(foreach dir, $(OBJDIRS), $(OBJDIR)/$(dir)/%Test.o) \
           $(foreach dir, $(OBJDIRS), $(OBJDIR)/$(dir)/%.pb.cc) \
           $(foreach dir, $(OBJDIRS), $(OBJDIR)/$(dir)/%.pb.h)

.PHONY: all always clean test
