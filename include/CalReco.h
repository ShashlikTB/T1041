// CalReco Class
// Add an vector of TBRecHits to the tree passed to the Process method
// nSigmaCut is used to suppress channels that have amplitude < nSigma over
// the their estimated noise thresholds

#ifndef CALRECO_H
#define CALRECO_H

#include <TTree.h>

class CalReco{
 public:
 CalReco(float nSigmaCut=0) : _nSigmaCut(nSigmaCut) {;}
  int Process(TTree *recTree);
 private:
  float _nSigmaCut;
};


#endif

