#!/usr/bin/env python
# Run analyze example
# this is just a wrapper for the ROOT C++ code
# Usage: python runRecoAnalyze.py [file.root]
# Created 7/19/2014 B.Hirosky: Initial release

import sys
from ROOT import *
from TBUtils import *


if len(sys.argv)<2:
    runDat="latest_reco.root"
else: runDat=sys.argv[1]

print "Processing file:",runDat

LoadLibs("TBLIB","libTB.so")
gSystem.SetIncludePath("-I\"$TBHOME/include\"")

gROOT.ProcessLine(".L rootscript/recoAnalyzer.C+")
recoAnalyzer(runDat)

hit_continue('Hit any key to exit')












