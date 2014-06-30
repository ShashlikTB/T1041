SHELL:=/bin/sh
# ----------------------------------------------------------------------------
# Build libTB.so for Test Beam 2014 code
# Created 5 June 2014 HBP
# ----------------------------------------------------------------------------
ifndef ROOTSYS
$(error *** Please set up environment variable ROOTSYS)
endif
# ----------------------------------------------------------------------------
NAME	:= TB
BLDDIR	:= build
LIBDIR	:= build/lib
SRCDIR	:= src
INCDIR	:= include
LIBRARY	:= $(LIBDIR)/lib$(NAME).so
$(shell mkdir -p build/lib)
# ----------------------------------------------------------------------------
# sources for which dictionaries are to be created, 
# but without the use of LinkDefs
SRCSNOLINKDEF	:= WCPlanes.cc Connection.cc Slot.cc Util.cc Dialog.cc\
Mapper.cc

# sources for which dictionaries are to be created, 
# using LinkDefs
SRCSWITHLINKDEF:= TBEvent.cc waveInterface.cc PadeChannel.cc
# ----------------------------------------------------------------------------
SRCSNOLINKDEF	:= $(patsubst %.cc,$(SRCDIR)/%.cc,$(SRCSNOLINKDEF))
DICTSNOLINKDEF	:= $(patsubst $(SRCDIR)/%.cc,$(BLDDIR)/%Dict.cxx,$(SRCSNOLINKDEF))

SRCSWITHLINKDEF	:= $(patsubst %.cc,$(SRCDIR)/%.cc,$(SRCSWITHLINKDEF))
DICTSWITHLINKDEF:= $(patsubst $(SRCDIR)/%.cc,$(BLDDIR)/%Dict.cxx,$(SRCSWITHLINKDEF))

CCSRCS	:= $(SRCSNOLINKDEF)
CCSRCS	+= $(SRCSWITHLINKDEF)
# add other sources
CCSRCS	+= $(filter-out $(CCSRCS),$(wildcard $(SRCDIR)/*.cc))

CXXSRCS	:= $(DICTSNOLINKDEF) $(DICTSWITHLINKDEF) 

# objects
CXXOBJS	:= $(patsubst %.cxx,%.o,$(CXXSRCS))
CCOBJS	:= $(patsubst $(SRCDIR)/%.cc,$(BLDDIR)/%.o,$(CCSRCS))
OBJECTS	:= $(CCOBJS) $(CXXOBJS) 
# ----------------------------------------------------------------------------
CINT	:= rootcint
PYCFLAGS:= $(shell python-config --includes)
CPPFLAGS:= -I. -I$(INCDIR) -DR__HAVE_CONFIG \
$(filter-out -stdlib=libc++,$(shell root-config --cflags)) $(PYCFLAGS)
CXXFLAGS:= -O -Wall -fPIC -g -ansi -Wextra -Wshadow
LDFLAGS	:= -g
# ----------------------------------------------------------------------------
# which operating system?
OS := $(shell uname -s)
ifeq ($(OS),Darwin)
CXX	:= g++
LD	:= g++
LDFLAGS += -dynamiclib
ROOTLIBS:= $(shell root-config --glibs --nonew) -lMathMore
else
CXX	:= g++
LD	:= g++
LDFLAGS	+= -shared -Wl,-soname,lib$(NAME).so
ROOTLIBS:= $(filter-out -stdlib=libc++,$(shell root-config --glibs --nonew))
endif
# ----------------------------------------------------------------------------
# libs
# ----------------------------------------------------------------------------
PYHOME	:= $(shell python-config --prefix)
PYLIB	:= -L$(PYHOME)/lib
PYLIBS	:= $(shell python-config --libs)
LDFLAGS += $(shell root-config --ldflags) $(PYLIB)
LIBS	:= -lMathMore -lSpectrum -lPyROOT $(ROOTLIBS) $(PYLIBS)

ifdef DEBUG
say	:= $(shell echo "CCSRCS:  $(CCSRCS)" >& 2)
say	:= $(shell echo "CCOBJS:  $(CCOBJS)\n" >& 2)

say	:= $(shell echo "CXXSRCS: $(CXXSRCS)" >& 2)
say	:= $(shell echo "CXXOBJS: $(CXXOBJS)\n" >& 2)

say	:= $(shell echo "DICTSNOLINKDEF:   $(DICTSNOLINKDEF)" >& 2)
say	:= $(shell echo "DICTSWITHLINKDEF: $(DICTSWITHLINKDEF)" >& 2)

say	:= $(shell echo "LIBRARY:  $(LIBRARY)\n" >& 2)
say 	:= $(shell echo "CPPFLAGS: $(CPPFLAGS)\n" >& 2)
say 	:= $(shell echo "CXXFLAGS: $(CXXFLAGS)\n" >& 2)
say 	:= $(shell echo "LDFLAGS:  $(LDFLAGS)\n" >& 2)
say 	:= $(shell echo "LIBS:     $(LIBS)\n" >& 2)
$(error bye)
endif

# ----------------------------------------------------------------------------
all: $(LIBRARY)

$(LIBRARY)	: $(OBJECTS)
	@echo "===> Linking shared library $@"
	$(LD) $(LDFLAGS)  $^ $(LIBS) -o $@
	@echo ""

$(CCOBJS)	: $(BLDDIR)/%.o	: 	$(SRCDIR)/%.cc
	@echo "==> Compiling $<"
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@
	@echo ""

$(CXXOBJS)	: %.o	: %.cxx
	@echo "==> Compiling $<"
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@
	@echo ""

$(DICTSNOLINKDEF)	: $(BLDDIR)/%Dict.cxx	: $(INCDIR)/%.h
	@echo "===> Building dictionary for $*"
	@rm -f $(BLDDIR)/$*.[ch]*
	$(CINT)	-f $@ -c $^
	@echo ""

$(DICTSWITHLINKDEF)	: $(BLDDIR)/%Dict.cxx : $(INCDIR)/%.h $(INCDIR)/%LinkDef.h
	@echo "===> Building dictionary for $*"
	@rm -f $(BLDDIR)/$*Dict.[ch]*
	$(CINT) $@ -c $^
	@echo ""

clean:
	rm -rf $(LIBRARY) $(BLDDIR)/*Dict.* $(BLDDIR)/*.o
