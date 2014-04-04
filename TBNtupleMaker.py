import os, re, glob, sys, array
from ROOT import *
from string import split
import time
from array import array
import bz2
from TBUtils import *

# Unify PADE blocks and WC blocks into one Ntuple by doing
# python FakeDataNtupleMaker.py rec_capture_20140324_134340.txt wc_data.txt t1041_20140320232645.dat

DEBUG_LEVEL=0
NEventLimit = 500

#=======================================================================# 
#  Declare data containers                                              #
#=======================================================================#

gROOT.ProcessLine(".L TBEvent.cc+")

#=======================================================================# 
#  Declare an element of the event class for our event                  #
#=======================================================================#
event = TBEvent()
padeChannel = PadeChannel()


#=======================================================================# 
#  Declare new file and tree with branches                              #
#=======================================================================#
fout = TFile("outputNtuple.root", "recreate")
#print "channel is "+ str(pade_channel.channel)
#pade_channel_.channel = 3
BeamTree = TTree("BeamData", "BeamData")
BeamTree.Branch("event", "TBEvent", AddressOf(event), 64000, 0)

#=======================================================================# 
#  Read in and characterize the input file                              #
#=======================================================================#
filelist = glob.glob(sys.argv[1])
listsize = len(filelist)
#print "number of input files: " + str(listsize)

for ifile in range(0,len(filelist)):
    print filelist[ifile]
print "="*20

padeDat=sys.argv[1]
if padeDat.endswith("bz2") : fPade = bz2.BZ2File(padeDat,"r")
else : fPade = open(sys.argv[1], "r")
haveWC=False
if len(sys.argv)>2 : 
    fWC =  open(sys.argv[2], "r")
    haveWC=True

lastEvent=-1
lastSpill=-1
nSpills=0
nEventsInSpill=0
nEventsTot=0
nPadeChannels=-1


# read pade data file
while 1:
    padeline=fPade.readline()
    if not padeline:  # end of file
        if (nEventsTot>0) : BeamTree.Fill()  # write final event
        break

    # new spill condition
    if "starting spill" in padeline: 
        lastEvent=-1
        print padeline
        timestr=padeline[padeline.index('at')+3:-1].strip()
        the_spill_pctime = time.mktime(time.strptime(timestr, "%m/%d/%Y %H:%M:%S %p"))  # time on PC
        the_spill_ts = the_spill_pctime                                                 # time on WC controller (temporary)
        the_spill_number = int(padeline[padeline.index('num')+4:padeline.index('at')-5])
        lastSpill=the_spill_number
        nSpills=nSpills+1;
        event.SetSpill(the_spill_number)
        event.SetPCTime(long(the_spill_pctime))
        event.SetSpillTime(long(the_spill_ts))
        # should begin by finding the WC spill, do "fseek"
        continue

    # unpack PADE channel line
    padeline=padeline.split()
    pade_ts=long(padeline[0])
    pade_transfer_size=int(padeline[1]+padeline[2],16)
    pade_board_id=int(padeline[3],16)
    pade_hw_counter=int(padeline[4]+padeline[5]+padeline[6],16)
    pade_ch_number=int(padeline[7],16)
    eventNumber = int(padeline[8]+padeline[9],16)

    # new event condition
    if eventNumber != lastEvent:  # new event condition
        if (nEventsTot>0) : BeamTree.Fill() # write previous event
        if (nEventsTot==NEventLimit) : break 
        event.ResetData()
        nPadeChannels=0;          # counter channels found in this event
        foundWC=False;
        lastEvent=eventNumber
        event.SetEventnumber(eventNumber)
        nEventsTot=nEventsTot+1
        nEventsInSpill=nEventsInSpill+1
        print "Event in spill",the_spill_number,"(",eventNumber,")  / total", nEventsTot
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
                     event.AddWCHit(tdcNum,wire,tdcCount)
                     endOfEvent=fWC.tell()
                     if DEBUG_LEVEL>1: event.GetWCChan(nhits).Dump()
                     nhits=nhits+1

    # continuation of channel line unpacking                 
    waveform=(padeline[10:])    
    nsamples=len(waveform)        # need error checking here
    if nsamples != padeChannel.__SAMPLES():
        WARN("Incorrect number of ADC samples, expected",
             padeChannel.__SAMPLES(),"found:",nsamples)
    samples=array("i")
    for val in waveform:
        samples.append(int(val,16))

    event.FillPadeChannel(pade_ts, pade_transfer_size, pade_board_id, 
                          pade_hw_counter, pade_ch_number, eventNumber, samples)
    if DEBUG_LEVEL>1: event.GetPadeChan(nPadeChannels).Dump()
    nPadeChannels=nPadeChannels+1;


            
#=======================================================================# 
#  Write tree and file to disk                                          #
#=======================================================================#
print "writing file: outputNtuple.root"
BeamTree.Write()

print "closing file: outputNtuple.root"
fout.Close() 


INFO("Summary: nSpills = "+str(nSpills)+" Total Events= "+str(nEventsTot))

print "Exiting" 
exit(0)
