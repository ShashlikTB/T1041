# display channel maps
# this is just a wrapper for the ROOT C++ code

# Created 5/1/2014 B.Hirosky: Initial release

 
from ROOT import *
from TBUtils import *


LoadLibs("TBLIB","libTB.so")


gROOT.ProcessLine(".L rootscript/testChannelMap.C+")

testChannelMap(635479530091849620)  # October 2014
#testChannelMap(6354216716076907531)  # Summer 2014
#testChannelMap(0) # April 2014

hit_continue('Hit any key to exit')












