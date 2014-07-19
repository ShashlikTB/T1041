#ifndef CALRECO_H
#define CALRECO_H

#include <TTree.h>

class CalReco{
 public:
  CalReco(){;}
  int Process(TTree *recTree);
 private:
  TTree *_recTree;
};


#endif

