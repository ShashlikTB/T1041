ROOTLIB=$(shell root-config --libdir)
ROOTCFLAGS=$(shell root-config --cflags)
ROOTLFLAGS=$(shell root-config --ldflags)
ROOTLIBS=$(shell root-config --libs) -lSpectrum -lGui
INC=../include
DEBUG=
PROFILE=-pg
CXXFLAGS=-Wall -fPIC -O2 $(DEBUG) -rdynamic 

TBEvent.so: TBEvent.o TBEventDict.so
	g++ -shared -o TBEvent.so $(ROOTCFLAGS) $(CXXFLAGS) TBEvent.o ./TBEventDict.so $(ROOTLIBS)

TBEvent.o: TBEvent.cc
	g++ -c -pipe -Wshadow -W -Woverloaded-virtual $(ROOTCFLAGS) $(CXXFLAGS) -DR__HAVE_CONFIG TBEvent.cc

TBEventDict.so: TBEventDict.cxx
	g++ -shared -o TBEventDict.so $(ROOTCFLAGS) $(CXXFLAGS) TBEventDict.cxx

TBEventDict.cxx: TBEvent.h LinkDef.h
	rootcint TBEventDict.cxx -c TBEvent.h LinkDef.h

clean:
	rm  -f *.so *.o *~ TBEventDict.cxx TBEventDict.h
