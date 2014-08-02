#!/bin/bash

export TBHOME=`pwd`
export TBLIB=$TBHOME/build/lib
export PATH=$TBHOME/bin:$PATH
export PYTHONPATH=$TBHOME/python:$PYTHONPATH
export LD_LIBRARY_PATH=$TBHOME/build/lib:$LD_LIBRARY_PATH

# hack until someone figures out  how to check for ROOT exe in the Makefile
if [ -e /usr/bin/root ] ; then
  if [ -z $ROOTSYS ] ; then export ROOTSYS="." ; fi
fi


HOST=`hostname`
if [ "$host" == "heprocks-head.phys.virginia.edu" ] ; then
  alias python=/raids/nas0/tba9h/tools/bin/python2.7
fi


