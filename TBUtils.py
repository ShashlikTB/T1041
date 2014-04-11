import sys, bz2, inspect
from commands import getoutput,getstatusoutput


# a simple command sting builder
def ccat(*arg):
    cc=""
    for i in range(len(arg)): cc=cc+" "+str(arg[i])
    return cc

# A simple error logger class
# Instantiate as logger=Logger(num=1) 
# Print information messages and up to num (default=1) occurances of each warning
# The Summary method provides statistics on all warnings

class Logger():
    def __init__(self,max=1):
        self.warnings={}
        self.RED='\033[91m'
        self.COL_OFF='\033[0m'
        self.max=max
        print "Init logger, max print count =",max
    def Info(self,*arg):
        msg=ccat(*arg)
        sys.stdout.write("Info: "+msg+"\n")   
    def Warn(self,*arg):
        msg=ccat(*arg)
        if msg in self.warnings: self.warnings[msg]=self.warnings[msg]+1
        else: self.warnings[msg]=1
        if self.warnings[msg]<=self.max: 
            sys.stdout.write(self.RED+"Warning: "+msg+"\n"+self.COL_OFF)
    def Fatal(self,*arg):
        msg=ccat(*arg)
        sys.stdout.write(self.RED+"**FATAL**: "+msg+"\n"+self.COL_OFF)
        sys.exit(1)
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

def TBOpen(fin):
    if fin.endswith("bz2"): return bz2.BZ2File(fin,"r")
    else: return open(fin,"r")

##############################
# data file parsers
##############################
def ParsePADEdata(padeline):
    padeline=padeline.split()
    pade_ts=long(padeline[0])
    pade_transfer_size=int(padeline[1]+padeline[2],16)
    pade_board_id=int(padeline[3],16)
    pade_hw_counter=int(padeline[4]+padeline[5]+padeline[6],16)
    pade_ch_number=int(padeline[7],16)
    eventNumber = int(padeline[8]+padeline[9],16)
    waveform=(padeline[10:])
    return (pade_ts,pade_transfer_size,pade_board_id,
            pade_hw_counter,pade_ch_number,eventNumber,waveform)



def readWCevent(fWC):
    endOfEvent=0
    nhits=0
    while 1:
        wcline=fWC.readline()
        if not wcline: break
        if "SPILL" in wcline: continue
        wcline=wcline.split()
        if not foundWC and "EVENT" in wcline[0]:  # found new event
            trigWCrun=wcline[1]   
            trigWCspill=wcline[2] 
            foundWC=true
            continue
        elif "EVENT" in wcline[0]: 
            fWC.seek(endOfEvent)
            break
        if "Module" in wcline[0]: 
            tdcNum=int(wcline[1])
            endOfEvent=fWC.tell()
        if "Channel" in wcline[0]: 
            wire=int(wcline[1])
            tdcCount=int(wcline[2])
            eventDict[eventNumber].AddWCHit(tdcNum,wire,tdcCount) #!
            endOfEvent=fWC.tell()
            if DEBUG_LEVEL>1: event.GetWCChan(nhits).Dump()
            nhits=nhits+1

def getWCspills(fWC):
    
    endOfEvent=0
    fWC.seek(loc)
    while 1:
        wcline=fWC.readline()
        if not wcline: return -1
        if "SPILL" in wcline: continue
        wcline=wcline.split()
        if not foundWC and "EVENT" in wcline[0]:  # found new event
            trigWCrun=wcline[1]   
            trigWCspill=wcline[2] 
            foundWC=true
            continue
        elif "EVENT" in wcline[0]: 
            fWC.seek(endOfEvent)
            break
        if "Module" in wcline[0]: 
            tdcNum=int(wcline[1])
            endOfEvent=fWC.tell()
        if "Channel" in wcline[0]: 
            wire=int(wcline[1])
            tdcCount=int(wcline[2])
            eventDict[eventNumber].AddWCHit(tdcNum,wire,tdcCount) #!
            endOfEvent=fWC.tell()
            if DEBUG_LEVEL>1: event.GetWCChan(nhits).Dump()
            nhits=nhits+1


