# very rudimentary code to display wave forms 
# Created 4/13/2014 B.Hirosky: Initial release

import os, re, glob, sys, array
from ROOT import *
from TBUtils import *

if len(sys.argv)<2:
    runDat="latest.root"
else: runDat=sys.argv[1]


print "Processing file:",runDat
LoadLibs("TBLIB","TBEvent.so")
gROOT.ProcessLine(".L rootscript/waveViewer.C+")

print "To kill use: kill",os.getpid()

# show all wave forms in file
waveViewer(runDat)

# restrict display to a specific board/channel
#waveViewer(runDat,115,10)






