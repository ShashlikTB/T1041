import argparse 
import bz2
import re
import glob
import os 
import os.path
import datetime
from operator import itemgetter
import time
from TBUtils import * 

logger=Logger(1)  # instantiate a logger, w/ 1 repetition of messages



#generate a set of previously read files
def findPrevReadFiles(handle): 
	files = set()
	for line in handle: 
		l = re.split(' +', line.strip())
		files.add(l[-2])
		
	return files
		

def generateSpillDB(wcHandle, dbHandle, filename): 
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
			tstring = time.strptime(' '.join([currentSpill['date'], currentSpill['time']]) , "%d-%m-%y %H:%M:%S")
			currentSpill['unixtime'] = time.mktime(tstring)

			spills.append(currentSpill)
			print "New Spill position: %s" % (pos - len(line))
		currentSpill  =  {
			'pos': pos-len(line), 
			'unixtime':None,
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
	    elif data[0] == 'EVENT':
		    if currentSpill['date'] is None:
			    logger.Warn('Bad Data File no SDATE/TIME...skipping')
			    return

	    if (pos > 0) and currentSpill == None:
		    logger.Warn("Bad File...skipping")
		    return
        
        if currentSpill:
		tstring = time.strptime(' '.join([currentSpill['date'], currentSpill['time']]) , "%d-%m-%y %H:%M:%S")
		currentSpill['unixtime'] = time.mktime(tstring)
		spills.append(currentSpill)

        for spill in spills:
		outputLine = "%s    %s    %s    %s    %s\r\n" % (spill['unixtime'], spill['date'], spill['time'], os.path.abspath(filename), spill['pos'])
		dbHandle.write(outputLine)

def main():
	# Argparse Definitions 

	parser = argparse.ArgumentParser(description='Test Beam Wire Chamber DB Generator')
	parser.add_argument('-f', metavar='-f', type=str, nargs=1, default=None,
			    help="Filename or Pathname to process")
	parser.add_argument('-x', metavar='extension', type=str, nargs=1, 
			    default = ['*.bz2'],
			    help="Filename extension to use")
	parser.add_argument('--force', action = 'store_true',
			    help="force regenerating the database")
	parser.add_argument('-output', metavar='o', type=str, nargs=1, 
			    default = ['wcdb.txt'],
			    help="Filename of output database")


	args = parser.parse_args()
	dbHandle = None 
	prevReadFiles = None 
	if args.force:
		dbHandle = open(args.output[0], 'w')
	else: 
		try:
			handle = open(args.output[0], 'r')
			prevReadFiles = findPrevReadFiles(handle)
			dbHandle = open(args.output[0], 'a')
		except IOError: 
			print "Couldn't open db file %s, regenerating" % args.output[0]
			dbHandle = open(args.output[0], 'w')



	if args.f is None: 
		location = os.path.curdir
	else:
		location = args.f[0]

	if os.path.isfile(location):
	    filename = location
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
	elif os.path.isdir(location): 
	    absPath =  os.path.abspath(location)
	    joinedPath = os.path.join(absPath, args.x[0])
	    files = glob.glob(joinedPath)

	    #Generate a list of tuples containing the filename and file modification time 
	    sortedFiles = []
	    for f in files:
		    sortedFiles.append((f,os.path.getmtime(f)))

	    #Sort the files by the file modification time 
	    sortedFiles = sorted(sortedFiles, key=itemgetter(1))
	    for filename,mtime in sortedFiles:
                if args.force or not prevReadFiles or (prevReadFiles and filename not in prevReadFiles):
			if filename.endswith(".bz2"):
				try:
					wcHandle = bz2.BZ2File(filename, "r")
					logger.Info("Processing %s" % filename)
					generateSpillDB(wcHandle, dbHandle, filename)
				except IOError as e:
					logger.Warn("Unable to open %s, %s" % (filename, e))
			else: 
				try:
					wcHandle = open(filename, "r")
					logger.Info("Processing %s" % filename)
					generateSpillDB(wcHandle, dbHandle, filename)
				except IOError as e:
					logger.Warn("Unable to open %s, %s" % (filename, e))



if __name__ == "__main__":
    main()


