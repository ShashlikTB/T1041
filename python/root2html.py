import re, sys, commands, os
sys.argv.append( '-b' )
from ROOT import *

COLUMNS=2

rootFile=TString(sys.argv[1])
outDir=TString(rootFile)
outDir=str(outDir.ReplaceAll(".root",""))
commands.getstatusoutput("rm -rf "+outDir)
commands.getstatusoutput("mkdir -p "+outDir)
titles=[]
fi = TFile(rootFile.Data(),"READ")
html=os.path.join(outDir,"index.html")
fo = open(html, "wb")
fo.write('''<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01//EN )
"http://www.w3.org/TR/html4/strict.dtd">''')
fo.write('<head><title>'+rootFile.Data()+'</title></head>')
fo.write('<body><TABLE border=2>')
fo.write('<TR><TH colspan='+str(COLUMNS)+'align=center><H3>Figures</H3></TH></TR>')


c1=TCanvas("c1","histo")
count=0
for key in fi.GetListOfKeys():
    obj = key.ReadObj(); 
    if (obj.IsA().InheritsFrom("TH1")):
        name=str(obj.GetName())
        img=re.sub('[^-a-zA-Z0-9_.]+', '', name)+'.png'
        obj.Draw();
        c1.Print(os.path.join(outDir,img));
        if count%COLUMNS==0: fo.write('<TR>\n')
        fo.write('<TD><CENTER>')
        fo.write('<img src="'+img+'"><BR>'+img)
        fo.write('</CENTER></TD>\n')
        count=count+1
        if count%COLUMNS==0: fo.write('</TR>\n')


fo.write('</TABLE></body></html>')
