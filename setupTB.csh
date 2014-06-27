#!/bin/sh

setenv TBHOME `pwd`
setenv TBLIB  ${TBHOME}/build/lib
setenv PATH   ${TBHOME}/bin:${PATH}
setenv PYTHONPATH ${TBHOME}/python:${PYTHONPATH}
setenv LD_LIBRARY_PATH ${TBHOME}/build/lib:${LD_LIBRARY_PATH}
