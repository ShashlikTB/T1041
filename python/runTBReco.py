# Run RECO tools
# this is just a wrapper for the ROOT C++ code
# Usage: python runTBReco.py [-o output directory] input_file.root 
# Created 4/20/2014 B.Hirosky: Initial release

import sys, getopt
from ROOT import *
from TBUtils import *

def usage():
    print
    print "Usage: python runTBReco.py [OPTION] input_file[=latest.root]"
    print "       -o DIR         : Output dir, instead of default = location of input file" 
    print 
    sys.exit()


### main ###

try:
    opts, args = getopt.getopt(sys.argv[1:], "o:")
except getopt.GetoptError as err: usage()


outDir=""
for o, a in opts:
    if o == "-o": 
        outDir=a
        print "Sending output to directory",outDir


if len(args)<2:
    runDat="latest.root"
else: runDat=sys.argv[1]


print "Processing file:",runDat

LoadLibs("TBLIB","libTB.so")



gROOT.ProcessLine(".L rootscript/runTBReco.C+")
runTBReco(runDat,"",outDir)

#hit_continue('Hit any key to exit')












