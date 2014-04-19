###############################################################################
#
# PADE and WC file format descriptions
# https://drive.google.com/folderview?id=0B4UcugGDz9LwSzlBaDA4TEQ4Z1k&usp=sharing
# https://cdcvs.fnal.gov/redmine/projects/ftbfwirechamberdaq/wiki/Processed_hit_data_description
#
# Created 04/12/2014 B.Hirosky: Initial release
###############################################################################

import os, re, glob, sys, array, getopt, commands
from ROOT import *
from string import split
from array import array
from TBUtils import *

# Unify PADE blocks and WC blocks into one Ntuple by doing
# python FakeDataNtupleMaker.py rec_capture_20140324_134340.txt wc_data.txt t1041_20140320232645.dat

DEBUG_LEVEL = 0
NEventLimit = 1000000
MASTERID = 112
MAXPERSPILL=126

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
#gSystem.Load("./TBEvent.so")   # for use w/ Makefile

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
    if len(eventDict)==0: return
    nfill=min(len(eventDict),MAXPERSPILL)
    for ievt in range(nfill):
        if ievt in eventDict:
            tbevent.cp(eventDict[ievt])     
            BeamTree.Fill()
        else: logger.Warn("Skip write of missing event in master",ievt,"of",len(eventDict))

lastBoardID=-1
lastEvent=-1
nSpills=0
nEventsInSpill=0
nEventsTot=0
skipToNextSpill=False
skipToNextBoard=False

fakeSpillData=False

writevent=True

# read PADE data file
while 1:
    padeline=fPade.readline().rstrip()
    if not padeline: 
        fillTree()          # end of file
        break

    ###########################################################
    ############### Reading spill header information ##########

    if "fake" in padeline:
        fakeSpillData=True
        logger.Info("Fake spill data")
        continue

    if "starting spill" in padeline:   # new spill condition
        fillTree()
        if (nEventsTot>=NEventLimit): break
        logger.Info(padeline)
        eventDict={}           # clear dictionary containing events in spill
        boardHeaders={}
        lastBoardID=-1
        lastEvent=-1
        newEvent=False
        skipToNextSpill=False
        skipToNextBoard=False
        nEventsInSpill=0

        if padeline.endswith("time ="):
            logger.Warn("Spill header error detected: WC time stamp missing")

        padeSpill=ParsePadeSpillHeader(padeline)
        nSpills=nSpills+1;
        if padeSpill['status']<0:
            skipToNextSpill=True
            logger.Warn("Spill header error detected: Invalid WC time stamp")
            continue
        
        # find associated spill in WC data
        wcSpill=wcLookup(padeSpill['wcTime'])
        if (wcSpill[0]>=0):
            logger.Info("WC data from file:",wcSpill[1])
        else:
            logger.Warn("No corresponding WC data found for spill")

        continue                     # finished spill header read next line in PADE file
    

    if skipToNextSpill: continue     # begin reading at next spill header (trigger by certain errors)

    if "spill status" in padeline:   # spill header for a PADE card
        (isMaster,boardID,status,trgStatus,
         events,memReg,trigPtr,pTemp,sTemp) = ParsePadeHeader(padeline)
        boardHeaders[boardID]=PadeBoard(isMaster,boardID,status,trgStatus,
                                        events,memReg,trigPtr,pTemp,sTemp)
        continue

    ############### Reading spill header information ########## 
    ###########################################################
    
    # parse PADE channel data
    (pade_ts,pade_transfer_size,pade_board_id,
     pade_hw_counter,pade_ch_number,padeEvent,waveform)=ParsePadeData(padeline)


    # new board/event conditions
    newBoard =  (pade_board_id != lastBoardID)
    newEvent = (padeEvent!=lastEvent)
    newMasterEvent = (pade_board_id==MASTERID and newEvent)

    if newBoard: 
        lastBoardID=pade_board_id
        lastEvent=-1
        skipToNextBoard=False
    if skipToNextBoard: continue

    # check for event overflows
    if padeEvent>MAXPERSPILL:
        logger.Warn("Event count overflow in spill, reading 1st",MAXPERSPILL,"events")
        skipToNextBoard=True
        continue

    # check for sequential events
    if newEvent and (padeEvent-lastEvent)!=1:
        #logger.Warn("Nonsequential event #:",padeEvent,"Expected",lastEvent+1,
        #            " Board:",pade_board_id,"channel:",pade_ch_number,"Clearing events in dictionary")
        logger.Warn("Nonsequential event #, delta=",padeEvent-lastEvent,
                    " Board:",pade_board_id,"channel:",pade_ch_number," Clearing events in dictionary")
        skipToNextBoard=True     # give up on remainder of this spill
        for ievt in range(lastEvent,len(eventDict)):
            if ievt in eventDict: del eventDict[ievt]    # remove incomplete event and all following
        continue
    lastEvent=padeEvent

    # check packet counter
    goodPacketCount = (newBoard or newEvent) or (pade_hw_counter-lastPacket)==1
    if not goodPacketCount:
        logger.Warn("Packet counter increment error, delta=",
                    pade_hw_counter-lastPacket," Board:",pade_board_id,"channel:",pade_ch_number,
                    "Clearing events in dictionary")
        for ievt in range(lastEvent,len(eventDict)):
            if ievt in eventDict: del eventDict[ievt]    # remove incomplete event and all following
        skipToNextBoard=True
        continue
    lastPacket=pade_hw_counter

    # fetch ADC samples (to do clear event from here on error)
    samples=array("i",[0xFFF]*padeChannel.__SAMPLES())
    nsamples=len(waveform)
    if nsamples != padeChannel.__SAMPLES():
        logger.Warn("Incorrect number of ADC samples, expected",
                    padeChannel.__SAMPLES(),"found:",nsamples,"Board:", pade_board_id)
        continue
    else:
        isSaturated = "FFF" in waveform
        if (isSaturated):
            logger.Warn("ADC shows saturation. Board:",pade_board_id,"channel:",pade_ch_number)
        for i in range(nsamples): 
            samples[i]=int(waveform[i],16)
            if (samples[i]>4095):
                logger.Warn("Invalid ADC reading > 0xFFF", pade_board_id,"channel:",pade_ch_number)

    

    writeChan=True     # now assume channel is good to write, until proven guilty
    # new event condition in master
    if newMasterEvent:
        nEventsTot=nEventsTot+1
        nEventsInSpill=nEventsInSpill+1
        if padeEvent%100==0:
            print "Event in spill",padeSpill['number'],"(",padeEvent,")  / total", nEventsTot

        eventDict[padeEvent]=TBEvent()
        eventDict[padeEvent].SetSpill(padeSpill['number'])
        eventDict[padeEvent].SetNtrigWC(padeSpill['nTrigWC'])
        

        # search for WC spill info
        if wcSpill[0]>=0:
            fWC = TBOpen(wcSpill[1])
            fWC.seek(int(wcSpill[0]))
