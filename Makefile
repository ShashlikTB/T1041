## Right now this Makefile is really brittle, if it fails during a build then you need to make clean; and restart 

SHELL=/bin/sh
.SUFFIXES:
.SUFFIXES: .cc .cxx .o
VPATH=src:include:build:build/lib:../src:/include:../include:lib
CC = g++
ROOTLIB=$(shell root-config --libdir)
ROOTCFLAGS=$(shell root-config --cflags)
ROOTLFLAGS=$(shell root-config --ldflags)
ROOTLIBS=$(shell root-config --libs) -lMathMore -lSpectrum -lGui 
DEBUG=-ggdb
PROFILE=-pg
CXXFLAGS=-Wall -fPIC -O2 $(DEBUG) -rdynamic 
SHAREDCFLAGS = -shared $(ROOTCFLAGS) $(CXXFLAGS)
NONSHARED = -c -pipe -Wshadow -W -Woverloaded-virtual $(ROOTCFLAGS) $(CXXFLAGS) -DR__HAVE_CONFIG
BUILDDIR = $(CURDIR)/build
LIB = $(BUILDDIR)/lib

all: dirs PadeChannel.so TBEvent.so waveInterface.so TBReco.so

dirs:
	test -d $(LIB) || mkdir -p $(LIB)

waveInterface.so: PadeChannel.so PadeChannelDict.so TBEvent.so TBEventDict.so waveEventDict.so  waveInterface.o
	g++ -shared -Wl,-soname,waveInterface.so $(ROOTLFLAGS) -Wl,--no-undefined -Wl,--as-needed -L$(ROOTLIB)  $(filter-out %.so, $^) $(addprefix $(LIB)/,$(filter-out %.o,$(notdir $^)))  $(ROOTLIBS) -o waveInterface.so
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

TBReco.so: TBReco.o
	$(CC) $(SHAREDCFLAGS) -o $@ $(addprefix $(LIB)/,$(filter-out %.o,$(notdir $^))) $(filter-out %.so,$^) $(ROOTLIBS)
	mv $@ $(LIB)/

TBReco.o: TBReco.cc TBReco.h shashlik.h
	$(CC) -I$(<D)/../include $(NONSHARED) $^

TBEvent.so: TBEvent.o TBEventDict.so 
	$(CC) $(SHAREDCFLAGS) -o $@ $(addprefix $(LIB)/,$(filter-out %.o,$(notdir $^))) $(filter-out %.so,$^) $(ROOTLIBS)
	mv $@ $(LIB)/	

TBEventDict.so: TBEventDict.cxx
	$(CC) $(SHAREDCFLAGS) -I. $(addprefix $(BUILDDIR)/,$(notdir $^)) -o $@
	mv $@ $(LIB)/

TBEventDict.cxx: TBEvent.h shashlik.h LinkDef.h
	rm -f ./TBEventDict.cxx ./TBEventDict.h
	rootcint $@ -c $^ 
	mv TBEventDict.[ch]* $(BUILDDIR)

TBEvent.o: TBEvent.cc calConstants.h shashlik.h pulseShapeForFit.h
	$(CC) -I$(<D)/../include $(NONSHARED) $^

PadeChannel.so: shashlik.o PadeChannel.o PadeChannelDict.so
	$(CC) $(SHAREDCFLAGS) -o $@ $(addprefix $(LIB)/,$(filter-out %.o,$(notdir $^))) $(filter-out %.so,$^) $(ROOTLIBS)
	mv $@ $(LIB)/	

PadeChannelDict.so: PadeChannelDict.cxx
	$(CC) $(SHAREDCFLAGS) -I. $(addprefix $(BUILDDIR)/,$(notdir $^)) -o $@
	mv $@ $(LIB)/

PadeChannelDict.cxx: PadeChannel.h LinkDef_PadeChannel.h
	rm -f ./PadeChannelDict.cxx ./PadeChannelDict.h
	rootcint $@ -c $^ 
	mv PadeChannelDict.[ch]* $(BUILDDIR)

PadeChannel.o: PadeChannel.cc calConstants.h shashlik.h pulseShapeForFit.h
	$(CC) -I$(<D)/../include $(NONSHARED) $^

shashlik.o: shashlik.cc shashlik.h 
	$(CC) -I$(<D)/../include $(NONSHARED) $^

clean:
	rm  -f *.so *.o *~ rootscript/*.so rootscript/*.d
	rm -rf $(BUILDDIR) $(LIB)

cleanall:  clean
	rm -f */*so *root */*.d */*~ python/*pyc include/*.gch

