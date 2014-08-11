# Created 4/12/2014 B.Hirosky: Initial release

import sys, os, bz2, inspect, re, time, collections, StringIO, pickle
from commands import getoutput,getstatusoutput
from ROOT import *

def hit_continue(msg='Hit any key to continue'):
    print
    print msg
    sys.stdout.flush() 
    raw_input('')

# a simple command string builder
def ccat(*arg):
    cc=""
    for i in range(len(arg)): cc=cc+" "+str(arg[i])
    return cc

def checkDep(file,msg=""):
    if not os.path.isfile(file):
        print file, "not found"
        if msg!="": print msg
        sys.exit()

def checkEnv(var,msg=""):
    if os.getenv(var)==None:
        print var, "not found"
        if msg!="": print msg
        sys.exit()

def LoadLibs(tblib,*libs):
    checkEnv(tblib,"Source the setup script")
    tblib=str(os.getenv("TBLIB"))
    for i in range(len(libs)): 
        lib=tblib+"/"+str(libs[i])
        gSystem.Load(lib)

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
        self.logfile=""
        self.stdout=sys.stdout
        print "Init logger, max print count =",max
    def SetLogFile(self,logfile):
        self.logfile=logfile
        self.stdout = open(self.logfile, 'w') # output socket
    def Info(self,*arg):
        msg="Info: "+ccat(*arg)+"\n"
        sys.stdout.write(msg)   
        if (self.logfile !=""): self.stdout.write("Info: "+msg+"\n")   
    def Warn(self,*arg):
        msg="Warning: "+ccat(*arg)+"\n"
        if msg in self.warnings: self.warnings[msg]=self.warnings[msg]+1
        else: self.warnings[msg]=1
        if self.warnings[msg]<=self.max: 
            sys.stdout.write(self.RED+msg+self.COL_OFF)
            if (self.logfile !=""): self.stdout.write(msg)
    def Fatal(self,*arg):
        msg="**FATAL**: "+ccat(*arg)+"\n"
        sys.stdout.write(self.RED+msg+self.COL_OFF)
        if (self.logfile !=""): self.stdout.write(msg)
        sys.exit(1)
    def Summary(self):
        output = StringIO.StringIO()
        print >>output
        print >>output,"="*40
        print >>output," WARNING Summary"
        print >>output,"="*40
        print >>output
        if len(self.warnings)==0: print >>output,"No Warnings reported"
        else:
            owarn = collections.OrderedDict(sorted(self.warnings.items()))
            for a in owarn: print >>output,"(%5d) %s" % (owarn[a],a)
        print >>output,"="*40
        print >>output," WARNING Summary (end)"
        print >>output,"="*40  
        print output.getvalue()
        if (self.logfile !=""): self.stdout.write(output.getvalue())
        output.close()

# hack to pass immutable data types "by reference" (under consideration)
class pyref():  
    def __init__(self,data):
        self.data=[data]
    def ref(self):
        return self.data[0]


def TBOpen(fin):
    if fin.endswith("bz2"): return bz2.BZ2File(fin,"r")
    else: return open(fin,"r")

##############################
# data file parsers
##############################
def ParsePadeData(padeline):
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

def ParsePadeSpillHeader(padeline):
    spill = { 'number':0, 'pctime':0, 'nTrigWC':0, 'wcTime':0, 'status':0 }
 # check for fake run or # WC time stamp missing
    haveWCtime = True
    if padeline.endswith("time =") or padeline.endswith("time unknown"): haveWCtime=False
    padeline=padeline.split()

    # hacky special case error checks
    if len(padeline)==18:
        spill['status']=-1 #error type 1
        return spill
    if len(padeline)==14:
        spill['status']=-2 #error type 2
        return spill

    spill['number']=int(padeline[4])
    pcTime=padeline[7]+" "+padeline[8]+" "+padeline[9]
    spill['pcTime']=time.mktime(time.strptime(pcTime, "%m/%d/%Y %H:%M:%S %p"))
    spill['nTrigWC']=wcTiggers=int(padeline[14],16)
    # check for fake run or # WC time stamp missing
    if haveWCtime: 
        wcTime=padeline[17]+" 20"+padeline[18]  # fix format to 4-digit year
        spill['wcTime']=time.mktime(time.strptime(wcTime, "%H:%M:%S %Y/%m/%d"))
    else:
        spill['wcTime']=0
    return spill


def ParsePadeBoardHeader(padeline):
    master = "Master" in padeline
    padeline=re.sub('=', ' ', padeline).split()
    boardID=int(padeline[5])
    status=int(padeline[7],16)
    trgStatus=int(padeline[9],16)
    events=int(padeline[13],16)
    memReg=int(padeline[16],16)
    trigPtr=int(padeline[19],16)
    pTemp=int(padeline[21],16)
    sTemp=int(padeline[23],16)
    return (master,boardID,status,trgStatus,events,memReg,trigPtr,pTemp,sTemp)


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


