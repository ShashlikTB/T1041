# Run reader/RECO example
# this is just a wrapper for the ROOT C++ code
# will replace w/ a simple GUI
# Usage: python readerExample.py [file.root]
# Created 4/20/2014 B.Hirosky: Initial release

import sys
from ROOT import *
from TBUtils import *

logger=Logger()

if len(sys.argv)<2:
    runDat="latest_reco.root"
else: runDat=sys.argv[1]

print "Processing file:",runDat

LoadLibs("TBLIB","libTB.so")
gSystem.SetIncludePath("-I\"$TBHOME/include\"")


gROOT.ProcessLine(".L rootscript/readerExample.C+")

readerExample(runDat)


#hit_continue('Hit any key to exit')












