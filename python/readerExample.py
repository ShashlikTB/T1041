# Run reader/RECO exampl
# this is just a wrapper for the ROOT C++ code
# will replace w/ a simple GUI
# Usage: python readerExample.py [file.root]
# Created 4/20/2014 B.Hirosky: Initial release

import os, re, glob, sys, array
from ROOT import *
from TBUtils import *

logger=Logger()

if len(sys.argv)<2:
    runDat="latest.root"
else: runDat=sys.argv[1]

print "Processing file:",runDat

LoadLibs("TBLIB","PadeChannel.so","TBEvent.so","TBReco.so")


gROOT.ProcessLine(".L rootscript/readerExample.C+")

readerExample(runDat)


hit_continue('Hit any key to exit')












