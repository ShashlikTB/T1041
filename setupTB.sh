#!/bin/bash

export TBHOME=`pwd`
export TBLIB=$TBHOME/build/lib
export PATH=$TBHOME/bin:$PATH
export PYTHONPATH=$TBHOME/python:$PYTHONPATH
export LD_LIBRARY_PATH=$TBHOME/build/lib:$LD_LIBRARY_PATH

