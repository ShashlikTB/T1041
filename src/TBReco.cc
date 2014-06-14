#include <TBranch.h>
#include <TSpectrum.h>
#include <TF1.h>
#include <TString.h>
#include <iostream>
#include "TBReco.h"
#include "TBEvent.h"
//#include "calConstants.h"
#include "WC.h"

using std::endl;
using std::cout;


WCtrack::WCtrack(WCChannel x1, WCChannel y1, WCChannel x2, WCChannel y2):
  _x1(x1), _y1(y1), _x2(x2), _y2(y2) {
  SetSlopeX();
  SetSlopeY();
} 

void WCtrack::Project(float z, float &x, float &y){
  x=_x1.GetX()+_mx*z;
  y=_y1.GetY()+_my*z;
}


// warning slope claculations assume we only deal w/ WC1 and WC2!
void WCtrack::SetSlopeX(){
  _mx = (_x2.GetX()-_x1.GetX())/dWC1toWC2; 
}

void WCtrack::SetSlopeY(){
  _my = (_y2.GetY()-_y1.GetY())/dWC1toWC2;  
}


WCReco::WCReco() : _cutsMade(false) {
  TString histname;
  for (Int_t c=0;c<NTDC; c++) {  // storage for the TDC histograms
    histname.Form("TDC%2d",c+1);
    _TDC[c]=new TH1I(histname, "Counts for "+histname, 300, 0.0, 300.0);
  }
}

WCReco::WCReco(TTree *tree){
  WCReco();
  AddTree(tree);
}

float WCReco::GetProjection(Float_t pos1, Float_t pos2,
                      Float_t WCdist, Float_t projDist){
  Float_t Delta = pos1 - pos2;
  Float_t projection = pos1 + (projDist*(Delta/WCdist));
  return (projection - 64);
}

bool WCReco::ScintConfirm(Float_t Pos1, Float_t Pos2, Float_t WCDist){
  const float ProjDist1 = -1231.9;          // distance between WC1 and Scin1
  const float ProjDist2 = -(1231.9+4445.0); // distance between WC1 and Scin2
  Float_t CheckProjection1 = WCReco::GetProjection(Pos1, Pos2, WCDist, ProjDist1);
  Float_t CheckProjection2 = WCReco::GetProjection(Pos1, Pos2, WCDist, ProjDist2);
  if(fabs(CheckProjection1)<=50 && fabs(CheckProjection2)<=50)return true;
  else return false;
}



void WCReco::AddTree(TTree *tree){
  TBEvent *event = new TBEvent();
  TBranch *bevent = tree->GetBranch("tbevent");
    bevent->SetAddress(&event);
  for (int i = 0; i < tree->GetEntries(); i++) {
    tree->GetEntry(i);
    for(int j = 0; j < event->GetWCHits(); j++){
      Int_t module        = event->GetWCChan(j).GetTDCNum();
      Int_t channelCount  = event->GetWCChan(j).GetCount();
      _TDC[module-1]->Fill(channelCount);
    }
  }
  _cutsMade=false;
}


void WCReco::FitTDCs(){
  TSpectrum tspectrum;
  TF1* fGaussian = new TF1("fGaussian","gaus",0,100);
  fGaussian->SetLineColor(2);

  for(Int_t drawG=0; drawG<NTDC; ++drawG){  
    // finding peak using tspectrum
    tspectrum.Search(_TDC[drawG],2,"goff",0.25); 
    int npeaks = tspectrum.GetNPeaks();
    Float_t *tdc_peaks = tspectrum.GetPositionX();
    Float_t early_peak = 9999.;
    for (int ipeak = 0; ipeak < npeaks; ipeak++){
      if ( tdc_peaks[ipeak] < early_peak ) early_peak = tdc_peaks[ipeak];
    }
    // Fitting the peak with a gaussian
    int maxBin    = _TDC[drawG]->FindBin(early_peak);
    _mean[drawG]  = _TDC[drawG]->GetBinCenter(maxBin);
    int ampl      = _TDC[drawG]->GetBinContent(maxBin);
    fGaussian->SetParameters(ampl, _mean[drawG], 4.5);
    fGaussian->SetRange(_mean[drawG]-tdcRange, _mean[drawG]+tdcRange);
    if (_TDC[drawG]->GetEntries()<150) _TDC[drawG]->Rebin(2);   // BH: improve for small samples
    _TDC[drawG]->Fit(fGaussian,"0+QRL");  // BH:add L, improve fits for small samples
    const Int_t kNotDraw = 1<<9;
    _TDC[drawG]->GetFunction("fGaussian")->ResetBit(kNotDraw);
  }
  for (Int_t c=0;c<NTDC; c++) {  // file the in-time ranges
    _tLow[c]=_mean[c]-tdcRange;
    _tHigh[c]=_mean[c]+tdcRange;
  }
  _cutsMade=true;
}

void WCReco::GetTDCcuts(int *mean, int *tLow, int *tHigh){
  if (!_cutsMade) FitTDCs();
  for (Int_t i=0; i<NTDC; i++) {
    mean[i]=_mean[i];
    tLow[i]=_tLow[i];
    tHigh[i]=_tHigh[i];
  }
}

void WCReco::GetTDChists(TH1I** TDC, int nmax){
  for (Int_t i=0; i<nmax; i++) {
    TDC[i]=_TDC[i];
  }
}


void CalCluster::MakeCluster(const vector<CalHit> &calHits, float threshold){
  float sumx=0, sumy=0; 
  float sumx2=0, sumy2=0;
  float sumE2=0;
  _Eu=_Ed=0;
  _ECenter=_EIso=0;
  float x,y,z;
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



CalReco::CalReco(const TTree *tbdata) : _tbdata(tbdata) {;}


