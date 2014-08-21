#ifndef CALRECO_H
#define CALRECO_H

#include <TTree.h>

/// CalReco Class : Add tbrechits branch
/** Add an vector of TBRecHits to the tree passed to the Process method
    nSigmaCut is used to suppress channels that have amplitude < nSigma over
    the their estimated noise thresholds <br>
    Dead channels are remapped to mirror the channel on the opposing side. 
    Except in the special case of laser runs in July/Aug 2014.  In this case
    channels are left as is, but the TBRecHit::kMonitor bit is set.  
**/
class CalReco{
 public:
 CalReco(float nSigmaCut=0) : _nSigmaCut(nSigmaCut) {;}
  int Process(TTree *rawTree, TTree *recTree);
 private:
  float _nSigmaCut;
};


#endif

