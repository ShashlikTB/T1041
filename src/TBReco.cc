#include <TBranch.h>
#include <TF1.h>
#include <TString.h>
#include <iostream>
#include "TBReco.h"
#include "TBEvent.h"
//#include "calConstants.h"


using std::endl;
using std::cout;




void CalCluster::MakeCluster(const vector<CalHit> &calHits, float threshold){
  float sumx=0, sumy=0; 
  float sumx2=0, sumy2=0;
  float sumE2=0;
  _Eu=_Ed=0;
  _ECenter=_EIso=0;
  double x,y,z;
  for (unsigned j=0; j<calHits.size(); j++){
    float val=calHits[j].Value();
    if (val<threshold) continue;
    calHits[j].GetXYZ(x,y,z);

    if (z>0) _Ed+=val;
    else _Eu+=val;
    sumE2+=val*val;
    sumx+=val*x;
    sumy+=val*y;
    sumx2+=val*x*x;
    sumy2+=val*y*y;
    if (TMath::Abs(x)<14 && TMath::Abs(y)<14) _ECenter+=val;
    else _EIso+=val;
  }
  _x=_y=_z=0;
  _sx=_sy=0;
  if (_Ed+_Eu==0) return;
  _x= sumx/(_Ed+_Eu);
  _y= sumy/(_Ed+_Eu);
  _z= ( _Eu*(-1)+ _Ed*(1) ) / (_Ed+_Eu);
  float neff=(_Ed+_Eu)*(_Ed+_Eu)/sumE2;
  float m2=sumx2/(_Ed+_Eu);
  _sx = TMath::Sqrt( (m2 - _x*_x) * neff / (neff-1) );
  m2=sumy2/(_Ed+_Eu);
  _sy = TMath::Sqrt( (m2 - _y*_y) * neff / (neff-1) );
}

void CalCluster::Print(){
  cout << "CalCluster (x,y,z,E;sx,sy) = ( " 
       << _x << "," << _y << "," << _z << ","  << _Ed+_Eu << ";" << _sx << ","<<_sy<<" )" << endl;
  cout << "ECenter/Iso = " << _ECenter << " / " << _EIso << endl;
}





