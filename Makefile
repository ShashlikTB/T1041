## Right now this Makefile is really brittle, if it fails during a build then you need to make clean; and restart 

SHELL=/bin/sh
.SUFFIXES:
.SUFFIXES: .cc .cxx .o
VPATH=src:include:build:build/lib:../src:/include:../include:lib
CC = g++
ROOTLIB=$(shell root-config --libdir)
ROOTCFLAGS=$(shell root-config --cflags)
ROOTLFLAGS=$(shell root-config --ldflags)
ROOTLIBS=$(shell root-config --libs) -lSpectrum -lGui
DEBUG=-ggdb
PROFILE=-pg
CXXFLAGS=-Wall -fPIC -O2 $(DEBUG) -rdynamic 
SHAREDCFLAGS = -shared $(ROOTCFLAGS) $(CXXFLAGS)
NONSHARED = -c -pipe -Wshadow -W -Woverloaded-virtual $(ROOTCFLAGS) $(CXXFLAGS) -DR__HAVE_CONFIG
BUILDDIR = $(CURDIR)/build
LIB = $(BUILDDIR)/lib

all: dirs waveInterface.so TBEvent.so TBReco.so

dirs:
	test -d $(LIB) || mkdir -p $(LIB)

waveInterface.so: waveEventDict.so TBEvent.so waveInterface.o TBEventDict.so
	g++ -shared -Wl,-soname,waveInterface.so $(ROOTLFLAGS) -Wl,--no-undefined -Wl,--as-needed -L$(ROOTLIB) $(ROOTLIBS) $(filter-out %.so, $^) $(abspath $(patsubst %.so, $(LIB)/%.so,$(filter %.so, $(notdir $^)))) $(ROOTLIBS) -o waveInterface.so
	mv $@ $(LIB)

waveEventDict.so: waveEventDict.cxx 
	$(CC) $(SHAREDCFLAGS) -I. $(BUILDDIR)/$^ -o $@
	mv $@ $(LIB)/

waveEventDict.cxx: waveInterface.h waveLinkDef.h
	rm -f ./waveEventDict.cxx ./waveEventDict.h
	rootcint $@ -c $^
	mv waveEventDict.[ch]* $(BUILDDIR)

waveInterface.o: waveInterface.cc
	$(CC) -I$(<D)/../include $(NONSHARED)  -Wno-sign-compare $^


TBEvent.so: shashlik.o TBEvent.o TBEventDict.so
	$(CC) $(SHAREDCFLAGS) -o $@ $(abspath $(patsubst %.so, $(LIB)/%.so,$^)) $(ROOTLIBS)
	mv $@ $(LIB)/	

TBEvent.o: TBEvent.cc 
	$(CC) -I$(<D)/../include $(NONSHARED) $^

TBEventDict.so: TBEventDict.cxx
	$(CC) $(SHAREDCFLAGS) -I. $(BUILDDIR)/$^ -o $@ 
	mv $@ $(LIB)/

TBEventDict.cxx: TBEvent.h LinkDef.h
	rm -f ./TBEventDict.cxx ./TBEventDict.h
	rootcint $@ -c $^ 
	mv TBEventDict.[ch]* $(BUILDDIR)

TBReco.so: TBReco.o
	$(CC) $(SHAREDCFLAGS) -o $@ $(abspath $(patsubst %.so, $(LIB)/%.so,$^)) $(ROOTLIBS)
	mv $@ $(LIB)/

TBReco.o: TBReco.cc TBReco.h
	$(CC) -I$(<D)/../include $(NONSHARED) $^

shashlik.o: shashlik.cc 
	$(CC) -I$(<D)/../include $(NONSHARED) $^

clean:
	rm  -f *.so *.o *~ TBEventDict.cxx TBEventDict.h waveEventDict.cxx waveEventDict.h 
	rm -rf $(BUILDDIR) $(LIB)

cleanall:  clean
	rm -f */*so *root */*.d */*~
