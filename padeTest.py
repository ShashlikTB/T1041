from TBUtils import * 
logger=Logger(1)  # instantiate a logger, w/ 1 repetition of messages


from padeFileParser import * 
from operator import itemgetter



p = padeParser('rec_capture_20140409_102942.txt.bz2')
print "Done Processing"

for spill in p.spills:
    print "ID: %s, pcTime: %s, Events: %s" % (spill['id'], spill['pcTime'], len(spill['events']))


