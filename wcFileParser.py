import bz2
import re
import time
from TBUtils import * 


logger=Logger(1)  # instantiate a logger, w/ 1 repetition of messages


class wcParser:
    def __init__(self):
        pass
    
    def __init__(self, filename):
        self.wc = None
        self.filename = filename
        try:
            if filename.endswith(".txt") or filename.endswith(".dat"):
                self.wc = open(filename, "r")
            elif filename.endswith(".bz2"):
                self.wc = bz2.BZ2File(filename, "r")
            else:
                logger.Warn('Unrecognized filename extension')
        except IOError as e:
            logger.Warn("Unable to open %s, %s" % (filename, e))

        
        self.spills = []
        self.inSpill = False


        


    

def wcSpill(line):
    spill = re.split(' +', line.strip())
    if len(spill) == 2:
        return spill[1]
    else:
        print "Bad Spill Line: %s" % line
        return None 


def wcStartDate(line): 
    data = re.split(' +', line.strip())
    date = None
    if len(data) != 4: 
        logger.Warn("Bad start date: %s" % line)
    else: 
        date = time.strptime(' '.join(data[1:]), "%d %m %y")


    print date
    return date

def wcStartTime(line):
    data = re.split(' +', line.strip())
    t = None
    if len(data) != 4:
        logger.Warn("Bad start time: %s" % line)
    else:
        t = time.strptime(' '.join(data[1:]), "%H %M %S")

    print t
    return t

def wcTotalTime(line):
    data = re.split(' +', line.strip())
    datetime = None
    if len(data) != 3:
        logger.Warn("Bad total time: %s" % line)
    else:
        datetime = time.strptime(' '.join(data[1:]), "%d/%m/%y %H:%M:%S")

    print datetime
    return datetime

def wcEvent(line):
    data = re.split(' +', line.strip())
    wcrun = data[1]
    wcspill = data[2]

    return (wcrun, wcspill)

def wcEventTime(line):
    data = re.split(' +', line.strip())
    
    return None

def wcModule(line):
    data = re.split(' +', line.strip())
    module = 0
    if len(data) != 2:
        logger.Warn("Bad Module Line: %s" % line)
    else:
        module = int(data[1])
    return module


def wcChannel(line):
    data = re.split(' +', line.strip())
    channel = 0
    count = 0
    if len(data) != 3:
        logger.Warn("Bad channel Line: %s" % line)
    else:
        channel = data[1]
        count = data[2]

    return (channel, count)

wcMapper = { 
    'SPILL':wcSpill,
    'SDATE':wcStartDate,
    'STIME':wcStartTime,
    'TTIME':wcTotalTime,
    'EVENT':wcEvent,
    'ETIME':wcEventTime,
    'Module':wcModule,
    'Channel':wcChannel,
}




def processWireChamber(filename): 
    wc = None
    try:
        if filename.endswith(".txt") or filename.endswith(".dat"):
            wc = open(filename, "r")
        elif filename.endswith(".bz2"):
            wc = bz2.BZ2File(filename, "r")
        else:
            logger.Warn('Unrecognized filename extension')
            return None
    except IOError as e:
        logger.Warn("Unable to open %s, %s" % (filename, e))
        raise

    chamberHits = {}
    for line in wc: 
        line = line.strip()
        data = re.split(' +', line)
        fn = wcMapper[data[0]]
        fn(line)


    return chamberHits
