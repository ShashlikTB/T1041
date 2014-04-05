T1041
=====

Analysis code for T1041 test beam run

Make TTrees from TB data 
------------------------
Usage:  
python TBNtupleMaker [OPTION] PADE_FILE [WC_FILE]
       Options 
       -n max_events  : Maximum number of events to read"


Make png plots and html page from a root file
---------------------------------------------
root2html file.root
     png files are an index.html file are produced in directory called "file", 
     the root file name w/o the .root


git cheatsheet
==============

clone a working copy

HTTPS clone:
git clone https://github.com/ShashlikTB/T1041.git
SSH clone:
git clone git@github.com:ShashlikTB/T1041.git


update/add:
git add <file>
git commit -m "comment"

check it in:
git push origin master

to do: branching information

