#include <TSpectrum.h>

#include "TBTrack.h"
#include "WC.h"

#include <iostream>

using std::endl;
using std::cout;


TBTrack::TBTrack(WCChannel x1, WCChannel y1, WCChannel x2, WCChannel y2):
  _x1(x1), _y1(y1), _x2(x2), _y2(y2) {
  SetSlopeX();
  SetSlopeY();
  SetSlope2D();
  SetTimDiffWC1();
  SetTimDiffWC2();
}

void TBTrack::Project(float z, float &x, float &y){
  x=_x1.GetX()+_mx*z;
  y=_y1.GetY()+_my*z;
}


// warning slope calculations assume we only deal w/ WC1 and WC2!
void TBTrack::SetSlopeX(){
  _mx = (_x2.GetX()-_x1.GetX())/dWC1toWC2; 
}

void TBTrack::SetSlopeY(){
  _my = (_y2.GetY()-_y1.GetY())/dWC1toWC2;  
}

void TBTrack::SetSlope2D(){
  _m2d = TMath::Sqrt( (_x2.GetX()-_x1.GetX())*(_x2.GetX()-_x1.GetX())
		      + (_y2.GetY()-_y1.GetY())*(_y2.GetY()-_y1.GetY()) )
    /dWC1toWC2;  
}

/// getting the X-Y TDCCounts ///
void TBTrack::SetTimDiffWC1(){
  _dt1=_x1.GetCount()-_y1.GetCount();
}

void TBTrack::SetTimDiffWC2(){
  _dt2=_x2.GetCount()-_y2.GetCount();
}

/// Account for Table Position in Shashlik Extrapolation ///
void TBTrack::TablePos(float x_pos, float x_pos_table, float y_pos, float y_pos_table, float &offX, float &offY){
  offX = x_pos + x_pos_table;
  offY = y_pos + y_pos_table;
}

void TBTrack::GetHits(WCChannel &x1, WCChannel &y1, WCChannel &x2, WCChannel &y2){
  x1=_x1; x2=_x2;
  y1=_y1; y2=_y2;
}



WCReco::WCReco() : _cutsMade(false) {
  TString histname;
  for (Int_t c=0;c<NTDC; c++) {  // storage for the TDC histograms
    histname.Form("TDC%d",c+1); // no space for histname, TDC 1  will now read TDC1 and so on
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
    if (i==0) _run=TBEvent::GetRunPeriod(event);
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
    tspectrum.Search(_TDC[drawG],5,"goff",0.25); 
    int npeaks = tspectrum.GetNPeaks();
    Float_t *tdc_peaks = tspectrum.GetPositionX();
    Float_t early_peak = 9999.;
    // hacky!  Test beam2 runs have TDC peak at earlier time values
    // whereas test beam1 runs may have noise peaks at early time values
    // better solution: set minimum width for choosing first peak
    float minPeakTime=25;
    if (_run==TBEvent::TBRun2b) minPeakTime=0;
    for (int ipeak = 0; ipeak < npeaks; ipeak++){  // TEMPORARY! was 25 for tb1
      if ( tdc_peaks[ipeak] < early_peak && tdc_peaks[ipeak] > minPeakTime) 
	early_peak = tdc_peaks[ipeak];
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