# WC Database lookup
# match WC spills w/in PAST 45 seconds of WC timestamp read by PADE
def wcLookup(tgttime, bound=45, filename="wcdb.txt"):
    print "tgttime",tgttime
    lookval=int(tgttime)/100   # seek matches w/in 100 second time range
    try:
        stat,spills=getstatusoutput("look "+str(lookval)+" "+filename)   # binary search of file
        if (int(stat)!=0) or len(spills)==0: 
            return (-1, None)                # no lines match
        spills=spills.split("\n") 
        for spill in spills:   # search spills <100 seconds from time in PADE spill header 
            print spill
            split = re.split(' +', spill.strip())
            sTime = float(split[0]) # spill time from WC controller
            diff = tgttime-sTime    # PADE read time - WC DAQ read time
            print "diff",diff
            if diff<0:        # Moved past the spill in the db file
                print "miss!"
                return (-1, None)
            if diff <= bound:   # fuzzy time match
                return( int(split[4]),split[3] )  # byte offset and filename

    except IOError as e:
        print "Failed to open file %s due to %s" % (filename, e)

    return (-1,None)


# WC Database lookup [old version]
# match WC spills w/in 15 seconds of timestamp given by PADE
def wcLookup_(tgttime, bound=15, filename="wcdb.txt"):
    lookval=long(tgttime)/1000   # seek matches w/in 1000 second time range
    try:
        stat,spills=getstatusoutput("look "+str(lookval)+" "+filename)   # binary search of file
        if (int(stat)!=0) or len(spills)==0: 
            return (-1, None)                # no lines match
        spills=spills.split("\n") 
        for spill in spills:   # search spills <100 seconds from time in PADE spill header 
            split = re.split(' +', spill.strip())
            sTime = float(split[0])
            diff = sTime-tgttime
            if abs(diff) <= bound:   # fuzzy time match
                return( int(split[4]),split[3] )  # byte offset and filename
            elif (diff>bound):       # Moved past the spill in the db file
                return (-1, None)

    except IOError as e:
        print "Failed to open file %s due to %s" % (filename, e)

    return (-1,None)

# WC Database lookup [older version]
# match WC spills w/in 15 seconds of timestamp given by PADE
def wcLookup__(tgttime, bound=15, filename="wcdb.txt"):
    tgttime=float(tgttime)
    try:
        handle = open(filename, 'r')        
        withinBound = []
        for line in handle:                       # todo: replace with binary search!
            split = re.split(' +', line.strip())
            sTime = float(split[0])
            diff = sTime-tgttime

            if abs(diff) <= bound:   # fuzzy time match
                return( int(split[4]),split[3] )  # byte offset and filename
            elif (diff>bound):       # Moved past the spill in the db file
                return (-1, None)

    except IOError as e:
        print "Failed to open file %s due to %s" % (filename, e)

    return (-1,None)




# find matching WC event number
def findWCEvent(fd,tgtevent):
    wcline=fd.readline()  # remove 1st line constaining SPILL number
    while(1):
        wcline=fd.readline()
        if not wcline or "SPILL" in wcline: return -1
        if "EVENT" in wcline:
            thisevent=int(wcline.split()[2])
            if thisevent-1==tgtevent: return fd.tell()  # WC/PADE events start at 1/0
            elif thisevent-1>tgtevent: return -1  # past the event number


def getTableXY(timeStamp):
    checkEnv("TBHOME","Source the setup script")
    tbhome=str(os.getenv("TBHOME"))
    posFile=tbhome+"/doc/TablePositions.txt"
    x=-999.0
    y=-999.0
    try:
        inFile=open(posFile, "r")
        for line in inFile:
            if line.find(timeStamp)>-1:
                line=line.split()
                nf=len(line)
                x=float(line[nf-2])
                y=float(line[nf-1])
    except IOError as e:
        print "Failed to open file %s due to %s" % (posFile, e)
    return (x,y)

def getRunData(timeStamp):
    with open('runlist.dat', 'r') as f:
        runlist = pickle.load(f)
    print "search for",timeStamp

    for run in runlist:  # could move to a binary search
        if run[0]==timeStamp:
            print run
            particle=run[3]
            try: 
                vga=int(run[4],16)
            except: 
                vga=0
            momentum=run[5].replace("GeV","")

            # table location
            try: 
                tableX=float(run[18])
            except: 
                tableX=-999.
            try: 
                tableY=float(run[19])
            except: 
                tableY=-999.
            # beam type
            pid=0
            if "elec" in particle: 
                pid=11
            elif "posi" in particle: 
                pid=-11
            elif "muo" in particle: 
                pid=12
            elif "pion" in paticle: 
                pid=211
            elif "prot" in particle: 
                pid=2212
            elif "las" in particle:  
                pid=-22

            # gain setting
            lna_pga=run[6]
            gain=10 # default is High_High
            if "Low_" in lna_pga:
                gain=gain-2
            if "Mid_" in lna_pga:
                gain=gain-1
            #if "High_" in lna_pga:
            #    gain=gain
            if "_low" in lna_pga:
                gain=gain-8
            if "_Mid" in lna_pga:
                gain=gain+4
            #if "_High" in lna_pga:
            #    gain=gain
            if "_VHigh" in lna_pga:
                gain=gain+4
            gain=gain+vga<<4

            return (pid,float(momentum),gain,float(tableX),float(tableY))
    return []

def lastRunDat():
    if not os.path.isfile('runlist.dat') : return "00000000_000000"
    with open('runlist.dat', 'r') as f:
        runlist = pickle.load(f)
    runs=len(runlist)
    last=runlist[runs-1][2].replace(".txt","").replace("rec_capture_","")
    return last


def dumpRunDat():
    with open('runlist.dat', 'r') as f:
        runlist = pickle.load(f)
    runs=len(runlist)
    for a in range(len(runlist)):
        print runlist[a]
