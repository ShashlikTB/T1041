#!/usr/bin/env python
#-----------------------------------------------------------------------------
# File:        TB3Ddisplay.py
# Description: TB 2014 simple 3D display
# Created:     30-May-2014 Harrison B. Prosper & Sam Bein
#-----------------------------------------------------------------------------
import sys, os, re
from ROOT import *
from string import lower, replace, strip, split, joinfields, find
from array import array
import numpy as np
#------------------------------------------------------------------------------
x_y_scale_factor = 20*1.0
z_WC1 = 0
z_WC2  = 3476.6 # mm - position of  Wire chamber 2
dz_WC  =   20.0 # mm - length in z
dx1_WC =  128 * x_y_scale_factor # mm - length in x at lower z surface
dx2_WC =  128 * x_y_scale_factor# mm - length in x at upper z surface
dy_WC =  128 * x_y_scale_factor

dz_BoxEcal = 253 #length of gap 
z_Ecal = z_WC2 + 533.4 + dz_BoxEcal #position of front face of Ecal
dx1_Tower =  56/4 * x_y_scale_factor # mm - length in x of cell
dx2_Tower =  56/4 * x_y_scale_factor # mm - same thing
dy_Tower  =  56/4 * x_y_scale_factor # mm - length in y of cell
dz_Tower  = 113.5  # mm - full length in z (of shashlik)

dz_setup = z_Ecal + 2*dz_BoxEcal + dz_Tower
transparency = 1
tracklength = dz_setup + 800 #length of track to dr

