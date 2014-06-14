import os, re, glob, sys, array
from ROOT import *
from TBUtils import *
from time import sleep

baseDir = os.environ['TBHOME']
if baseDir == None:
	baseDir = os.environ('PWD')
baseDir += '/'
print "==> baseDir: %s" % baseDir 


#print "\nProcessing file:",runDat

LoadLibs("TBLIB","TBEvent.so","TBReco.so")

gROOT.ProcessLine(".L WCPlanes.C+")


event = TBEvent()
myfile = TFile(baseDir+'../inputfiles/rec_capture_20140417_005112.root')
tree = myfile.Get('t1041')
bevent = tree.GetBranch('tbevent')
bevent.SetAddress(AddressOf(event))
tree.GetEntry(54)


for i in range(5,25):
    J = TCanvas('c','c',1000,450)
    tree.GetEntry(i)
    WCPlanes(J, event)  
    J.Update() 
    sleep(3)
exit(0)
