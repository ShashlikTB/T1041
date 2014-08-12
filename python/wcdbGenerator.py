import getopt,sys
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

def usage():
    print
    print "Usage: python TBNtupleMaker [OPTION] [PADE_FILE] [PADE_FILE] ..."
    print "      -d DIR         : Process all padefiles in DIR"
    print "                       Overrides all files given on command line list"
    print "      -x extension   : Filename extension to use"
    print "      -o DIR         : Output dir, instead of default = PWD" 
    print "      -f             : force regenerating the database"
    print 
    sys.exit()

def main():

        try:
                opts, args = getopt.getopt(sys.argv[1:], "d:x:o:f")
        except getopt.GetoptError as err: usage()

        extension=".bz2"
        force=False
        location="."  # location of input files
        dbfile="wcdb.txt"
        for o, a in opts:
                if o == "-d":
                        location=a
                        print "Reading WC data in",a
                elif o == "-x":
                        extension=a  
                elif o == "-o":
                        dbfile=a
                elif o == "-f": force=True



	dbHandle = None 
	prevReadFiles = None 
	if force:
		dbHandle = open(dbfile, 'w')
	else: 
		try:
			handle = open(dbfile, 'r')
			prevReadFiles = findPrevReadFiles(handle)
			dbHandle = open(dbfile, 'a')
		except IOError: 
			print "Couldn't open db file %s, regenerating" % dbfile
			dbHandle = open(dbfile, 'w')

	if os.path.isdir(location): 
                absPath =  os.path.abspath(location)
                # joinedPath = os.path.join(absPath, location)
                # files = glob.glob(joinedPath)
                files = glob.glob(absPath+"/t1041*dat*")
                print location,absPath,files
        elif os.path.isfile(location):
                files=[]
                files.append(os.path.abspath(location))

        #Generate a list of tuples containing the filename and file modification time 
        sortedFiles = []
        for f in files:
            sortedFiles.append((f,os.path.getmtime(f)))

        #Sort the files by the file modification time 
        sortedFiles = sorted(sortedFiles, key=itemgetter(1))
        for filename,mtime in sortedFiles:
                if force or not prevReadFiles or (prevReadFiles and filename not in prevReadFiles):
                        if not "t1041_" in filename: continue  # not a WC file
                        if filename.endswith(".dat.bz2"):
                                try:
                                        wcHandle = bz2.BZ2File(filename, "r")
                                        logger.Info("Processing %s" % filename)
                                        generateSpillDB(wcHandle, dbHandle, filename)
                                except IOError as e:
                                        logger.Warn("Unable to open %s, %s" % (filename, e))
                        elif filename.endswith(".dat"): 
                                try:
                                        wcHandle = open(filename, "r")
                                        logger.Info("Processing %s" % filename)
                                        generateSpillDB(wcHandle, dbHandle, filename)
                                except IOError as e:
                                        logger.Warn("Unable to open %s, %s" % (filename, e))



if __name__ == "__main__":
    main()


