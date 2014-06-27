#!/usr/bin/env python
#-----------------------------------------------------------------------------
# File:        TBdisplay.py
# Description: TB 2014 Event Display
# Created:     09-May-2014 Harrison B. Prosper & Sam Bein
#-----------------------------------------------------------------------------
import sys, os
from ROOT import *
from gui.TBEventDisplay import TBEventDisplay
#------------------------------------------------------------------------------
def main():
	#mystruct = Util()
	#exit(0)
	if len(sys.argv) > 1:
		filename = sys.argv[1]
	else:
		filename = None
		
	gui = TBEventDisplay("CMS Test Beam 2014 Event Display", filename)
	gui.run()
#------------------------------------------------------------------------------
try:
	main()
except KeyboardInterrupt:
	print
	print 'ciao!'




