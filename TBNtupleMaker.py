# Note: we assume the 1st PADE block in each event is always the MASTER

import os, re, glob, sys, array, getopt
import argparse 

from string import split
import time
from array import array
import bz2
from TBUtils import *
import re 


from wcFileParser import * 

# Argparse Definitions 

parser = argparse.ArgumentParser(description='Test Beam Ntuple Maker')
parser.add_argument('filename', metavar='filename', type=str, nargs=1, 
                    help="Filename to process")
parser.add_argument('-n', default=1, metavar='max_events', type=int, nargs=1, 
                    help="Maximum (requested) number of events to read, This will always read at least 1 spill")
parser.add_argument('--wc', type=str, nargs=1, 
                    help="WC File")

args = parser.parse_args()



## Root breaks normal argument parsing, so import it after we're done getting arguments


from ROOT import *

# Unify PADE blocks and WC blocks into one Ntuple by doing
# python FakeDataNtupleMaker.py rec_capture_20140324_134340.txt wc_data.txt t1041_20140320232645.dat

DEBUG_LEVEL = 0
NEventLimit = 1000000
MASTERID = 112

logger=Logger(1)  # instantiate a logger, w/ 1 repetition of messages


haveWC = False
if args.wc:
    fWC =  open(args.wc[0], "r")
    haveWC=True
else:
    logger.Info("No WC file provided")


#=======================================================================# 
#  Declare data containers                                              #
#=======================================================================#
gSystem.Load("./TBEvent.so")



class PadeChannel: 
    def __init__(self): 
        self.timestamp = 0
        self.transferSize = 0
        self.boardID = 0
        self.hwCounter = 0
        self.chNumber = 0
        self.eventNumber = 0
        self.samples = []
        self.flag = 0

    def __init__(self, ts, transferSize, bid, hwCounter, chNum, eventNum, samples):
        self.timestamp = ts
        self.transferSize = transferSize
        self.boardID = bid
        self.hwCounter = hwCounter
        self.chNumber = chNum
        self.eventNumber = eventNum
        self.samples = samples
        self.flag = 0


    def __repr__(self): 
        rep = '\r\n'.join(["timestamp: %r",
        "transferSize: %r",
        "boardID: %r",
        "hwCounter: %r",
        "chNumber: %r",
        "eventNumber: %r",
        "samples: %r"])
        return rep % (self.timestamp, self.transferSize, self.boardID, self.hwCounter,
                      self.chNumber, self.eventNumber, self.samples)


    


class PadeEvent:
    def __init__(self): 
        self.spillNumber = 0
        self.pcTime = 0
        self.spillTime = 0
        self.eventNumber = 0
        self.channels = {}
        self.wcChannels = {}

    def __init__(self, spillNumber, pcTime, spillTime, eventNumber): 
        self.spillNumber = spillNumber
        self.pcTime = pcTime
        self.spillTime = spillTime
        self.eventNumber = eventNumber
        self.channels = {}
        self.wcChannels = {}

    def AddChannel(pc): 
        try:
            self.channels[pc.chNumber].append(pc)
        except KeyError:
            self.channels[pc.chNumber] = []
            self.channels[pc.chNumber].append(pc)
            


def generateSpillDict(spillLine): 
    
    spill = {
        'nEvents':0,
        'events':{},
        'spillNumber':0,
        'pcTime':0,
        }
    
    try:
        if "WC" in spillLine:
            timestr=spillLine[spillLine.index('at')+3:spillLine.index('WC')].strip()
        else: timestr=spillLine[spillLine.index('at')+3:-1].strip()
        # time on PC
        spill['pcTime']= time.mktime(time.strptime(timestr, "%m/%d/%Y %H:%M:%S %p")) 
        spill['spillNumber'] = int(spillLine[spillLine.index('num')+4:spillLine.index('at')-5])

    except:
        print "Warning! problem generating spill dictionary from %s" % spillLine
    
    
    return spill


        
        
    
def filterNonMasterEvents(spills): 
    keysToRemove = []
    for spill in spills: 
        for key in spill['events']: 
            evt = spill['events'][key]
            if not evt.channels.has_key(MASTERID):
                logger.Warn("Found an excess slave board event: %s, Removing" % key)
                keysToRemove.append(key)

    for key in keysToRemove:
        del spill['events'][key]
        
            

