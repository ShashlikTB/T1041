from HTMLParser import HTMLParser
from subprocess import check_output
import pickle


# create a subclass and override the handler methods
class MyHTMLParser(HTMLParser):
    def __init__(self):
        HTMLParser.__init__(self)
        self.data=[]
        self.tdopen=False
        self.havedata=False
    def handle_starttag(self, tag, attrs):
        #print "Encountered a start tag:", tag
        if tag=="tr": 
            self.tdopen=False
            self.data=[]
        if tag=="td":  # enter <td> w/o previous </td>
            if self.tdopen and not self.havedata: self.data.append("na")
            self.tdopen=True
            self.havedata=False
    def handle_endtag(self, tag):
        if tag=="td" or tag=="tr":
            #print "Encountered an end tag :", tag
            self.tdopen=False
            if not self.havedata: self.data.append("na")  # empty <td></td> block
    def handle_data(self, data):
        if self.tdopen:
            if not data==".": self.data.append(data)
            self.tdopen=False # if multipe tags in cell, use first one only
            self.havedata=True
        #print "Encountered some data  :", data
    def getData(self):
        return self.data


# fetch testbeam spreadsheets in HTML formatreturn one line per row
def fetchData():
    URL1="https://docs.google.com/spreadsheet/pub?key=0Aq95c3AaXt2IdExOcktfbk9kRkNybHdqQV9DQTZQVkE"
    #URL1="https://docs.google.com/spreadsheets/d/1HmfBRe2Vj7VFDh_M3P0cLHvQZthRwuvhQe2EeM6mxCI/pubhtml"
    URL2="https://docs.google.com/spreadsheets/d/13naVWmsm7bUAGMlrelABaI6fuXtzOvg5Gh36bIpKwpk/pubhtml"
    cmd='curl "'+URL1+'" "'+URL2+'" | sed s_\<tr_\\\\n\<tr_g | grep rec_cap'
    rundata=check_output(cmd,shell=True)
    return rundata.split("\n")


# instantiate the parser
parser = MyHTMLParser()

rundata=fetchData()

runlist=[]

#for line in open("test.dat"):
for line in rundata:
#    print line
    parser.feed(line)
    fields=parser.getData()
#    print fields
    if not "rec_capture" in line: continue
    padeDat=fields[1]
    timeStamp=padeDat.replace("rec_capture_","").replace(".txt","")
    fields.insert(0,timeStamp)
    runlist.append(fields)
#    print fields
     
# update the local runlist file
with open('runlist.dat', 'w') as f:    
    pickle.dump(runlist,f)

print 'runlist.dat has been updated',len(runlist),"runs"

#print runlist

#with open('runlist.dat', 'r') as f:
#    runlist = pickle.load(f)


