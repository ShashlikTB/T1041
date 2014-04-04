import sys
from commands import getoutput,getstatusoutput

def INFO(*arg):
    s=""
    for i in range(len(arg)):
        s=s+" "+str(arg[i])
    sys.stderr.write("Info: "+s+"\n")


def WARN(*arg):
    RED_ON='\033[91m'
    COL_OFF='\033[0m'
    s=""
    for i in range(len(arg)):
        s=s+" "+str(arg[i])
    sys.stderr.write(RED_ON+"Warning: "+s+"\n"+COL_OFF)