def fillTree():
    logger.Info("Write spill",the_spill_number)
    for ievt in range(len(eventDict)):
        event.cp(eventDict[ievt])
        BeamTree.Fill()



def processPadeLine(line): 
    line = line.strip()
    data = line.split(' ')
    



    
    


def processPadeFile(filename): 
    fPade = None
    if filename.endswith(".txt"): 
        logger.Info("Txt File")
        fPade = open(filename, "r")
    elif filename.endswith(".bz2"): 
        fPade = bz2.BZ2File(filename, "r")
    if fPade is None: 
        logger.Warn("Could not open Pade file...Aborting!")
        exit()

    spills = []
    currentSpill = None
    lastPacket = 0
    lastEvent = -1
    totalEventCount = 0
    currEventCount = 0

    for line in fPade: 

        if line.find('spill status') != -1:
            #print and skip
            print line
        elif line.find('starting') != -1: 
            currEventCount = 0
            print 'Starting new spill: %s' % line
            
            if not currentSpill:
                currentSpill = generateSpillDict(line)
            else:
                spills.append(currentSpill)
                currentSpill = generateSpillDict(line)
        else:


            split = line.strip().split(' ')
            chLine = split[0:10]
            waveform = split[10:]
        
            pade_ts = long(chLine[0])
            pade_transfer_size = int(chLine[1]+chLine[2],16)
            pade_board_id = int(chLine[3],16)
            pade_hardware_counter = int(chLine[4]+chLine[5]+chLine[6],16)
            pade_channel = int(chLine[7],16)
            pade_event_number = int(chLine[8]+chLine[9],16)



            pc = PadeChannel(pade_ts, pade_transfer_size, pade_board_id, 
                             pade_hardware_counter, pade_channel, pade_event_number, 
                             waveform)

            try: 
                currentSpill['events'][pade_event_number].channels == None 
            except KeyError:
                currentSpill['events'][pade_event_number] = PadeEvent(0,0,0,pade_event_number)

            try: 
                currentSpill['events'][pade_event_number].channels[pade_board_id].append(pc)
            except:
                currentSpill['events'][pade_event_number].channels[pade_board_id] = []
                currentSpill['events'][pade_event_number].channels[pade_board_id].append(pc)
            currentSpill['nEvents'] = len(currentSpill['events'])


            if pade_event_number != lastEvent and pade_board_id == MASTERID: 
                lastEvent = pade_event_number
                print 'Event in spill %s ( %s ) / total %s' % (currentSpill['spillNumber'],currEventCount, totalEventCount)
                currEventCount += 1
                totalEventCount += 1
            

    spills.append(currentSpill)

##  add a hardware counter check 
    
    filterNonMasterEvents(spills)

    return spills
        
spills = processPadeFile(args.filename[0])

    #=======================================================================# 
    #  Declare new file and tree with branches                              #
    #=======================================================================#

outputFile = args.filename[0].replace(".txt", ".root")

if outputFile.endswith('.bz2'): 
    outputFile = outputFile.replace(".bz2", "")

fout = TFile(outputFile, "recreate")
logger.Info("Writing to output file",outputFile)


treeEvent = TBEvent()
BeamTree = TTree("BeamData", "BeamData")
BeamTree.Branch("event", "TBEvent", AddressOf(treeEvent), 64000, 0)



for spill in spills: 
    for key in spill['events']: 
        currEvent = TBEvent()
        currEvent.SetSpill(spill['spillNumber'])
        currEvent.SetPCTime(long(spill['pcTime']))
        currEvent.SetSpillTime(long(spill['pcTime']))


        evt = spill['events'][key]
        currEvent.SetEventNumber(evt.eventNumber)
        for ch in evt.channels: 
            channel = evt.channels[ch][0]


            samples=array("i")
            for val in channel.samples:
                samples.append(int(val,16))
            currEvent.FillPadeChannel(channel.timestamp, channel.transferSize, 
                                      channel.boardID, channel.hwCounter, 
                                      channel.chNumber, channel.eventNumber, samples)
        treeEvent.cp(currEvent)
        BeamTree.Fill()
        
            


BeamTree.Write()
fout.Close()


exit()







lastEvent=-1
nSpills=0
nEventsInSpill=0
nEventsTot=0
lastBoardID=-1
lastChannel=-1

eventDict={} # dictionary holds events in a spill, use event # as key

# read PADE data file


    


