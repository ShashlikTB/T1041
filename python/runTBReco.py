# Run RECO tools
# this is just a wrapper for the ROOT C++ code
# Usage: python runTBReco.py [file.root]
# Created 4/20/2014 B.Hirosky: Initial release

import sys
from ROOT import *
from TBUtils import *


if len(sys.argv)<2:
    runDat="latest.root"
else: runDat=sys.argv[1]
outdir=""
if len(sys.argv)>2:
    outdir=sys.argv[2]

print "Processing file:",runDat

LoadLibs("TBLIB","libTB.so")

gROOT.ProcessLine(".L rootscript/runTBReco.C+")
runTBReco(runDat,"",outdir)

hit_continue('Hit any key to exit')












