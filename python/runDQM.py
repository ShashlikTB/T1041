#!/usr/bin/env python
# Run basic DQM plotter
# this is just a wrapper for the ROOT C++ code
# Usage: python runRecoAnalyze.py [file.root]
# Created 8/25/2014 B.Hirosky: Initial release

import sys
from ROOT import *
from TBUtils import *


if len(sys.argv)<2:
    runDat="latest_reco.root"
else: runDat=sys.argv[1]

print "Processing file:",runDat

LoadLibs("TBLIB","libTB.so")
gSystem.SetIncludePath("-I\"$TBHOME/include\"")

gROOT.ProcessLine(".L rootscript/DQMplots.C+")
DQMplots(runDat)

hit_continue('Hit any key to exit')












