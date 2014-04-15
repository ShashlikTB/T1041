import argparse 
import bz2
import re
import glob
import os 
import os.path
from TBUtils import * 
logger=Logger(1)  # instantiate a logger, w/ 1 repetition of messages




def generateSpillDB(wcHandle, dbHandle): 
        logger.Info("Generating Spill DB")
        currentSpill = None 
        spills = []

        if not wcHandle:
            raise 

        line = -1
        while line != '': 
            line = wcHandle.readline()
            pos = wcHandle.tell()

            data = re.split(' +', line.strip())

            if data[0] == 'SPILL': 
                if currentSpill: 
                    spills.append(currentSpill)
                print "New Spill position: %s" % (pos - len(line))
                currentSpill  =  {
                    'pos': pos-len(line), 
                    'date': None,
                    'time': None,
                    }
                
            elif data[0] == 'SDATE': 
                data = re.split(' +', line.strip())
                date = None
                if len(data) != 4: 
                    logger.Warn("Bad start date: %s" % line)
                else: 
                    date = '-'.join(data[1:])
                currentSpill['date'] = date
            elif data[0] == 'STIME':
                data = re.split(' +', line.strip())
                t = None
                if len(data) != 4:
                    logger.Warn("Bad start time: %s" % line)
                else:
                    t = ':'.join(data[1:])
                currentSpill['time'] = t

        if currentSpill:
            spills.append(currentSpill)

        print len(spills)
        for spill in spills: 
            outputLine = "%s %s, %s, %s\r\n" % (spill['date'], spill['time'], filename, spill['pos'])
            dbHandle.write(outputLine)


# Argparse Definitions 

parser = argparse.ArgumentParser(description='Test Beam Wire Chamber DB Generator')
parser.add_argument('location', metavar='location', type=str, nargs=1, 
                    help="Filename or Pathname to process")
parser.add_argument('-x', metavar='extension', type=str, nargs=1, 
                    default = ['*.bz2'],
                    help="Filename extension to use")
parser.add_argument('-output', metavar='o', type=str, nargs=1, 
                    default = ['wcdb.csv'],
                    help="Filename of output database")


args = parser.parse_args()

dbHandle = open(args.output[0], 'w')

if os.path.isfile(args.location[0]): 
    filename = args.location[0]
    try:
        if filename.endswith(".txt") or filename.endswith(".dat"):
            wcHandle = open(filename, "r")
        elif filename.endswith(".bz2"):
            wcHandle = bz2.BZ2File(filename, "r")
        else:
            logger.Warn('Unrecognized filename extension')

    except IOError as e:
        logger.Warn("Unable to open %s, %s" % (filename, e))
    generateSpillDB(wcHandle, dbHandle)
elif os.path.isdir(args.location[0]): 
    absPath =  os.path.abspath(args.location[0])
    joinedPath = os.path.join(absPath, args.x[0])

    for filename in glob.glob(joinedPath):
        if filename.endswith(".bz2"):
            try:
                wcHandle = bz2.BZ2File(filename, "r")
                logger.Info("Processing %s" % filename)
                generateSpillDB(wcHandle, dbHandle)
            except IOError as e:
                logger.Warn("Unable to open %s, %s" % (filename, e))
        else: 
            try:
                wcHandle = open(filename, "r")
                logger.Info("Processing %s" % filename)
                generateSpillDB(wcHandle, dbHandle)
            except IOError as e:
                logger.Warn("Unable to open %s, %s" % (filename, e))





