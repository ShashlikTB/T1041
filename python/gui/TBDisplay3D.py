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
import numpy 
#------------------------------------------------------------------------------
x_y_scale_factor = 20
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

        # Coordinates:
        # +z to the right, in direction of beam
        # +x into the page
        # +y vertically upwards

        self.geometry = TEveElementList("Testbeam 2014 Geometry")

        tracklength = dz_setup + 800 #length of track to draw

        self.wc1 = TEveGeoShape('WC 1')
        self.wc1.SetShape( TGeoTrd1(dx1_WC/2, dx2_WC/2, dy_WC/2, dz_WC/2) )
        self.wc1.SetMainColor(kCyan)
        self.wc1.SetMainTransparency(transparency)
        self.geometry.AddElement(self.wc1)

	self.wc2 = TEveGeoShape('WC 2')
        self.wc2.SetShape( TGeoTrd1(dx1_WC/2, dx2_WC/2, dy_WC/2, dz_WC/2) )
        self.wc2.SetMainColor(kMagenta)
       	self.wc2.SetMainTransparency(50)
       	self.wc2.RefMainTrans().SetPos(0,0, z_WC2)
        self.geometry.AddElement(self.wc2)

        # Draw shashlik modules
        color = [kYellow, kBlue]
        kk = -1
        step = dx1_Tower
        xmin =-2*step
        ymin =-2*step
        self.module = []
        for ii in xrange(4):
            x = xmin + (ii+0.5)*step
            for jj in xrange(4):
                kk += 1
                y = ymin + (jj+0.5)*step
                icolor = kk % 2
                if jj == 3: kk += 1

                self.module.append(TEveGeoShape('Shashlik%d%d' % (ii, jj)))
                self.module[-1].SetShape( TGeoTrd1(dx1_Tower/2, dx2_Tower/2,
						   dy_Tower/2, dz_Tower/2) )
                self.module[-1].SetMainColor(color[icolor])
                #self.module[-1].SetMainTransparency(2)
                self.module[-1].RefMainTrans().SetPos(x, y, z_Ecal)
                self.geometry.AddElement(self.module[-1])

        gEve.AddElement(self.geometry)

    def __del__(self):
        pass

    def Show(self, util):
        #gEve.Redraw3D(kTRUE)
        #refPos = [5.1,60.2,100.3]
        #refPos = numpy.asarray(refPos, dtype=numpy.float64)
        #print "my crazy array is ", refPos
        
        #v= gEve.GetDefaultGLViewer()
        #v.CurrentCamera().SetCenterVec(70, 1070, 0.09*util.eventNumber)
        #cam = v.CurrentCamera()
        #cam.SetCenterVec(70, 1070, 10700)
        #cam.Configure(0,500,refPos,.1*util.eventNumber, 1.01*util.eventNumber)
        #cam.UpdateInterest(True)
        #v.SetResetCamerasOnUpdate(kFALSE)
        gEve.Redraw3D(kTRUE)        
        #gEve.DoRedraw3D()
        
        
    #----------------------------------------------------------------------
    # Draw hits
    #----------------------------------------------------------------------
    def Draw(self, event, util):	
        from random import uniform
        from random import randint
        
        
        elements = self.page.elements
        if not util.accumulate:
        	elements.DestroyElements()       
        
        hitx1 = util.x1hit
        hity1 = util.y1hit
        hitx2 = util.x2hit
        hity2 = util.y2hit
        
        self.blob1 = TEveGeoShape('blob1')
        self.blob1.SetShape(TGeoSphere(0,50.0))
        self.blob1.SetMainColor(kRed)
        self.blob1.RefMainTrans().SetPos(x_y_scale_factor*hitx1,x_y_scale_factor*hity1,0)
        elements.AddElement(self.blob1)
        self.blob2 = TEveGeoShape('blob2')
        self.blob2.SetShape(TGeoSphere(0,50.0))
        self.blob2.SetMainColor(kRed)
        self.blob2.RefMainTrans().SetPos(x_y_scale_factor*hitx2,x_y_scale_factor*hity2,z_WC2)
        elements.AddElement(self.blob2)

        
        self.track = TEveGeoShape('track')
        self.track.SetShape(TGeoEltu(5,5,tracklength/2))
        self.track.SetMainColor(kYellow)
        thx = atan((hitx1-hitx2)*x_y_scale_factor/z_WC2)
        thy = atan((hity1-hity2)*x_y_scale_factor/z_WC2)
        offsetx = -(dz_setup)/2*tan(thx)
        offsety = -(dz_setup)/2*tan(thy)
        setx = x_y_scale_factor*hitx1+offsetx
        sety = x_y_scale_factor*hity1+offsety
        self.track.RefMainTrans().SetPos(setx, sety, dz_setup/2)
        
        self.track.RefMainTrans().SetRotByAngles(0, thx, thy)
        elements.AddElement(self.track)
    
                
        self.Show(util)
                
