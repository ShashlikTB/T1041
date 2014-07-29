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
        t = object.hMapBack
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

        object.hMapFront= TH2F('hMap1','Upstream Face ADC - evt '+str(util.eventNumber) , 8,-28,28,8,-28,28)
        object.hMapBack = TH2F('hMap2','Downstream Face ADC - evt '+str(util.eventNumber),8,-28,28,8,-28,28)
        object.hMapReset = True

        object.hMapFront.SetTitle('Upstream Face ADC - evt '+str(util.eventNumber) )
        object.hMapBack.SetTitle('Downstream Face ADC - evt '+str(util.eventNumber))
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
    
    
    for i in range(0,128):
        pade = event.GetPadeChan(i)
        chanID = pade.GetChannelID()
        fiber = mymap.ChannelID2FiberID(chanID)
        if not (fiber > 0):
            continue
        if not util.stealthmode:
            pade.Dump()
        #maxADC = pade.GetMax()
        maxADC = getWFmax(pade)
        xy = mymap.ChannelID2XY(chanID)
        x,y = xy[0], xy[1]
        hMapFront.Fill(x, y, maxADC)
        #hMapFront.SetRange(hMapFront.GetMinimum()-1,hMapFront.GetMaximum()+1,"Z")
        
        
    for i in range(0,128):
        pade = event.GetPadeChan(i)
        chanID = pade.GetChannelID()
        fiber = mymap.ChannelID2FiberID(chanID)
        if not (fiber < 0):
            continue
        if not util.stealthmode:
            pade.Dump()
        #maxADC = pade.GetMax()
        maxADC = getWFmax(pade)
        chanID = pade.GetChannelID();   
        xy = mymap.ChannelID2XY(chanID)
        x,y = xy[0], xy[1]
        hMapBack.Fill(x, y, maxADC)
        #hMapBack.SetAxisRange(hMapBack.GetMinimum()-1,hMapBack.GetMaximum()+1,"Z")
        
    if not util.stealthmode:
        gStyle.SetPalette(1)
        gStyle.SetOptStat(0)
        HistoSamStyleHeat(hMapFront)
        HistoSamStyleHeat(hMapBack)
        c1.cd(1)
        hMapFront.Draw('COLZ text')
        c1.cd(2)
        hMapBack.Draw('COLZ text')
        c1.Update()
        try:
            hMapFrontCopy.Destroy()
            hMapBackCopy.Destroy()
        except:
            pass
        hMapFrontCopy = hMapFront.Clone()
        hMapBackCopy = hMapBack.Clone()

        hMapFrontCopy.SetAxisRange(hMapFront.GetMinimum(),hMapFront.GetMaximum()+4,"Z")
        hMapBackCopy.SetAxisRange(hMapBack.GetMinimum(),hMapBack.GetMaximum()+4,"Z")
        paletteUpstream = hMapFrontCopy.GetListOfFunctions().FindObject('palette')
        paletteDownstream = hMapBackCopy.GetListOfFunctions().FindObject('palette')                            
        util.colorsDownstream.clear()
        util.colorsUpstream.clear()
        for i in range (1,9):
            for j in range(1,9):
                util.colorsDownstream.push_back(paletteDownstream.GetBinColor(i,j))
                util.colorsUpstream.push_back(paletteUpstream.GetBinColor(i,j))
                
        
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
	for iwf in range(0,PadeChannel().N_PADE_SAMPLES):  
		number = pade.GetWform()[iwf]
		if number > max:
			max = number
	return max

