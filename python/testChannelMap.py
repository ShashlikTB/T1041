# display channel maps
# this is just a wrapper for the ROOT C++ code

# Created 5/1/2014 B.Hirosky: Initial release

 
from ROOT import *
from TBUtils import *


LoadLibs("TBLIB","TBEvent.so")


gROOT.ProcessLine(".L rootscript/testChannelMap.C+")

testChannelMap()


hit_continue('Hit any key to exit')












