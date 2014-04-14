from wcFileParser import * 
from operator import itemgetter, attrgetter



p = wcParser('wc.dat')
spills = p.spills
spills = sorted(spills, key=itemgetter('id'))
for spill in spills:
    print "Spill %s has %s events." % (spill['id'], len(spill['events']))
    events = spill['events']

    # Sort the events based on the second part of the tuple stored in the time entry
    fn = lambda x: itemgetter('time')(x)[1]
    events = sorted(events, key=fn)
    for event in events:
        print "Spill %s, Event %s, Start Time: %s" % (event['spill'], event['id'], event['time'])
        print 'Modules %s' % event['modules'].keys()
    for event in events:
        print "Event %s, Start Time: %s" % (event['id'], event['time'][1])

        
