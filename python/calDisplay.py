# display integrated hits data for Shashlik calorimeter
# this is just a wrapper for the ROOT C++ code
# will replace w/ a simple GUI

# Created 4/12/2014 B.Hirosky: Initial release

import os, re, glob, sys, array
from time import sleep
from ROOT import *
from TBUtils import *

logger=Logger()

if len(sys.argv)<2:
    runDat="latest.root"
else: runDat=sys.argv[1]
selectEvent=-1
if len(sys.argv)>2: selectEvent=int(sys.argv[2])
play=False
if selectEvent==999: play=True;

print "Processing file:",runDat

LoadLibs("TBLIB","PadeChannel.so","TBEvent.so")


gROOT.ProcessLine(".L rootscript/calDisplay.C+")

calDisplay(runDat,selectEvent)

hit_continue('Hit any key to exit')












