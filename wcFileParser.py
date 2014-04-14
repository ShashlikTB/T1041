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

        self.inSpill = False
        self.spills = []
        self.currentModule = None 
        self.currentSpill = None 
        self.currentEvent = None 
        try: 
            self.processFile()
        except Exception as e:
            logger.Warn("Couldn't process file %s" % e)



    def wcSpill(self, line):
        spill = re.split(' +', line.strip())
        if len(spill) == 2:
            if self.currentSpill:
                self.spills.append(currentSpill)
            self.currentSpill = {'id':spill[1],
                                 'events':[]}
        else:
            print "Bad Spill Line: %s" % line
            return None 


    def wcStartDate(self, line): 
        data = re.split(' +', line.strip())
        date = None
        if len(data) != 4: 
            logger.Warn("Bad start date: %s" % line)
        else: 
            date = time.strptime(' '.join(data[1:]), "%d %m %y")


        print date
        return date

    def wcStartTime(self, line):
        data = re.split(' +', line.strip())
        t = None
        if len(data) != 4:
            logger.Warn("Bad start time: %s" % line)
        else:
            t = time.strptime(' '.join(data[1:]), "%H %M %S")

        print t
        return t

    def wcTotalTime(self, line):
        data = re.split(' +', line.strip())
        datetime = None
        if len(data) != 3:
            logger.Warn("Bad total time: %s" % line)
        else:
            datetime = time.strptime(' '.join(data[1:]), "%d/%m/%y %H:%M:%S")

        print datetime
        return datetime

    def wcEvent(self, line):
        if self.currentEvent:
            # New Event 
            self.currentSpill['events'].append(self.currentEvent)


        data = re.split(' +', line.strip())
        wcrun = data[1]
        wcspill = data[2]

        self.currentEvent = {'modules':{},
                      'time':None,
                      'id':data[1], 
                      'spill':data[2]}


        

    def wcEventTime(self, line):
        data = re.split(' +', line.strip())
        self.currentEvent['time'] = (data[1], data[2])

        

    def wcModule(self, line):
        data = re.split(' +', line.strip())
        module = 0
        if len(data) != 2:
            logger.Warn("Bad Module Line: %s" % line)
        else:
            module = int(data[1])

        if self.currentModule:
            self.currentEvent['modules'][self.currentModule['id']] = self.currentModule
        
        self.currentModule = {'id':module,
                              'channels':{}}


    def wcChannel(self, line):
        data = re.split(' +', line.strip())
        channel = 0
        count = 0
        if len(data) != 3:
            logger.Warn("Bad channel Line: %s" % line)
        else:
            channel = data[1]
            count = data[2]
        
        self.currentModule['channels'][channel] = count

            

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




    def processFile(self): 
        if not self.wc:
            raise 

        for line in self.wc: 
            line = line.strip()
            data = re.split(' +', line)
            fn = self.wcMapper[data[0]]
            print "Executing %s" % data[0]
            fn(self,line)

        if self.currentSpill:
            self.spills.append(self.currentSpill)
        



    def processWireChamber(self): 
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
