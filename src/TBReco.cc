#include <TBranch.h>
#include <TSpectrum.h>
#include <TF1.h>
#include <TString.h>
#include <iostream>
#include "TBReco.h"
#include "TBEvent.h"
#include "calConstants.h"
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
  _Eu=_Ed=0;
  float x,y,z;
  for (unsigned j=0; j<calHits.size(); j++){
    float val=calHits[j].Value();
    if (val<threshold) continue;
    calHits[j].GetXYZ(x,y,z);
    //    calHits[j].Print();

    if (z>0) _Ed+=val;
    else _Eu+=val;
    sumx+=val*x;
    sumy+=val*y;
  }
  _x=_y=_z=0;
  if (_Ed+_Eu==0) return;
  _x= sumx/(_Ed+_Eu);
  _y= sumy/(_Ed+_Eu);
  _z= ( _Eu*(-1)+ _Ed*(1) ) / (_Ed+_Eu);
}

void CalCluster::Print(){
  cout << "CalCluster (x,y,z,E) = ( " 
       << _x << "," << _y << "," << _z << ","  << _Ed+_Eu << " )" << endl;
}



CalReco::CalReco(const TTree *tbdata) : _tbdata(tbdata) {;}


