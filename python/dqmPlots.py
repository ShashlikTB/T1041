import os, re, glob, sys, array
from ROOT import *
from TBUtils import *

logger=Logger()


if len(sys.argv)<2:
    runDat="latest.root"
else: runDat=sys.argv[1]

print "\nProcessing file:",runDat



gROOT.SetBatch()


LoadLibs("TBLIB","PadeChannel.so","TBEvent.so","TBReco.so")


gROOT.ProcessLine(".L dqm/dqmPlots.C+")
gROOT.ProcessLine(".L dqm/WC_Reader.C+")

if len(sys.argv) < 3:
    dqmDisplay(runDat)
    displayAllBigPeaks(runDat)
    WC_Reader(runDat)
else:
    dqmDisplay(runDat, int(sys.argv[2]))
