import os, re, glob, sys, array
from ROOT import *
from TBUtils import *

logger=Logger()

if len(sys.argv)<2:
    logger.Fatal("No ROOT file given")


runDat=sys.argv[1]

print "Processing file:",runDat

gROOT.ProcessLine(".L TBEvent.cc+")
gROOT.ProcessLine(".L dqm/dqmPlots.C+")

dqmDisplay(runDat)
displayAllBigPeaks(runDat)

hit_continue('Hit any key to exit')
