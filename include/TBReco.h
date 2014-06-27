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
  void Project(float z, float &x, float &y);
  int TimingDifferenceWC1(){return _dt1;}
  int TimingDifferenceWC2(){return _dt2;}
  void TablePos(float x_pos, float x_pos_table, float y_pos, float y_pos_table, float &offX, float &offY);
  // flag is a bit mask
  // bit 0: confirmed by project to SC1
  // bit 1: confirmed by project to SC2
  static const int kSC1=1;
  static const int kSC2=2;
  //bool Confirmed(int flag=1){return true;}  // to do 
  
 private:
  void SetSlopeX();
  void SetSlopeY();
  void SetTimDiffWC1();
  void SetTimDiffWC2();
  WCChannel _x1, _y1, _x2, _y2;
  float _mx, _my;  // slopes
  int _dt1, _dt2; // time difference
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
  float GetProjection(float pos1, float pos2, float WCdist, float projDist);
  bool ScintConfirm(float Pos1, float Pos2, float WCDist);
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
  void MakeCluster(const vector<CalHit> &calHits, float threshold=0);
  float GetX() {return _x;}
  float GetY() {return _y;}
  float GetZ() {return _z;}
  float GetSigX() {return _sx;}
  float GetSigY() {return _sy;}
  float GetE() {return _Eu+_Ed;}
  float GetEu() {return _Eu;}
  float GetEd() {return _Ed;}
  float GetECenter() {return _ECenter;}
  float GetEIso() {return _EIso;}
  void Print();
 private:
  float _Eu, _Ed;   // upsteam and downstream "energies"
  float _x, _y, _z;     // E-weighted position  
  float _sx, _sy;   // width
  float _ECenter, _EIso;  // energy in central 2x2 modules, outer ring
  vector<CalHit> _hits;   // hits included in cluster
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
