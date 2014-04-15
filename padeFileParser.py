import bz2
import re
import time
from TBUtils import * 


logger=Logger(1)  # instantiate a logger, w/ 1 repetition of messages

class padeParser:
    statusMessage = 'spill status'
    def __init__(self): 
        pass

    def __init__(self, filename): 
        self.currentSpill = None
        self.spills = []
        self.filename = filename
        self.pade = None 

        self.totalEventCount = 0
        self.currentEventCount = 0
        self.lastEvent = -1
        self.MASTERID = 112 

        try:
            if filename.endswith(".txt") or filename.endswith(".dat"):
                self.pade = open(filename, "r")
            elif filename.endswith(".bz2"):
                self.pade = bz2.BZ2File(filename, "r")
            else:
                logger.Warn('Unrecognized filename extension')
        except IOError as e:
            logger.Warn("Unable to open %s, %s" % (filename, e))


        try:
            self.processFile()
        except Exception as e:
            logger.Warn("Couldn't process file %s" % e)


    def filterNonMasterEvents(self): 
        keysToRemove = []
        for spill in self.spills: 
            for key in spill['events']: 
                evt = spill['events'][key]
                if not evt['boards'].has_key(self.MASTERID):
                    logger.Warn("Found an excess slave board event: %s, Removing" % key)
                    keysToRemove.append(key)

        for key in keysToRemove:
            del spill['events'][key]

    def processDataLine(self, line): 

        try: 
            split = line.strip().split(' ')

            chLine = split[0:10]
            waveform = split[10:]
            # unpack PADE channel line

            pade_ts = long(chLine[0])
            pade_transfer_size = int(chLine[1]+chLine[2],16)
            pade_board_id = int(chLine[3],16)
            pade_hardware_counter = int(chLine[4]+chLine[5]+chLine[6],16)
            pade_channel = int(chLine[7],16)
            pade_event_number = int(chLine[8]+chLine[9],16)

            channel =  {
                'id':pade_event_number,
                'ts':pade_ts,
                'transfer_size':pade_transfer_size,
                'board_id':pade_board_id,
                'harware_counter':pade_hardware_counter,
                'channel':pade_channel
                }
            try:
                self.currentSpill['events'][pade_event_number] == None 
            except KeyError:
                self.currentSpill['events'][pade_event_number] = {
                    'id':pade_event_number,
                    'boards':{}
                    }
            try:
                self.currentSpill['events'][pade_event_number]['boards'][pade_board_id].append(channel)
            except KeyError: 
                self.currentSpill['events'][pade_event_number]['boards'][pade_board_id] = []
                self.currentSpill['events'][pade_event_number]['boards'][pade_board_id].append(channel)  
              


            self.currentSpill['nEvents'] = len(self.currentSpill['events'])

            if pade_event_number != self.lastEvent and pade_board_id == self.MASTERID: 
                self.lastEvent = pade_event_number
                print 'Event in spill %s ( %s ) / total %s' % (self.currentSpill['id'],self.currentEventCount, self.totalEventCount)
                self.currentEventCount += 1
                self.totalEventCount += 1
        

        except Exception as e:
            logger.Warn("Failed to process file due to error: %s" % e)
                        
            if self.currentSpill is not None:
                self.spills.append(self.currentSpill)




    def processFile(self): 

        for line in self.pade: 
            if line.find(self.statusMessage) != -1: 
                print line

            elif line.find('starting') != -1: 
                currEventCount = 0
                print 'Starting new spill: %s' % line
    
                if self.currentSpill is not None: 
                    self.spills.append(self.currentSpill)
                    self.currentEventCount = 0

                self.currentSpill = {'nEvents':0,
                                    'events':{},
                                    'id':-1,
                                    'pcTime':-1}
                try:
                    if "WC" in line:
                        timestr=line[line.index('at')+3:line.index('WC')].strip()
                    else: timestr=line[line.index('at')+3:-1].strip()
                        # time on PC
                    self.currentSpill['pcTime']= time.mktime(time.strptime(timestr, "%m/%d/%Y %H:%M:%S %p")) 
                    self.currentSpill['id'] = int(line[line.index('num')+4:line.index('at')-5])

                except:
                    print "Warning! problem generating spill dictionary from %s" % line
                    
            else: 
                #Normal Pade Data Line 
                self.processDataLine(line)
                    


        if self.currentSpill:
            self.spills.append(self.currentSpill)

        logger.Info("Filtering bad events")
        self.filterNonMasterEvents()
            

        
