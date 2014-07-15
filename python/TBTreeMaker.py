###############################################################################
#
# PADE and WC file format descriptions
# https://drive.google.com/folderview?id=0B4UcugGDz9LwSzlBaDA4TEQ4Z1k&usp=sharing
# https://cdcvs.fnal.gov/redmine/projects/ftbfwirechamberdaq/wiki/Processed_hit_data_description
#
# Created 04/12/2014 B.Hirosky: Initial release
# 4/30/2014: BH big cleanup and new command line args
# 7/1/2014: BH - read table positions file, if present
###############################################################################

import os, re, glob, sys, getopt, commands
import cProfile, pstats, StringIO
from ROOT import *
from string import split
from array import array
from TBUtils import *

##### Parameter block #####

DEBUG_LEVEL = 0
NMAX = 1000000   # stop after NMAX events
MASTERID = 112   
MAXPERSPILL=126  # do not process more that this many events per spill ( mem overwrite issue )

###########################

def usage():
    print
    print "Usage: python TBNtupleMaker [OPTION] [PADE_FILE] [PADE_FILE] ..."
    print "      -n max_events  : Maximum (requested) number of events to read"
    print "                       Always read at least 1 spill"
    print "      -d DIR         : Process all padefiles in DIR"
    print "                       Overrides all files given on command line list"
    print "      -r DIR         : Process all padefiles in DIR, and all subdirectories"
    print "                       Overrides all files given on command line list"
    print "      -k             : Keep existing root files, only process new inputs"
    print "      -o DIR         : Output dir, instead of default = location of input file" 
    print 
    sys.exit()



def fillTree(tree, eventDict, tbspill):  
    if len(eventDict)==0: return
    nfill=min(len(eventDict),MAXPERSPILL)
    tree[0].SetBranchAddress("tbspill",AddressOf(tbspill))
    for ievt in range(nfill):
        if ievt in eventDict:
            tree[0].SetBranchAddress("tbevent",AddressOf(eventDict[ievt]))            
            tree[0].Fill()


