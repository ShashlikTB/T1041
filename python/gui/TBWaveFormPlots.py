#!/usr/bin/env python
#-----------------------------------------------------------------------------
# File:        TBWaveFormPlots.py
# Description: TB 2014 Wave Form plots
# Created:     09-May-2014 Harrison B. Prosper & Sam Bein
#-----------------------------------------------------------------------------
import sys, os, re
from ROOT import *
from string import lower, replace, strip, split, joinfields, find
from array import array
from gui.histutil import mkhist1, mkhist2
#------------------------------------------------------------------------------
# Draw pedistal subtracted wave forms
#------------------------------------------------------------------------------
class TracePlot:

    def __init__(self, canvas):
        self.canvas = canvas
        self.nsamples  = 120
        self.nchannels = 128
        self.step      = 6.5
        self.offset    = self.nchannels*self.step/2

        nsamples  = self.nsamples
        nchannels = self.nchannels

        # cache wave form histograms in parent object
        self.h = [0]*nchannels
        color  = [kBlack, kRed, kGreen+2, kBlue]
        kk = -1
        for ii in xrange(nchannels):
            kk += 1
            if kk > len(color)-1: kk = 0

            self.h[ii] = mkhist1('hwform%3.3d' % ii,
                         'sample number','',
                         nsamples, 0, nsamples)
            self.h[ii].SetMinimum(-500)
            self.h[ii].SetMaximum( 500)
            self.h[ii].SetLineColor(color[kk])
        h = self.h

    def __del__(self):
        pass

    def Draw(self, event):
        #----------------------------------------------------------------------
        # fill
        #----------------------------------------------------------------------
        nsamples  = self.nsamples
        nchannels = self.nchannels
        step      = self.step
        offset    = self.offset
        h         = self.h

        gStyle.SetPalette(1)
        # Margins:
        gStyle.SetPadTopMargin(0.05)
        gStyle.SetPadBottomMargin(0.16)
        gStyle.SetPadLeftMargin(0.20)
        gStyle.SetPadRightMargin(0.16)
        # For the axis titles:
        gStyle.SetTitleXOffset(1.20)
        gStyle.SetTitleYOffset(1.30)
        gStyle.SetOptFile(0)
        gStyle.SetOptStat(0)

        self.canvas.cd()
        option = 'hist'
        for ii in xrange(nchannels):
            channel  = event.GetPadeChan(ii)
            pedestal = channel.GetPedestal()
            wform    = channel.GetWform()
            yoffset  = offset - step * ii
            ymax = 0.0
            for jj in xrange(nsamples):
                ibinx = jj+1				
                y = wform[jj] - pedestal
                h[ii].SetBinContent(ibinx, y - yoffset)
                if y > ymax: ymax = y

            h[ii].Draw(option)
            option = 'hist same'
        self.canvas.Update()

#------------------------------------------------------------------------------
# Plot wave forms as a surface plot
#------------------------------------------------------------------------------
class SurfacePlot:

    def __init__(self, canvas):
        self.canvas = canvas
        self.nsamples  = 120
        self.nchannels = 128
        nsamples  = self.nsamples
        nchannels = self.nchannels

        # cache histogram in parent object
        self.hwform2D = mkhist2('hWFsurface',
                    'sample number','channel number',
                    nsamples, 0, nsamples,
                    nchannels,0, nchannels)		

    def  __del__(self):
        pass

    def Draw(self, event):
        #----------------------------------------------------------------------
        # fill
        #----------------------------------------------------------------------

        nsamples  = self.nsamples
        nchannels = self.nchannels
        h = self.hwform2D

        for ii in xrange(nchannels):
            ibiny = ii+1			
            channel  = event.GetPadeChan(ii)
            pedistal = channel.GetPedistal()
            wform    = channel.GetWform()

            for jj in xrange(nsamples):
                ibinx = jj+1			
                y = wform[jj] - pedistal
                h.SetBinContent(ibinx, ibiny, y)

        gStyle.SetPalette(1)
        # Margins:
        gStyle.SetPadTopMargin(0.05)
        gStyle.SetPadBottomMargin(0.16)
        gStyle.SetPadLeftMargin(0.20)
        gStyle.SetPadRightMargin(0.16)
        # For the axis titles:
        gStyle.SetTitleXOffset(1.25)
        gStyle.SetTitleYOffset(1.35)
        gStyle.SetOptStat(0)

        self.canvas.cd()
        h.Draw('surf2')
        self.canvas.Update()