#            wcline=fWC.readline()  # remove 1st line constaining SPILL number

            wcStart=findWCEvent(fWC,padeEvent)  # advance file pointer and return location of event 
            if wcStart>0:                       # matching event found in WC data
                fWC.seek(wcStart)
                etime=fWC.readline()            # discard ETIME line
                while 1:
                    wcline=fWC.readline().split()
                    if "Module" in wcline[0]: tdcNum=int(wcline[1])
                    elif "Channel" in wcline[0]:
                        wire=int(wcline[1])
                        tdcCount=int(wcline[2])
                        eventDict[padeEvent].AddWCHit(tdcNum,wire,tdcCount)
                    else: break
            else:
                logger.Warn("No matching event in WC")
            fWC.close()

    else: # new event in a slave
        if not padeEvent in eventDict:
#            logger.Warn("Event count mismatch. Slave:",
#                        pade_board_id,"reports event",padeEvent,"not present in master.",
#                        "Total events in master:",nEventsInSpill)
            logger.Warn("Event count mismatch. Slave:",
                        pade_board_id,"reports event not present in master.")
            writeChan=False
            skipToNextBoard=True

    if writeChan:
        eventDict[padeEvent].FillPadeChannel(pade_ts, pade_transfer_size, pade_board_id, 
                                               pade_hw_counter, pade_ch_number, 
                                               padeEvent, samples)
        if DEBUG_LEVEL>1: eventDict[padeEvent].GetLastPadeChan().Dump()


            
#=======================================================================# 
#  Write tree and file to disk                                          #
#=======================================================================#
print
print "Finished processing"
BeamTree.Print()
eventsInTree=BeamTree.GetEntries()
print "writing file:",outFile
BeamTree.Write()
fout.Close()

# for convinence when working interactively
print commands.getoutput(ccat('ln -sf',outFile,' latest.root'))

print
logger.Info("Summary: nSpills processed= ",nSpills," Total Events Processed= ",nEventsTot)
logger.Info("Fraction of events kept:",float(eventsInTree)/nEventsTot*100)

logger.Summary()
if fakeSpillData: logger.Info("Fake spill data")
print "Exiting" 
exit(0)
