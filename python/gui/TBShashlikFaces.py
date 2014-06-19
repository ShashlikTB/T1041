import os, sys, glob, random
from ROOT import *
from time import sleep
from TBUtils import *
import gui.utils


class ShashlikHeatmap:
	def __init__(self, canvas):
		self.canvas = canvas
	def __del__(self):
		pass

	def Draw(self, event, util):
		ShashlikFaces(self, self.canvas, event, util)
		
def ShashlikFaces(object, c1, event, util):
	print "ShashlikeFaces called"
	try:
		o = object.hMapFront
		t = object.hMapReset
	except:

		c1.cd()
		c1.SetRightMargin(0.18)
		c1.SetLeftMargin(0.1)

		c1.Divide(2,1)

		c1.cd(1)
		c1.SetRightMargin(0.18)
		c1.SetLeftMargin(-0.2)

		c1.cd(2)
		c1.SetRightMargin(3.9)
		c1.SetLeftMargin(-0.2)

		object.hMapFront= TH2F('hMap1','Upstream Face (ADC counts)' , 8,-28,28,8,-28,28)
		object.hMapBack = TH2F('hMap2','Downstream Face (ADC counts)',8,-28,28,8,-28,28)
		object.hMapReset = True
       		print "defining accumulate for the first time"	 
	hMapFront = object.hMapFront
	hMapBack  = object.hMapBack
	
	if util.accumulate==True:
		object.hMapReset = False
		print "not resetting"
	if util.accumulate==False:
		print "resetting"
		object.hMapReset = True

	if object.hMapReset:
		hMapFront.Reset()
		hMapBack.Reset()

	mymap = Mapper.Instance()
	print "*** mymap %s" % mymap
	for i in range(0,64):
        	pade = event.GetPadeChan(i)
        	maxADC = getWFmax(pade)
       
        	#xfill = i%8 - 4
        	#yfill = (i - i%8)/8 - 4
        	#hMapFront.Fill(xfill, yfill, maxADC) 	
                chanID = pade.GetChannelID()
        	xy = mymap.ChannelID2XY(chanID)
        	x,y = xy[0], xy[1]
		hMapFront.Fill(x, y, maxADC)

	for i in range(64,128):
		pade = event.GetPadeChan(i)
        	maxADC = getWFmax(pade)
        	chanID = pade.GetChannelID();   
        	xy = mymap.ChannelID2XY(chanID)
        	x,y = xy[0], xy[1]
        	hMapBack.Fill(x, y, maxADC)
	
	if not util.stealthmode:
		gStyle.SetPalette(1)
        	gStyle.SetOptStat(0)
		HistoSamStyleHeat(hMapFront)
		HistoSamStyleHeat(hMapBack)
        	print "must not be in stealthmode"
		c1.cd(1)
        	hMapFront.Draw('COLZ')
        	c1.cd(2)
        	hMapBack.Draw('COLZ')
        	c1.Update()
        

def HistoSamStyleHeat(histo):
	LabelSize = 0.045
	lwidth = 2
	histo.GetXaxis().SetTitle('x-position (mm)')
	histo.GetYaxis().SetTitle('y-position (mm)')
	histo.GetXaxis().SetTitleSize(LabelSize)
	histo.GetXaxis().SetTitleOffset(0.9)
	histo.GetXaxis().SetLabelSize(LabelSize)
	histo.GetYaxis().SetTitleSize(LabelSize)
	histo.GetYaxis().SetLabelSize(LabelSize)
	histo.GetYaxis().SetTitleOffset(1)
	histo.GetZaxis().SetLabelSize(0.7*LabelSize)

def getWFmax(pade):
	max = 0
	wform = pade.GetWform()
	for iwf in range(0,120):
		number = pade.GetWform()[iwf]
		if number > max:
			max = number
	return max

