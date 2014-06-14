#-----------------------------------------------------------------------------
# File:        utils.py
# Description: Some gui utilities
# Created:     09-May-2014 Harrison B. Prosper & Sam Bein
#-----------------------------------------------------------------------------
import sys, os, re, platform
from ROOT import *
from string import lower, replace, strip, split, joinfields, find
from array import array
from TBUtils import *
#-----------------------------------------------------------------------------
print "==> Loading libTB.so"
gSystem.Load("libTB.so")
ICONDIR = "%s/icons" % os.environ["TBHOME"]
#-----------------------------------------------------------------------------

# GUI widget Layouts


RIGHT        = TGLayoutHints(kLHintsRight)
RIGHT_X      = TGLayoutHints(kLHintsRight | kLHintsExpandX)
RIGHT_X_Y    = TGLayoutHints(kLHintsRight | kLHintsExpandX | kLHintsExpandY)

LEFT         = TGLayoutHints(kLHintsLeft)
LEFT_X       = TGLayoutHints(kLHintsLeft | kLHintsExpandX)
LEFT_X_Y     = TGLayoutHints(kLHintsLeft | kLHintsExpandX | kLHintsExpandY)


TOP          = TGLayoutHints(kLHintsTop)
TOP_X        = TGLayoutHints(kLHintsTop  | kLHintsExpandX)
TOP_X_SUNKEN = TGLayoutHints(kLHintsTop  | kLHintsExpandX | kSunkenFrame)
TOP_LEFT_X   = TGLayoutHints(kLHintsTop  | kLHintsLeft | kLHintsExpandX)
TOP_LEFT_X_Y = TGLayoutHints(kLHintsTop  | kLHintsLeft | kLHintsExpandX |
							 kLHintsExpandY)
TOP_LEFT     = TGLayoutHints(kLHintsTop  | kLHintsLeft)
TOP_LEFT_PADDED     = TGLayoutHints(kLHintsTop  | kLHintsLeft,  5, 5, 2, 2)
TOP_RIGHT_PADDED    = TGLayoutHints(kLHintsTop  | kLHintsRight, 5, 5, 2, 2)
TOP_RIGHT_X_PADDED  = TGLayoutHints(kLHintsTop  | kLHintsRight | \
									kLHintsExpandX, 5, 5, 2, 2)


TOP_RIGHT    = TGLayoutHints(kLHintsTop  | kLHintsRight)
TOP_X_Y      = TGLayoutHints(kLHintsTop  | kLHintsExpandX | kLHintsExpandY)
TOP_X_Y_RAISED = TGLayoutHints(kLHintsTop | kLHintsExpandX | kLHintsExpandY | \
							   kRaisedFrame)

K_PROG_MAX  = 20.0
C_STANDARD  = 0
C_OPENGL    = 1

BLACK       = Util.Color("black")
WHITE       = Util.Color("white")
RED         = Util.Color("red")
ORANGE      = Util.Color("orange")
YELLOW      = Util.Color("yellow")
GREEN       = Util.Color("green")
BLUE        = Util.Color("blue")
DARKRED     = Util.Color("darkred")
LIGHTYELLOW = Util.Color("lightyellow")
LIGHTGREEN  = Util.Color("lightgreen")
#-----------------------------------------------------------------------------
class Element:
	pass

class MenuBar(TGMenuBar):
	
	def __init__(self, object, frame, layout=TOP_LEFT_PADDED):
		TGMenuBar.__init__(self, frame)
		frame.AddFrame(self, TOP_X)

		self.object   = object
		self.frame    = frame
		self.layout   = layout
		self.number   = 0
		self.callbacks= {}
		self.elements = []

		
	def __del__(self):
		pass

	def Add(self, name, items):
		menu = TGPopupMenu(gClient.GetRoot())  	
		connection = Connection(menu, "Activated(Int_t)",
								self, "menu")
		self.AddPopup(name, menu, self.layout)
		self.elements.append((menu, connection))

		# add menu items
		for item in items:
			if type(item) == type(0):
				menu.AddSeparator()
			else:
				self.number += 1
				namen, method = item
				menu.AddEntry(namen, self.number)
				self.callbacks[self.number] = 'self.object.%s()' % method

   # Responds to: Activated(Int_t)
	def menu(self, number):
		if self.callbacks.has_key(number):
			exec(self.callbacks[number])
		else:
			print "Unrecognized menu id = %d" % number


#-----------------------------------------------------------------------------
class ProgressBar(TGHProgressBar):
	
	def __init__(self, object, toolBar, method, seconds=0.2):
		TGHProgressBar.__init__(self, toolBar,
								TGProgressBar.kFancy, 1)
		
		toolBar.AddFrame(self,
						 TGLayoutHints(kLHintsLeft |
									   kLHintsExpandX |
									   kLHintsExpandY,
									   10, 10, 2, 2))

		self.SetBarColor("green")
		self.SetRange(0, K_PROG_MAX)

		# Set up a timer for progress bars
		self.timer = TTimer()
		self.connection = Connection(self.timer, "Timeout()", object, method)

	def __del__(self):
		pass
	
