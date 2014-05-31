# very rudimentary code to display wave forms 
# Created 4/13/2014 B.Hirosky: Initial release

import os, re, glob, sys, array
from ROOT import *
from TBUtils import *

if len(sys.argv)<2:
    runDat="latest.root"
else: runDat=sys.argv[1]


print "Processing file:",runDat
LoadLibs("TBLIB","TBEvent.so","waveInterface.so")

gROOT.ProcessLine(".L rootscript/waveViewer.C+")


# show all wave forms in file
wi=waveInterface(runDat)


hit_continue('Hit any key to exit')





