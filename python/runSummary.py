#!/usr/bin/env python
#-----------------------------------------------------------------------------
# File:        runSummary.py
# Description: TB 2014 Run Summary Generator
# Created:     09-May-2014 Sam Bein

#-----------------------------------------------------------------------------
import sys, os, re
from ROOT import *
from time import ctime, sleep
from string import lower, replace, strip, split, joinfields, find
from glob import glob
from array import array
from gui.utils import *
from gui.TBFileReader import TBFileReader
from gui.TBWaveFormPlots import TracePlot, SurfacePlot
from gui.TBShashlikFaces import ShashlikHeatmap
from gui.TDCtiming import TDCtiming
from gui.TBFiberADC import FiberADC
from gui.TBDisplay3D import Display3D
from gui.TBPedestalNoise import PedestalNoise
#------------------------------------------------------------------------------
def getTree(filename):
    hfile = TFile(filename)
    if not hfile.IsOpen():
        print "** can't open file %s" % filename
        sys.exit()
    treename = 't1041'
    tree = hfile.Get(treename)
    if tree == None:
        print "** can't find tree %s" % treename
        sys.exit()
    return (hfile, tree)


def main():
    if os.path.exists("doc/etc"):
        os.system('rm -rf doc/etc')
    os.system('mkdir doc/etc')

    try:
        filename = sys.argv[1]
    except:
        print "please specify input file as arg 1, and optionally, nevents as arg 2"

    try:
        nevents = int(sys.argv[2])
    except:
        nevents = 999999999
        print "SUGGESTION: to speed things up, specify the number of events as arg2 (100)"

    tfile, ttree = getTree(filename)
    event = TBEvent()
    spill = TBSpill()
    util = Util()

    ttree.SetBranchAddress("tbevent", AddressOf(event));
    ttree.SetBranchAddress("tbspill", AddressOf(spill));



    util.accumulate = True
    util.stealthmode = True
    util.WC_showQhits = True
    util.WC_showIThits = True
    gROOT.SetBatch(1)

    cADC = TCanvas("ADC","ADC",1000,500)
    adcHeatmap = ShashlikHeatmap(cADC)

    cWC = TCanvas("WC","WC",1000,500)
    wcHits = WCPlanes(cWC)

    cPulse = TCanvas("Pulse","Pulse",1000,500)
    pulseHealth = FiberADC(cPulse)

    cNoise = TCanvas("Noise","Noise",1000,250)
    pedNoise = PedestalNoise(cNoise)

    cTDC = TCanvas("TDC","TDC",1000,500)
    tdcTiming = TDCtiming(cTDC)

    nevents = min(nevents, ttree.GetEntries())
    for ientry in range(nevents-1):#ttree.GetEntries()-1):
        ttree.GetEntry(ientry)
        if ientry%5==0:
            print "processing entry", ientry,"/",nevents
        adcHeatmap.Draw(event, util)
        wcHits.Draw(event, util)
        pulseHealth.Draw(event,util)
        pedNoise.Draw(event,util)


    util.stealthmode = False
    ttree.GetEntry(nevents)  
    cADC.cd()    
    adcHeatmap.Draw(event, util)
    adcHeatmap.hMapFront.Scale(1.0/50.0)
    adcHeatmap.hMapBack.Scale(1.0/50.0)
    cADC.Update()
    cADC.Print("doc/etc/heatmap.pdf")

    wcHits.Draw(event, util)
    cWC.cd(1)
    cWC.Update()
    cWC.cd(2)
    cWC.Update()
    cWC.Print("doc/etc/wc.pdf")


    pulseHealth.Draw(event, util)
    cPulse.Update()
    cPulse.Print("doc/etc/pulse.pdf")

    LabelSize = .055
    pedNoise.Draw(event, util)
    pedNoise.hMapPedSigVsFiber.GetXaxis().SetTitleSize(1.4*LabelSize)
    pedNoise.hMapPedSigVsFiber.GetXaxis().SetLabelSize(1.2*LabelSize)
    pedNoise.hMapPedSigVsFiber.GetYaxis().SetLabelSize(1.06*LabelSize)
    pedNoise.hMapPedSigVsFiber.GetZaxis().SetLabelSize(0.7*LabelSize)
    cNoise.Update()
    cNoise.Print("doc/etc/noise.pdf")

    tdcTiming.Draw(event, util)
    cTDC.Update()
    cTDC.Print("doc/etc/tdc.pdf")
    gROOT.SetBatch(0)

    summaryTemplate = open('doc/runSummaryTemplate.tex')
    lines = summaryTemplate.readlines()
    summaryTemplate.close()
    summaryFile = open('doc/etc/summary_'+filename[filename.rfind('/')+1:].replace('.root','.tex'),"w")
    for line in lines:
        line = line.replace("ENERGY","32 GeV").replace("SPECIES","Electrons")
        line = line.replace("NEVENTS",str(nevents)+'/'+str(ttree.GetEntries()))
        fname = filename[filename.rfind("/")+1:].replace("_","\\_")
        line = line.replace("FILENAME",fname)
        summaryFile.write(line)
    summaryFile.close()

    os.system('pdflatex -output-directory=doc/etc doc/etc/summary_'+filename[filename.rfind('/')+1:].replace('.root','.tex')+" > doc/etc/texlog.txt")
    os.system('mv doc/etc/summary*.pdf data')
    #os.system('open data/summary_'+filename[filename.rfind('/')+1:].replace('.root','.pdf'))





if __name__ == "__main__":
    main()
    sys.exit()