#-----------------------------------------------------------------------------
buttonNumber=-1
class PictureButton(TGPictureButton):
	
	def __init__(self, object, toolBar,
		     picture,
		     method,
		     text='',
		     layout=kLHintsRight):
		global buttonNumber
		buttonNumber += 1
		number = buttonNumber
		
		self.pool = TGPicturePool(gClient, ICONDIR)
		if self.pool:
			self.picture = self.pool.GetPicture(picture)
			if self.picture:
				TGPictureButton.__init__(self, toolBar, self.picture, number)
				toolBar.AddFrame(self,
						 TGLayoutHints(layout, 2, 2, 2, 2))
				self.SetToolTipText(text)
				self.connection = Connection(self, "Clicked()",
							     object, method)
	def __del__(self):
		pass
#-----------------------------------------------------------------------------

class CheckButton(TGCheckButton):
	
	def __init__(self, object, toolBar,
		     hotstring,
		     method,
		     text='',
		     layout=kLHintsRight):
		global buttonNumber
		buttonNumber += 1
		number = buttonNumber
		
		self.hotstring = hotstring
		if self.hotstring:
            		TGCheckButton.__init__(self, toolBar, self.hotstring, number)
            		toolBar.AddFrame(self,
                        	         TGLayoutHints(layout, 2, 2, 2, 2))
            	self.SetToolTipText(text)
            	self.connection = Connection(self, "Clicked()",
                                         object, method)
	def __del__(self):
		pass
#-----------------------------------------------------------------------------
class NoteBook(TGTab):
	
	def __init__(self, object, frame, method, width=800, height=600):
		TGTab.__init__(self, frame, 1, 1)
		
		frame.AddFrame(self, TOP_X_Y)
		self.connection = Connection(self, "Selected(Int_t)",
									 object, method)
		self.number=-1
		self.pages = {}
		self.names = {}
		self.width = width
		self.height= height
		self.currentPage = 0
		self.page = None
		
	def __del__(self):
		pass


	def Add(self, name, canvasType=C_STANDARD, menu=None):
		self.number += 1
		self.names[name] = self.number
		self.pages[self.number] = Element()
		element = self.pages[self.number]
		self.page = element
		
		element.name   = name
		element.redraw = True
		element.tab    = self.AddTab(name)
		
		# check for menu items
		if menu != None:
			element.hframe2  = TGHorizontalFrame(element.tab, 1, 1)
			element.tab.AddFrame(element.hframe2, TOP_X)				
			element.menuBar = MenuBar(object, element.hframe2,
						  TOP_RIGHT_PADDED)
			menuName, menuItems = menu
			element.menuBar.Add(menuName, menuItems)

		element.hframe  = TGHorizontalFrame(element.tab, 1, 1)
		element.tab.AddFrame(element.hframe, TOP_X_Y)

		from string import upper
		standardCanvas = find(upper(name), '3D') < 0
		if standardCanvas:
			# set up a regular ROOT  canvas
			element.ecanvas = TRootEmbeddedCanvas("c%s" % name,
							      element.hframe,
							      self.width,
							      self.height)
			element.canvas  = element.ecanvas.GetCanvas()
			element.hframe.AddFrame(element.ecanvas, TOP_X_Y)
		else:
			# set up a canvas that can handle OpenGL			
			element.viewer  = TGLEmbeddedViewer(element.hframe)
			element.hframe.AddFrame(element.viewer.GetFrame(),
						TOP_X_Y)
			TEveManager.Create(kFALSE)
			element.canvas = TEveViewer("TB2014 Viewer")
			TEveManager.Create(kTRUE)
			
			element.canvas.SetGLViewer(element.viewer,
						   element.viewer.GetFrame())
			element.canvas.AddScene(gEve.GetEventScene())
			gEve.GetViewers().AddElement(element.canvas)
			
			element.elements = TEveElementList()
			gEve.AddElement(element.elements)


	def SetPage(self, id):

		# Before changing current tab's color,
		# re-color previous tab to the
		# TGMainframe's default color

		tab = self.GetTabTab(self.currentPage)
		tab.ChangeBackground(self.GetDefaultFrameBackground())

		# Now change tab and re-color tab

		if self.pages.has_key(id):
			self.currentPage = id
			
		elif self.names.has_key(id):
			self.currentPage = self.names[id]

		self.page = self.pages[self.currentPage]
		self.GetTabTab(self.currentPage).ChangeBackground(YELLOW)
		self.SetTab(self.currentPage)
