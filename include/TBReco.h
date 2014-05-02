#ifndef TBRECO_H
#define TBRECO_H

#include <TTree.h>
#include <TH1I.h>
#include <TObject.h>
#include "TBEvent.h"
#include "WC.h"

// a track is a container for a pair of (x,y) hits in each two chambers
class WCtrack{
 public:
  WCtrack(WCChannel x1, WCChannel y1, WCChannel x2, WCChannel y2);
  float GetSlopeX(){return _mx;}
  float GetSlopeY(){return _my;}
  // flag is a bit mask
  // bit 0: confirmed by project to SC1
  // bit 1: confirmed by project to SC2
  static const int kSC1=1;
  static const int kSC2=2;
  //bool Confirmed(int flag=1){return true;}  // to do 
 private:
  void SetSlopeX();
  void SetSlopeY();
  WCChannel _x1, _y1, _x2, _y2;
  float _mx, _my;  // slopes
};


// This class histograms TDC data based on on or more input trees
// It provides the in time cuts
class WCReco{
public:
  WCReco(); 
  WCReco(TTree *tree);
  void AddTree(TTree *tree);
  void GetTDChists(TH1I** TDC, int nmax=NTDC);
  void GetTDCcuts(int *mean, int *tLow, int *tHigh);
 private:
  void FitTDCs();
  TH1I* _TDC[NTDC];
  float _mean[NTDC];
  float _tLow[NTDC];
  float _tHigh[NTDC];
  bool _cutsMade;
};


class CalCluster{
 public:
  CalCluster(){;}
  CalCluster(const TBEvent *event);
  void MakeCluster(const TBEvent *event=0, float threshold=0);
  void MakeCluster(float threshold=0);
  void SetEvent(const TBEvent *event) {_event=event;}
  float GetX() {return _x;}
  float GetY() {return _y;}
 private:
  const TBEvent *_event;
  float _Eu, _Ed;   // upsteam and downstream "energies"
  float _x, _y;     // E-weighted position  
  float _sx, _sy;   // width
  vector<PadeChannel> _channels;   // channels included in cluster
};



// use this for plots integrated over a run
class CalReco{
 public:
  CalReco(){;}
  CalReco(const TTree *tbdata);
  void SetTree(const TTree *tbdata) {_tbdata=tbdata;}
 private:
  const TTree *_tbdata;
};


#endif
