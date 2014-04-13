# display wave forms interactively
# Created 4/13/2014 B.Hirosky: Initial release

import os, re, glob, sys, array
from ROOT import *
from TBUtils import *

runDat=sys.argv[1]
if len(sys.argv)<2:
    logger.Fatal("No ROOT file given")

print "Processing file:",runDat
gROOT.ProcessLine(".L TBEvent.cc+")
gROOT.ProcessLine(".L waveViewer.cc+")

waveViewer(runDat)