while 1:
    padeline=fPade.readline()
    if not padeline:  # end of file
        if len(eventDict)>0:   # fill events from last spill into Tree
            fillTree()
        break

    # new spill condition
    if "starting spill" in padeline: 
        if len(eventDict)>0:   # fill events from last spill into Tree
            fillTree()
            if (nEventsTot>=NEventLimit): break
        logger.Info(padeline)
        eventDict={}
        lastEvent=-1
        nEventsInSpill=0
        if "WC" in padeline:
            timestr=padeline[padeline.index('at')+3:padeline.index('WC')].strip()
        else: timestr=padeline[padeline.index('at')+3:-1].strip()
        the_spill_pctime = time.mktime(time.strptime(timestr, "%m/%d/%Y %H:%M:%S %p"))  # time on PC
        the_spill_ts =  the_spill_pctime              # time on WC controller (temporary)  UPDATE ME!
        the_spill_number = int(padeline[padeline.index('num')+4:padeline.index('at')-5])
        nSpills=nSpills+1;
        continue # read next line in PADE file

    # unpack PADE channel line
    padeline=padeline.split()
    pade_ts=long(padeline[0])
    pade_transfer_size=int(padeline[1]+padeline[2],16)
    pade_board_id=int(padeline[3],16)
    pade_hw_counter=int(padeline[4]+padeline[5]+padeline[6],16)
    pade_ch_number=int(padeline[7],16)
    eventNumber = int(padeline[8]+padeline[9],16)

    # new board/channel condition
    if pade_board_id != lastBoardID or pade_ch_number != lastChannel:
        lastBoardID=pade_board_id
        lastChannel=pade_ch_number
        lastPacket=pade_hw_counter-1
    if (pade_hw_counter-lastPacket) != 1:
        logger.Warn("Packet counter error. Board:",pade_board_id)
    lastPacket=pade_hw_counter

    # new event condition in master
    if pade_board_id==MASTERID and eventNumber!=lastEvent: 
        lastEvent=eventNumber    # last event in master
        nEventsTot=nEventsTot+1
        nEventsInSpill=nEventsInSpill+1
        print "Event in spill",the_spill_number,"(",eventNumber,")  / total", nEventsTot

        eventDict[eventNumber]=TBEvent()
        eventDict[eventNumber].SetSpill(the_spill_number)
        eventDict[eventNumber].SetPCTime(long(the_spill_pctime))
        eventDict[eventNumber].SetSpillTime(long(the_spill_ts))
        eventDict[eventNumber].SetEventNumber(eventNumber)

        # read WC data (hack for now)
        foundWC=False;
        if haveWC:
            # read one WC event 
            endOfEvent=0
            nhits=0;
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
                     eventDict[eventNumber].AddWCHit(tdcNum,wire,tdcCount)
                     endOfEvent=fWC.tell()
                     if DEBUG_LEVEL>1: event.GetWCChan(nhits).Dump()
                     nhits=nhits+1

    else: # not new event condition
        if not eventNumber in eventDict:
            logger.Warn("Event count mismatch",
                        "Event",eventNumber,"not present in PADE master. Board:",
                        pade_board_id,"event:",eventNumber,
                        "Last in master:",nEventsInSpill-1)
            continue  # skip this extra event in the PADE slave

    # continuation of channel line unpacking                 
    waveform=(padeline[10:])    
    nsamples=len(waveform)        # need error checking here
    if nsamples != padeChannel.__SAMPLES():
        logger.Warn("Incorrect number of ADC samples, expected",
                    padeChannel.__SAMPLES(),"found:",nsamples)
    samples=array("i")
    for val in waveform:
        samples.append(int(val,16))

    eventDict[eventNumber].FillPadeChannel(pade_ts, pade_transfer_size, pade_board_id, 
                                           pade_hw_counter, pade_ch_number, 
                                           eventNumber, samples)
    if DEBUG_LEVEL>1: eventDict[eventNumber].GetLastPadeChan().Dump()
    pade_ts=0
    pade_transfer_size=0
    pade_board_id=0
    pade_hw_counter=0 
    pade_ch_number=0
    eventNumber=0

            
#=======================================================================# 
#  Write tree and file to disk                                          #
#=======================================================================#
print "writing file: outputNtuple.root"
BeamTree.Write()

print "closing file: outputNtuple.root"
fout.Close() 

print
logger.Info("Summary: nSpills = "+str(nSpills)+" Total Events= "+str(nEventsTot))

logger.Summary()

print "Exiting" 
exit(0)
