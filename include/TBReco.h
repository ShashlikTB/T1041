#ifndef TBRECO_H
#define TBRECO_H

// soon to be removed!

#include "TBEvent.h"


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




#endif
