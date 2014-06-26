import os, sys, glob, random
from ROOT import *
from time import sleep
from TBUtils import *
import gui.utils


class TDCtiming:
    def __init__(self, canvas):
        self.canvas = canvas
    def __del__(self):
        pass

    def Draw(self, event, util):
        tdcWorkHorse(self, self.canvas, event, util)


def tdcWorkHorse(object, c1, event, util):

    try:
        o = object.names
    except:
        c1.Divide(4,4)
    
    print "getting stuff done"
    file = TFile('tdc_dists.root')
    object.names = [k.GetName() for k in file.GetListOfKeys()]
    names = object.names
    gStyle.SetOptStat(0)
    t = TLatex()
    t.SetTextSize(0.15)
    for i in range(0,len(names)):
        c1.cd(i+1);
        histo = file.Get(names[i])
        HistoSamStyleTDC(histo)       
        histo.Draw();
        t.DrawLatex(175, histo.GetMaximum()*0.75, 'TDC '+str(i+1))  
    c1.Update()
    file.Close()



def HistoSamStyleTDC(histo):
    LabelSize = 0.15
    lwidth = 2
    histo.SetTitle('')
    histo.GetXaxis().SetTitle('sample # ')
    histo.GetXaxis().SetTitleSize(LabelSize)
    histo.GetXaxis().SetTitleOffset(-.3)
    histo.GetXaxis().SetLabelSize(LabelSize - 0.03)
    histo.GetYaxis().SetTitle('counts ')
    histo.GetYaxis().SetTitleSize(LabelSize)
    histo.GetYaxis().SetLabelSize(LabelSize - 0.03)
    histo.GetYaxis().SetTitleOffset(-.2)

