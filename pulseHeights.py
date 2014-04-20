# Run as siple pulse height viewer
# this is just a wrapper for the ROOT C++ code
# will replace w/ a simple GUI

# Created 4/19/2014 B.Hirosky: Initial release

import getopt, sys
from ROOT import *
from TBUtils import *

def usage():
    print
    print "Usage: python pulseHeights.ps [OPTION]"
    print "      -b boardID  [112]"
    print "      -f rootFile [latest.root]"
    print "      -m minimum of pulseheight histogram [80]"
    print "      -M maximium of pulseheight histogram [300]"
    print "      -h print this message"
    print 
    sys.exit()



fname="latest.root"
board=112
xmin=80
xmax=300

opts, args = getopt.getopt(sys.argv[1:], "b:f:m:M:h")
for o, a in opts:
    if o == "-b": board=int(a)
    elif o == "-f": fname=a
    elif o=="-m": xmin=int(a)
    elif o=="-M": xmax=int(a)
    elif o=="-h": usage()


gROOT.ProcessLine(".L pulseHeights.C")
pulseHeights(board,fname,xmin,xmax)

tb=TBrowser()

hit_continue('Hit any key to exit')












