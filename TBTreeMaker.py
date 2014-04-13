###############################################################################
#
# PADE and WC file format descriptions
# https://drive.google.com/folderview?id=0B4UcugGDz9LwSzlBaDA4TEQ4Z1k&usp=sharing
# https://cdcvs.fnal.gov/redmine/projects/ftbfwirechamberdaq/wiki/Processed_hit_data_description
#
# display integrated hits data for Shashlik calorimeter
# this is just a wrapper for the ROOT C++ code
# will replace w/ a simple GUI
# Created 04/12/2014 B.Hirosky: Initial release
###############################################################################

import os, re, glob, sys, array, getopt
from ROOT import *
from string import split
from array import array
from TBUtils import *

# Unify PADE blocks and WC blocks into one Ntuple by doing
# python FakeDataNtupleMaker.py rec_capture_20140324_134340.txt wc_data.txt t1041_20140320232645.dat

DEBUG_LEVEL = 0
NEventLimit = 1000000
MASTERID = 112

def usage():
    print
    print "Usage: python TBNtupleMaker [OPTION] PADE_FILE [WC_FILE]"
    print "      -n max_events  : Maximum (requested) number of events to read"
    print "                       Always read at least 1 spill"
    print 
    sys.exit()

logger=Logger(1)  # instantiate a logger, w/ 1 repetition of messages
#=======================================================================# 
#  Read in and characterize the input file                              #
#=======================================================================#
opts, args = getopt.getopt(sys.argv[1:], "n:")

for o, a in opts:
    if o == "-n":
        NEventLimit=int(a)
        logger.Info("Stop at end of spill after reading at least",NEventLimit,"events")

if len(args)==0: usage()
padeDat=args[0]
outFile=padeDat.replace(".bz2","").replace(".txt",".root")
fPade=TBOpen(padeDat)                   # open the PADE data file

haveWC=False
if len(args)>1: 
    wcDat=args[1]
    fWC = TBOpen(wcDat)
    haveWC=True
else:
    logger.Info("No WC file provided")


#=======================================================================# 
#  Declare data containers                                              #
#=======================================================================#
gROOT.ProcessLine(".L TBEvent.cc+")

#=======================================================================# 
#  Declare an element of the event class for our event                  #
#=======================================================================#
tbevent = TBEvent()
tbspill = TBSpill()
padeChannel = PadeChannel()
#=======================================================================# 
#  Declare new file and tree with branches                              #
#=======================================================================#
fout = TFile(outFile, "recreate")
logger.Info("Writing to output file",outFile)
BeamTree = TTree("t1041", "T1041")
BeamTree.Branch("tbevent", "TBEvent", AddressOf(tbevent), 64000, 0)

eventDict={} # dictionary holds event data for a spill, use event # as key
padeDict={}  # dictionary holds PADE header data for a spill, use PADE ID as key

def fillTree():
    logger.Info("Write spill",the_spill_number)
    for ievt in range(len(eventDict)):
        tbevent.cp(eventDict[ievt])     
        BeamTree.Fill()

lastEvent=-1
nSpills=0
nEventsInSpill=0
nEventsTot=0
lastBoardID=-1

fakeSpillData=False

# read PADE data file
while 1:
    padeline=fPade.readline().rstrip()
    if not padeline:  # end of file
        if len(eventDict)>0:   # fill events from last spill into Tree
            fillTree()
        break

    ###########################################################
    ########## Dealing with spill header information ##########

    if "fake" in padeline:
        fakeSpillData=True
        logger.Info("Fake spill data")
        continue

    if "starting spill" in padeline:   # new spill condition
        if len(eventDict)>0:   # fill events from last spill into Tree
            fillTree()
            if (nEventsTot>=NEventLimit): break
        logger.Info(padeline)
        eventDict={}
        boardHeaders={}
        lastEvent=-1
        newEvent=False
        nEventsInSpill=0

        (timestr,the_spill_pctime,the_spill_ts,the_spill_number)=ParsePadeSpillHeader(padeline)

        nSpills=nSpills+1;
        continue # read next line in PADE file

    if "spill status" in padeline:    # spill header for a PADE card
        (master,boardID,status,trgStatus,
         events,memReg,trigPtr,pTemp,sTemp) = ParsePadeHeader(padeline)
        boardHeaders[boardID]=PadeBoard(master,boardID,status,trgStatus,
                                        events,memReg,trigPtr,pTemp,sTemp)
        continue

    ########## Dealing with spill header information ########## 
    ###########################################################

    
    # parse PADE channel data
    (pade_ts,pade_transfer_size,pade_board_id,
     pade_hw_counter,pade_ch_number,eventNumber,waveform)=ParsePadeData(padeline)

    samples=array("i",[0xFFF]*padeChannel.__SAMPLES())
    nsamples=len(waveform)
    if nsamples != padeChannel.__SAMPLES():
        logger.Warn("Incorrect number of ADC samples, expected",
                    padeChannel.__SAMPLES(),"found:",nsamples,"Board:", pade_board_id)
    else:
        isSaturated = "FFF" in waveform
        if (isSaturated):
            logger.Warn("ADC shows saturation. Board:",pade_board_id,"channel:",pade_ch_number)
        for i in range(nsamples): samples[i]=int(waveform[i],16)

            
    # check for special channel conditions
    newMasterEvent=False
    newEvent=False
    if eventNumber!=lastEvent:
        newEvent=True
        if pade_board_id==MASTERID: newMasterEvent=True
    lastEvent=eventNumber
    
    # new board/event condition
    if pade_board_id != lastBoardID or newEvent:
        lastPacket=pade_hw_counter
    elif (pade_hw_counter-lastPacket) != 1:
        logger.Warn("Packet counter error. Board:",pade_board_id)
    lastPacket=pade_hw_counter

    writeChan=True     # assume channel is good to write, until proven guilty
    # new event condition in master
    if newMasterEvent:
        nEventsTot=nEventsTot+1
        nEventsInSpill=nEventsInSpill+1

        if eventNumber%100==0:
            print "Event in spill",the_spill_number,"(",eventNumber,")  / total", nEventsTot

        eventDict[eventNumber]=TBEvent()

        eventDict[eventNumber].SetSpill(the_spill_number)
#        eventDict[eventNumber].SetPCTime(long(the_spill_pctime))
#        eventDict[eventNumber].SetSpillTime(long(the_spill_ts))
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

    else: # new event in a slave
        if not eventNumber in eventDict:
            logger.Warn("Event count mismatch. Slave:",
                        pade_board_id,"reports",eventNumber-nEventsInSpill+1,
                        "extra events in the spill")
            writeChan=False

    if writeChan:
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
print
print "Finished processing"
BeamTree.Print()
print "writing file:",outFile
BeamTree.Write()

fout.Close() 

print
logger.Info("Summary: nSpills processed= "+str(nSpills)+" Total Events= "+str(nEventsTot))

logger.Summary()
logger.Info("Fake spill data")
print "Exiting" 
exit(0)
