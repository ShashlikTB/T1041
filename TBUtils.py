import sys
import re 
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


#WC Database lookup
#Simple, Naive version 
def wcLookup(unixtime, bound, filename):
    try:
        handle = open(filename, 'r')
        
        withinBound = []
        for line in handle:
            split = re.split(' +', line.strip())
            eTime = int(float(split[0]))
            diff = int(unixtime)-eTime

            if abs(diff) < bound: 
                withinBound.append((eTime, split[3], split[4]))
            elif split[0] == str(unixtime):
                return (split[3], int(split[4]))
            elif (eTime-unixtime) > bound:
                #Moved past the event in the db file
                break
        bestMatch = None 
        best = 999999
        for entry in withinBound:
            # find the closest match, in the case of a tie lowest first wins
            diff = abs(entry[0]-int(unixtime))
            if diff < best: 
                bestMatch = entry
                best = diff

        return bestMatch



    except IOError as e:
        print "Failed to open file %s due to %s" % (filename, e)
            

    return None 

