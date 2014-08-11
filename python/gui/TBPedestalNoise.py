import os, sys, glob, random
from ROOT import *
from time import sleep
from TBUtils import *
import gui.utils


class PedestalNoise:
	def __init__(self, canvas):
		self.canvas = canvas
	def __del__(self):
		pass

	def Draw(self, event, util):
		MakePlots(self, self.canvas, event, util)



def MakePlots(object, c1, event, util):
	try:
		o = object.hMapPedSigVsFiber
	except:
		gStyle.SetPalette(1)
		gStyle.SetOptStat(0)
		object.hMapPedSigVsFiber = TH2F('hMapPedSigVsFiber','Pedestal Std. Dev. VS Channel', 128,0,128,50,0, 10)
		HistoSamStyle(object.hMapPedSigVsFiber)

	hMapPedSigVsFiber = object.hMapPedSigVsFiber

	if not util.accumulate:
		hMapPedSigVsFiber.Reset()


	hit = TBRecHit()
	nSigmaCut=10
	for i in range(0,128):
			pade = event.GetPadeChan(i)
			hit.Init(pade, nSigmaCut)
			for iwf in range(0,60):
				number = pade.GetPedSigma()
				hMapPedSigVsFiber.Fill(i,min(9.9,number))

	if not util.stealthmode:
		gStyle.SetTitleSize(.08,"t"); 
		c1.cd()
		hMapPedSigVsFiber.Draw('COLZ')
		c1.Update()







def HistoSamStyle(histo):
	LabelSize = 0.045
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

