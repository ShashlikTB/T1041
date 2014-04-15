import os, re, glob, sys, array
from ROOT import *
from TBUtils import *

logger=Logger()

if len(sys.argv)<2:
    logger.Fatal("No ROOT file given")


runDat = sys.argv[1]

print "\nProcessing file:",runDat

gROOT.SetBatch()
gROOT.ProcessLine(".L TBEvent.cc+")
gROOT.ProcessLine(".L dqm/dqmPlots.C+")
gROOT.ProcessLine(".L WC_Reader.C+")

if len(sys.argv) < 3:
    dqmDisplay(runDat)
    displayAllBigPeaks(runDat)
    WC_Reader(runDat)
else:
    dqmDisplay(runDat, int(sys.argv[2]))
