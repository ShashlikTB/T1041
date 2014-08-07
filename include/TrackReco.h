// TrackReco Class
// Add an vector of TBTracks to the tree passed to the Process method


#ifndef TRACKRECO_H
#define TRACKRECO_H

#include "WCPlanes.h"
#include <TTree.h>

class TrackReco{
 public:
  TrackReco() {;}
  int Process(TTree *rawTree, TTree *recTree);
 private:
  std::vector<WCChannel> hitsX1, hitsY1, hitsX2, hitsY2;
  int mean_[NTDC];
  int tLow_[NTDC]; 
  int tHigh_[NTDC]; 
};



#endif

