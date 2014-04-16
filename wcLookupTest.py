from TBUtils import * 
import argparse 


parser = argparse.ArgumentParser(description='Test Beam Wire Chamber DB Tester')
parser.add_argument('-f', metavar='-f', type=str, nargs=1, default=['wcdb.txt'],
                    help="Database file name")
parser.add_argument('-b', metavar='-b', type=int, nargs=1, default=[20],
                    help='Time Bound for the event time')
parser.add_argument('eventTime', metavar='eventTime', type=int, nargs=1, 
                    help='Event Time to search for in the db file')

args = parser.parse_args()



fileName = args.f[0]
eventTime = args.eventTime[0]
boundTime = args.b[0]



print wcLookup(eventTime, boundTime, fileName)


