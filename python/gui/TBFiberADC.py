import os, sys, glob, random
from ROOT import *
from time import sleep
from TBUtils import *
import gui.utils


class FiberADC:
	def __init__(self, canvas):
		self.canvas = canvas
	def __del__(self):
		pass

	def Draw(self, event, util):
		MakePlots(self, self.canvas, event, util)

        
		
def MakePlots(object, c1, event, util):
    try:
        o = object.hMapADCvsFiber
        p = object.hMapCHI2vsFiber

    except:

        c1.Divide(1,2)
        gStyle.SetPalette(1)
        gStyle.SetOptStat(0)

        object.hMapADCvsFiber = TH2F('hMapADCfiber','ADC Samples Vs. Fiber', 128,0,128,3000,0,3000)
        object.hMapCHI2vsFiber = TH2F('hMapADCvsFiber','Chi2 Vs. Fiber', 128,0,128,25,0,10000)
        HistoSamStyle(object.hMapADCvsFiber)
        HistoSamStyle(object.hMapCHI2vsFiber)

    hMapADCvsFiber = object.hMapADCvsFiber
    hMapCHI2vsFiber = object.hMapCHI2vsFiber

    if not util.accumulate:
        hMapADCvsFiber.Reset()
        hMapCHI2vsFiber.Reset()


    hit = TBRecHit()
    nSigmaCut=10
    for i in range(0,128):
            pade = event.GetPadeChan(i)
            hit.Init(pade, nSigmaCut)
            for iwf in range(0,60):
                    number = pade.GetWform()[iwf]
                    hMapADCvsFiber.Fill(i,number)
            if not (hit.Status() and TBRecHit.kZSP):
                chi2= min(9999,hit.Chi2())
                hMapCHI2vsFiber.Fill(i,chi2)
        

    if not util.stealthmode:
        gStyle.SetTitleSize(.08,"t"); 
        c1.cd(1)
        c1.SetLogz()
        hMapADCvsFiber.Draw('COLZ')
        c1.cd(2)
        hMapCHI2vsFiber.Draw('COLZ')
        c1.Update()

    
    
            
        
                
        
def HistoSamStyle(histo):
    LabelSize = 0.055
    lwidth = 2
    histo.GetXaxis().SetTitle('channel    \t\t  ')
    histo.GetXaxis().SetTitleSize(1.4*LabelSize)
    histo.GetXaxis().SetTitleOffset(0.45)
    histo.GetXaxis().SetLabelSize(1.2*LabelSize)
    histo.GetYaxis().SetLabelSize(1.06*LabelSize)
    histo.GetYaxis().SetTitleOffset(1)
    histo.GetZaxis().SetLabelSize(0.7*LabelSize)

def getWFmax(pade):
	max = 0
	wform = pade.GetWform()
	for iwf in range(0,PadeChannel().N_PADE_SAMPLES): 
		number = pade.GetWform()[iwf]
		if number > max:
			max = number
	return max

