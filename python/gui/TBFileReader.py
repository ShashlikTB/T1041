#!/usr/bin/env python
#-----------------------------------------------------------------------------
# File:        TBFileReader.py
# Description: TB 2014 Event Display
# Created:     09-May-2014 Harrison B. Prosper & Sam Bein
#-----------------------------------------------------------------------------
import sys, os, re, platform
from ROOT import *
from time import ctime, sleep
from string import lower, replace, strip, split, joinfields, find
from array import array
#------------------------------------------------------------------------------
NTDC = 16
gSystem.Load("libTB.so")
#------------------------------------------------------------------------------
class TBFileReader:
    def __init__(self, filename, treename='t1041'):
        print 'initialized TBFileReader'
        if not os.path.exists(filename):
            print "** TBFileReader: can't find file %s" % filename
            sys.exit(0)
        self.f = TFile(filename)
        if not self.f.IsOpen():
            print "** TBFileReader: can't open file %s" % filename
            sys.exit(0)
        self.t = self.f.Get(treename)
        if self.t == None:
            print "** TBFileReader: can't read tree %s" % treename
            sys.exit(0)
        self.nevents = self.t.GetEntries()
        self.e = TBEvent() 
        self.t.SetBranchAddress('tbevent', AddressOf(self.e))
        self.s = TBSpill()
        self.t.SetBranchAddress('tbspill', AddressOf(self.s))
        self.t.GetEntry(0)
        self.tableX = self.s.GetTableX()
        self.tableY = self.s.GetTableY()
        self.s.Dump()
        
    def __del__(self):
        self.f.Close()


    def tree(self):
        return self.t


    def file(self):
        return self.f

    def entries(self):
        return self.nevents

    def read(self, ii):
        if ii < 0:
            return
        if ii >= self.nevents:
            return
        self.t.GetEntry(ii)


    def event(self):
        return self.e


    def maxPadeADC(self):
        nsamples  = PadeChannel().N_PADE_SAMPLES
        nchannels = self.e.NPadeChan()
        ymax = 0
        for ii in xrange(nchannels):
            channel  = self.e.GetPadeChan(ii)
            pedestal = channel.GetPedestal()
            wform    = channel.GetWform()
            for jj in xrange(nsamples):
                y = wform[jj] - pedestal
                if y > ymax: ymax = y
        return ymax
#------------------------------------------------------------------------------
