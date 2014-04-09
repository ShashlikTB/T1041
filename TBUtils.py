import sys
from commands import getoutput,getstatusoutput

#def INFO(*arg):
#    s=""
#    for i in range(len(arg)):
#        s=s+" "+str(arg[i])
#    sys.stdout.write("Info: "+s+"\n")


class Logger():
    def __init__(self,max=1):
        self.warnings={}
        self.RED='\033[91m'
        self.COL_OFF='\033[0m'
        self.max=max
        print "Init logger, max print count =",max
    def Info(self,*arg):
        msg=""
        for i in range(len(arg)):
            msg=msg+" "+str(arg[i])
        sys.stdout.write("Info: "+msg+"\n")   
    def Warn(self,*arg):
        msg=""
        for i in range(len(arg)): msg=msg+" "+str(arg[i])
        if msg in self.warnings: self.warnings[msg]=self.warnings[msg]+1
        else: self.warnings[msg]=1
        if self.warnings[msg]<=self.max: 
            sys.stdout.write(self.RED+"Warning: "+msg+"\n"+self.COL_OFF)
    def Summary(self):
        print
        print "="*40
        print " WARNING Summary"
        print "="*40
        print 
        if len(self.warnings)==0: print "No Warnings reported"
        else:
            for a in self.warnings: print "%5d %s" % (self.warnings[a],a)
        print "="*40
        print " WARNING Summary (end)"
        print "="*40        
