# very redimentary code to display wave forms 
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

# show all wave forms in file
# waveViewer(runDat)

# restrict display to a specific board/channel
waveViewer(runDat,115,10)






