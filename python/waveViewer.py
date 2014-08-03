# very rudimentary code to display wave forms 
# Created 4/13/2014 B.Hirosky: Initial release

import sys
from ROOT import *
from TBUtils import *


if len(sys.argv)<2:
    runDat="latest.root"
else: runDat=sys.argv[1]


print "Processing file:",runDat
LoadLibs("TBLIB","libTB.so")
gSystem.SetIncludePath("-I\"$TBHOME/include\"")

gROOT.ProcessLine(".L rootscript/waveViewer.C+")

print "To kill use: control-\ or kill",os.getpid(),";fg"

# show all waveforms in file
waveViewer(runDat)

# restrict display to a specific board/channel
#waveViewer(runDat,115,10)