def filler(padeDat, NEventLimit=NMAX, keepFlag=False, outDir=""):

    logger=Logger(1)  # instantiate a logger, w/ 1 repetition of messages

    #=======================================================================# 
    #  Declare an element of the event class for our event                  #
    #=======================================================================#
    tbevent = TBEvent()
    tbspill = TBSpill()
    padeChannel = PadeChannel()

    #=======================================================================# 
    #  Declare new file and tree with branches                              #
    #=======================================================================#
    outFile=padeDat.replace(".bz2","").replace(".txt",".root")
    if not outDir=="":
        outFile=outDir+"/"+os.path.basename(outFile)

    if  os.path.isfile(outFile) and keepFlag:
        logger.Info(outFile,"is present, skip processing due to -k flag")
        return

    timeStamp=os.path.basename(outFile).replace("rec_capture_","").replace(".root","")
    tableX,tableY=getTableXY(timeStamp)
    logger.Info("Table position:",tableX,tableY)
    

    fout = TFile(outFile+"_tmp", "recreate")   # write to tmp file, rename at successful close

    logger.Info("Writing to output file",outFile)

    BeamTree = [TTree("t1041", "T1041")] # ugly python hack to pass a reference
    BeamTree[0].Branch("tbevent", "TBEvent", AddressOf(tbevent), 64000, 0)
    BeamTree[0].Branch("tbspill", "TBSpill", AddressOf(tbspill), 64000, 0)


    if (NEventLimit<NMAX):
        logger.Info("Stop at end of spill after reading at least",NEventLimit,"events")
    fPade=TBOpen(padeDat)                   # open the PADE data file


    eventDict={} # dictionary holds event data for a spill, use event # as key
    padeDict={}  # dictionary holds PADE header data for a spill, use PADE ID as key

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
            fillTree(BeamTree,eventDict,tbspill)          # end of file
            break

        ###########################################################
        ############### Reading spill header information ##########

        if "fake" in padeline:
            fakeSpillData=True
            logger.Info("Fake spill data")
            continue

        if "starting spill" in padeline:   # new spill condition
            fillTree(BeamTree,eventDict,tbspill) 
            if (nEventsTot>=NEventLimit): break
            tbspill.Reset();
            logger.Info(padeline)
            eventDict={}           # clear dictionary containing events in spill
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
            tbspill.SetSpillData(padeSpill['number'],int(padeSpill['pctime']),
                                 padeSpill['nTrigWC'],int(padeSpill['wcTime']),
                                 0,0,tableX,tableY)

            # find associated spill in WC data
            wcSpill=wcLookup(padeSpill['wcTime'])
            if (wcSpill[0]>=0):
                logger.Info("WC data from file:",wcSpill[1])
            else:
                logger.Warn("No corresponding WC data found for spill")

            continue  # finished w/ spill header read next line in PADE file

        # begin reading at next spill header (triggered by certain errors)
        if skipToNextSpill: continue    

        if "spill status" in padeline:   # spill header for a PADE card
            (isMaster,boardID,status,trgStatus,
             events,memReg,trigPtr,pTemp,sTemp) = ParsePadeBoardHeader(padeline)
            tbspill.AddPade(PadeHeader(isMaster,boardID,status,trgStatus,
                                            events,memReg,trigPtr,pTemp,sTemp))
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
            logger.Warn("Event count overflow in spill, reading 1st",
                        MAXPERSPILL,"events")
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
                logger.Warn("ADC shows saturation. Board:",
                            pade_board_id,"channel:",pade_ch_number)
            for i in range(nsamples): 
                samples[i]=int(waveform[i],16)
                if (samples[i]>4095):
                    logger.Warn("Invalid ADC reading > 0xFFF", 
                                pade_board_id,"channel:",pade_ch_number)

        writeChan=True   # now assume channel is good to write, until proven guilty
        # new event condition in master
        if newMasterEvent:
            nEventsTot=nEventsTot+1
            nEventsInSpill=nEventsInSpill+1
            if padeEvent%100==0:
                print "Event in spill",padeSpill['number'],"(",padeEvent,")  / total", nEventsTot

            eventDict[padeEvent]=TBEvent()

            # search for WC spill info
            if wcSpill[0]>=0:
                fWC = TBOpen(wcSpill[1])
                fWC.seek(int(wcSpill[0]))
                # advance file pointer and return location of event 
                wcStart=findWCEvent(fWC,padeEvent)  
                if wcStart>0:         # matching event found in WC data
                    fWC.seek(wcStart)
                    etime=fWC.readline()            # discard ETIME line
                    while 1:
                        wcline=fWC.readline()
                        if not wcline : break   # EOF 
                        wcline=wcline.split()
                        if len(wcline)<2: logger.Warn("Error in line from WC file:",wcline)
                        if "Module" in wcline: tdcNum=int(wcline[1])
                        elif "Channel" in wcline:
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
            eventDict[padeEvent].FillPadeChannel(pade_ts, pade_transfer_size, 
                                                 pade_board_id, pade_hw_counter, 
                                                 pade_ch_number, padeEvent, samples)
            if DEBUG_LEVEL>1: eventDict[padeEvent].GetLastPadeChan().Dump()


    #=======================================================================# 
    #  Write tree and file to disk                                          #
    #=======================================================================#
    print
    print "Finished processing"
    BeamTree[0].Print()
    eventsInTree=BeamTree[0].GetEntries()
    print "writing file:",outFile
    BeamTree[0].Write()
    fout.Close()
    commands.getoutput("mv -f "+outFile+"_tmp "+outFile)

    # for convinence when working interactively
    print commands.getoutput(ccat('ln -sf',outFile,' latest.root'))

    print
    logger.Info("Summary: nSpills processed= ",nSpills," Total Events Processed= ",nEventsTot)
    logger.Info("Fraction of events kept:",float(eventsInTree)/nEventsTot*100)

    logger.Summary()
    if fakeSpillData: logger.Info("Fake spill data")



if __name__ == '__main__': 
    try:
        opts, args = getopt.getopt(sys.argv[1:], "n:d:r:o:kp")
    except getopt.GetoptError as err: usage()

    NEventLimit=NMAX
    fileList=[]
    inputDir=""
    recurse=False
    keepFlag=False
    prof=False
    outDir=""
    for o, a in opts:
        if o == "-n": NEventLimit=int(a)
        elif o == "-d":
            inputDir=a
        elif o == "-r":
            inputDir=a
            recurse=True
        elif o == "-k": keepFlag=True
        elif o == "-p": prof=True
        elif o == "-o": outDir=a


    if inputDir=="":
        if len(args)>0: fileList=args[0:]
        else: usage()
    elif not recurse:
        if not os.path.isdir(inputDir): usage()
        fileList.extend( glob.glob(inputDir+'/rec_capture_*.txt*') )
    elif recurse:
        cmd=ccat('find',inputDir,'-name "rec_capture_*.txt*"')
        fileList.extend( commands.getoutput(cmd).split() )
    else: usage()

    print "Processing",len(fileList),"files"

    #===========================================================# 
    #  Declare data containers                                  #
    #===========================================================#
    LoadLibs("TBLIB","libTB.so")

    if prof:
        pr = cProfile.Profile()
        pr.enable()

    count=1
    for padeDat in fileList:
        print "="*60
        print "Processing File ===>",padeDat,count,"/",len(fileList)
        print "="*60
        filler(padeDat,NEventLimit,keepFlag,outDir)
        count=count+1
        print "="*60
        print "Finished File ===>",padeDat
        print "="*60

    if prof:
        pr.disable()
        s = StringIO.StringIO()
        sortby = 'cumulative'
        ps = pstats.Stats(pr, stream=s).sort_stats(sortby)
        ps.print_stats()
        print s.getvalue()

    print "Exiting" 
    exit(0)
