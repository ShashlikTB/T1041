# Run RECO tools
# this is just a wrapper for the ROOT C++ code
# Usage: python runTBReco.py [-o output directory] input_file.root 
# Created 4/20/2014 B.Hirosky: Initial release

import sys, os, getopt, glob, commands
from ROOT import *
from TBUtils import *

def usage():
    print
    print "Usage: python runTBReco.py [OPTION] input_path[=latest.root]"
    print "       -r             : Recursively process input_path"
    print "       -o DIR         : Output dir, instead of default = location of input file" 
    print "       -n number      : max # of events to RECO"
    print 
    sys.exit()


### main ###

try:
    opts, args = getopt.getopt(sys.argv[1:], "ro:n:")
except getopt.GetoptError as err: usage()


outDir=""
recurse=False
for o, a in opts:
    if o == "-r":
        recurse=True
    elif o == "-o": 
        outDir=a
        print "Sending output to directory",outDir
    elif o == "-n": 
        nMax=a
        print "Process only up to",nMax,"events"


if len(args)<1:
    runDat="latest.root"
else: runDat=args[0]
if not os.path.isdir(runDat):
    recurse=False

print "Processing file:",runDat

LoadLibs("TBLIB","libTB.so")
gSystem.SetIncludePath("-I\"$TBHOME/include\"")



gROOT.ProcessLine(".L rootscript/runTBReco.C+")

fileList=[]
if recurse:
    fileList.extend(glob.glob(runDat+'/rec_capture_*[0-9]*root'))
else:
    fileList.extend(glob.glob(runDat))
    
for file in fileList:
    outFile=runTBReco(file,"",outDir)    
    print "finished",outFile
    # for convinence when working interactively
    commands.getoutput(ccat('ln -sf',outFile,' latest_reco.root'))

#hit_continue('Hit any key to exit')












