#ifndef TRACKRECO_H
#define TRACKRECO_H

#include "WCPlanes.h"
#include <TTree.h>

/// TrackReco Class : Add tbtracks branch
/** Add an vector of TBTracks to the tree passed to the Process method 
    Multiple tracks may be found in a single event.  These are ordered
    according to incerasind 2D slope**/
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

