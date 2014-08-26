#!/usr/bin/env python
# wrapper script to run wave gui

import sys, signal
from ROOT import *
from TBUtils import *


# Register signal handler to stop cleanly on ctrl-C.
def signal_handler(signal, frame):
    print
    print 'signal_handler: received ctrl-C, exiting...'
    sys.exit(0)

signal.signal(signal.SIGINT,signal_handler)

if len(sys.argv)<2:
    runDat="latest.root"
else: runDat=sys.argv[1]


print "Processing file:",runDat
LoadLibs("TBLIB","libTB.so")
gSystem.SetIncludePath("-I\"$TBHOME/include\"")


# show all wave forms in file
wi=waveInterface(runDat)


hit_continue('Hit any key to exit')





