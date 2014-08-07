#ifndef TBTRACK_H
#define TBTRACK_H

#include <TTree.h>
#include <TH1I.h>
#include <TObject.h>
#include "TBEvent.h"
#include "WC.h"

// a track is a container for a pair of (x,y) hits in each of chambers 1 and 2
class TBTrack : public TObject {
  ClassDef(TBTrack,1);
 public:
  TBTrack(){;}
  TBTrack(WCChannel x1, WCChannel y1, WCChannel x2, WCChannel y2);
  float GetSlopeX(){return _mx;}
  float GetSlopeY(){return _my;}
  float GetSlope2D(){return _m2d;}
  void Project(float z, float &x, float &y);
  int TimingDifferenceWC1(){return _dt1;}
  int TimingDifferenceWC2(){return _dt2;}
  void TablePos(float x_pos, float x_pos_table, float y_pos, float y_pos_table, float &offX, float &offY);
  void GetHits(WCChannel &x1, WCChannel &y1, WCChannel &x2, WCChannel &y2);
  // flag is a bit mask
  // bit 0: confirmed by projection to SC1
  // bit 1: confirmed by projection to SC2
  static const int kSC1=1;  // not used
  static const int kSC2=2;
  //bool Confirmed(int flag=1){return true;}  // to do 
  bool operator < (const TBTrack& trk) const{
    return _m2d<trk._m2d;
  }
 private:
  void SetSlopeX();
  void SetSlopeY();
  void SetSlope2D();
  void SetTimDiffWC1();
  void SetTimDiffWC2();
  WCChannel _x1, _y1, _x2, _y2;
  float _mx, _my, _m2d;  // slopes
  int _dt1, _dt2;       // time difference
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



#endif