class Display3D:

    def __init__(self, page):

        self.page = page
        self.first = True

    def __del__(self):
        pass

    def Show(self, util):
        if self.first:
             gEve.Redraw3D(kTRUE)
             self.first = False
             return
        gEve.Redraw3D(kFALSE)
        
        
    #----------------------------------------------------------------------
    # Draw hits
    #----------------------------------------------------------------------
    def Draw(self, event, util):	
        
        elements = self.page.elements

        if util.accumulate:
            for i in range(130):
            	elements.LastChild().Destroy()
        
        if not util.accumulate:   
            elements.DestroyElements()   

            
        
        hitx1 = -util.x1hit * x_y_scale_factor #left-handed coordinate system!
        hity1 = util.y1hit * x_y_scale_factor
        hitx2 = -util.x2hit * x_y_scale_factor #left-handed coordinate system!
        hity2 = util.y2hit * x_y_scale_factor

        tableX = util.tableX * x_y_scale_factor
        tableY = util.tableY * x_y_scale_factor

        if abs(util.tableX) > 28:
            tableX = 0
        if abs(util.tableY) > 28:
            tableY = 0
        
        for hit1 in range(0,util.WC1Xallhits.size()):
            blob = TEveGeoShape(str(hit1+1)+'blobby'+str(util.eventNumber))
            blob.SetShape(TGeoSphere(0,50.0))
            blob.SetMainColor(kBlue)
            blob.RefMainTrans().SetPos(-x_y_scale_factor*util.WC1Xallhits[hit1],x_y_scale_factor* util.WC1Yallhits[hit1], 0)
            elements.AddElement(blob)
        
        for hit2 in range(0,util.WC2Xallhits.size()):
            blob = TEveGeoShape(str(hit2+1)+'blobbby'+str(util.eventNumber))
            blob.SetShape(TGeoSphere(0,50.0))
            blob.SetMainColor(kBlue)
            blob.RefMainTrans().SetPos(-x_y_scale_factor*util.WC2Xallhits[hit2],x_y_scale_factor* util.WC2Yallhits[hit2], z_WC2)
            elements.AddElement(blob)
        
        
        self.blob1 = TEveGeoShape('blob1_'+str(util.eventNumber))
        self.blob1.SetShape(TGeoSphere(0,50.0))
        self.blob1.SetMainColor(kRed)
        self.blob1.RefMainTrans().SetPos(hitx1, hity1, 0)
        elements.AddElement(self.blob1)
        self.blob2 = TEveGeoShape('blob2_'+str(util.eventNumber))
        self.blob2.SetShape(TGeoSphere(0,50.0))
        self.blob2.SetMainColor(kRed)
        self.blob2.RefMainTrans().SetPos(hitx2, hity2,z_WC2)
        elements.AddElement(self.blob2)
        

        
        #elements.AddElement(TEveElement(util.mycanv))
        
        
        self.track = TEveGeoShape('track_'+str(util.eventNumber))
        self.track.SetShape(TGeoEltu(5,5,tracklength/2))
        self.track.SetMainColor(kYellow)
        thx = atan((hitx1-hitx2)/z_WC2)
        thy = atan((hity1-hity2)/z_WC2)
        offsetx = -(dz_setup)/2*tan(thx)
        offsety = -(dz_setup)/2*tan(thy)
        setx = hitx1+offsetx
        sety = hity1+offsety
        self.track.RefMainTrans().SetPos(setx, sety, dz_setup/2)
        
        self.track.RefMainTrans().SetRotByAngles(0, thx, thy)
        elements.AddElement(self.track)
        
        
        self.wc1 = TEveGeoShape('WC1_'+str(util.eventNumber))
        self.wc1.SetShape( TGeoTrd1(dx1_WC/2, dx2_WC/2, dy_WC/2, dz_WC/2) )
        self.wc1.SetMainColor(kCyan)
        self.wc1.SetMainTransparency(30)
        if util._3D_showWC1:
            elements.AddElement(self.wc1)
            

         
        self.wc2 = TEveGeoShape('WC2_'+str(util.eventNumber))
        self.wc2.SetShape( TGeoTrd1(dx1_WC/2, dx2_WC/2, dy_WC/2, dz_WC/2) )
        self.wc2.SetMainColor(kMagenta)
       	self.wc2.SetMainTransparency(50)
       	self.wc2.RefMainTrans().SetPos(0,0, z_WC2)
        if util._3D_showWC2:
            elements.AddElement(self.wc2)
            
          

        # Draw shashlik modules
        step = dx1_Tower/2
        xmin =-4*step + tableX
        ymin =-4*step + tableY

        self.module = []
        for ii in xrange(8):
            x = -(xmin + (ii+0.5)*step)
            for jj in xrange(8):
                y = ymin + (jj+0.5)*step
                self.module.append(TEveGeoShape('ShashlikUp%d_%d_%d' % (ii, jj, util.eventNumber)))
                self.module[-1].SetShape( TGeoTrd1(dx1_Tower/4, dx2_Tower/4,
						   dy_Tower/4, dz_Tower/4) )
                #self.module[-1].SetMainColor(color[icolor])
                color = util.colorsUpstream[8*ii+jj]
                self.module[-1].SetMainColor(color)
                if util._3D_isolateClusters and (color==53 or color==55 or color==58 or color==60):
                    self.module[-1].SetMainColor(0)
                    self.module[-1].SetMainTransparency(100)
                self.module[-1].RefMainTrans().SetPos(x, y, z_Ecal)
                elements.AddElement(self.module[-1])
                

        
        self.module = []
        for ii in xrange(8):
            x = -(xmin + (ii+0.5)*step)
            for jj in xrange(8):
                y = ymin + (jj+0.5)*step
                self.module.append(TEveGeoShape('ShashlikD%d_%d_%d' % (ii, jj, util.eventNumber)))
                self.module[-1].SetShape( TGeoTrd1(dx1_Tower/4, dx2_Tower/4,
						   dy_Tower/4, dz_Tower/4) )
                #self.module[-1].SetMainColor(color[icolor])
                color = util.colorsDownstream[8*ii+jj]
                self.module[-1].SetMainColor(color)
                if util._3D_isolateClusters and (color==53 or color==55 or color==58 or color==60):
                    self.module[-1].SetMainColor(0)
                    self.module[-1].SetMainTransparency(100)
                self.module[-1].RefMainTrans().SetPos(x, y, z_Ecal+dz_Tower/2)
                elements.AddElement(self.module[-1])

        block = TEveGeoShape('BalanceBlock')
        block.SetShape(TGeoTrd1(dx1_Tower/4, dx2_Tower/4,
						   dy_Tower/4, dz_Tower/4))
        block.SetMainColor(0)
        block.SetMainTransparency(100)
        block.RefMainTrans().SetPos(0, 0, z_Ecal+dz_Tower + z_Ecal)
        elements.AddElement(block)
        
        self.Show(util)

    
        #myplot= TGLPlot3D.CreatePlot(mypad1,'0',mypad1)
        #myplot.SetAxisAlignedBBox(10, 20,15, 25, -1, 1)
        #myplot.Draw('sames')      
