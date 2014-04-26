# display integrated hits data for Shashlik calorimeter
# this is just a wrapper for the ROOT C++ code
# will replace w/ a simple GUI

# Created 4/12/2014 B.Hirosky: Initial release

import os, re, glob, sys, array
from ROOT import *
from TBUtils import *

logger=Logger()

if len(sys.argv)<2:
    runDat="latest.root"
else: runDat=sys.argv[1]

print "Processing file:",runDat

LoadLibs("TBLIB","TBEvent.so")


gROOT.ProcessLine(".L rootscript/calDisplay.C+")


calDisplay(runDat)


hit_continue('Hit any key to exit')












