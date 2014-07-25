// Created 4/12/2014 B.Hirosky: Initial release
// Modified 6/6/14 BH: Add S. Ledovskoy's fitter

#include <iostream>
#include <stdio.h>
#include "TTree.h"
#include "TFile.h"
#include "TString.h"
#include "TF1.h"
#include "WC.h"
#include "TBEvent.h"

using namespace std;

void TBEvent::Reset(){
  padeChannel.clear();
  wc.clear();
}


void TBEvent::FillPadeChannel(ULong64_t ts, UShort_t transfer_size, 
			      UShort_t  board_id, UInt_t hw_counter, 
			      UInt_t ch_number,  UInt_t eventnum, Int_t *wform){

  Mapper *mapper=Mapper::Instance();
  if (!mapper->validChannel(board_id, ch_number)){ // sanity check 
    cerr << "Warning: channel ID error, board:channel " 
	 << board_id << ":" << ch_number << endl;
  }
  PadeChannel pc;  // todo make constructor w/ fill inputs
  pc.Fill(ts, transfer_size, board_id, hw_counter, ch_number, eventnum, wform);
  padeChannel.push_back(pc);
}


void TBEvent::AddWCHit(UChar_t num, UChar_t wire, UShort_t count){
  WCChannel wctmp(num,wire,count);
  wc.push_back(wctmp);
}

// return X hits in a WC (if min/max given, use these to calculate in-time hits)
vector<WCChannel> TBEvent::GetWChitsX(Int_t nwc, Int_t *min, Int_t* max) const{
  vector<WCChannel> hits;
  for (unsigned i=0;i<wc.size(); i++){
    Int_t tdc=wc[i].GetTDCNum();
    bool keep = tdc2WC(tdc)==nwc && (tdc-1)%4<2;   // match to chamber
    if (max) {
      UShort_t count=wc[i].GetCount();
      keep &= count>=min[tdc] && count<=max[tdc];
    }
    if (keep) hits.push_back(wc[i]);
  }
  return hits;
}
vector<WCChannel> TBEvent::GetWChitsY(Int_t nwc, Int_t *min, Int_t* max) const{
  vector<WCChannel> hits;
  for (unsigned i=0;i<wc.size(); i++){
    Int_t tdc=wc[i].GetTDCNum();
    bool keep = tdc2WC(tdc)==nwc && (tdc-1)%4>1;   // match to chamber
    if (max) {
      UShort_t count=wc[i].GetCount();
      keep &= count>=min[tdc] && count<=max[tdc];
    }
    if (keep) hits.push_back(wc[i]);  
  }
  return hits;
}




void WCChannel::Dump() const {
  cout << "TDC# " << (int)_tdcNumber <<  " Wire " << (int)_tdcWire 
       << " Count " << (int)_tdcCount << endl;
}


void TBSpill::SetSpillData(Int_t spillNumber, ULong64_t pcTime, 
			   Int_t nTrigWC, ULong64_t wcTime,
			   Int_t pdgID, Float_t nomMomentum,
			   Float_t tableX, Float_t tableY, Float_t boxTemp, 
			   Float_t roomTemp){
  _spillNumber=spillNumber;
  _pcTime=pcTime;
  _nTrigWC=nTrigWC;
  _wcTime=wcTime;
  _pdgID=pdgID;
  _nomMomentum=nomMomentum;
  _tableX=tableX;
  _tableY=tableY;
  _boxTemp=boxTemp;
  _roomTemp=roomTemp;
}

void TBSpill::Reset(){
  _spillNumber=0;
  _pcTime=0;
  _nTrigWC=0;
  _wcTime=0;
  _padeHeader.clear();
  _pdgID=0;
  _nomMomentum=0;
  _tableX=-999;
  _tableY=-999;
  _boxTemp=0;
  _roomTemp=0;
}

void TBSpill::Dump() const {
  cout << "### TBSpill Dump ###" << endl;
  cout << "Spill Number: " << _spillNumber << endl;
  cout << "PC Time : " << _pcTime << endl;
  cout << "####################";
}

// warning: assume we only deal w/ WC1 and WC2! _tdcnum<=4
float WCChannel::GetX(){
  if (_tdcNumber==2 || _tdcNumber==6) return (0.5+_tdcWire);
  else if (_tdcNumber==1 || _tdcNumber==5) return -63.5+_tdcWire;
  return -999;  // not an x hit
}

float WCChannel::GetY(){
  if (_tdcNumber==4 || _tdcNumber==8) return -1.0*(0.5+_tdcWire);
  else if(_tdcNumber==3 || _tdcNumber==7) return 63.5-_tdcWire;
  return -999;  // not a y hit
}



void TBEvent::GetCalHits(vector<CalHit> &calHits, float* calconstants, float cut){
  calHits.clear();
  for (Int_t i=0; i<NPadeChan(); i++){
    int idx=padeChannel[i].GetChannelIndex();
    double ped,sig;
    padeChannel[i].GetPedestal(ped,sig);
    float val=padeChannel[i].GetMax()-ped;      
    if (calconstants) val*=calconstants[idx];     // relative calibration
    // Hack here to fix bad channel
    // bad channel 11316, set equal to 11608 (set rear fiber to front fiber)
    // padechannels are ordered according to boardID*100+channelID
    // so 11316 = padeChannel[48],  11608 = padeChannel[104]
    if (i==48){
      padeChannel[104].GetPedestal(ped,sig);
      val=padeChannel[104].GetMax()-ped;
      if (calconstants) val*=calconstants[104];    
    }    
    if (val>0 && val/sig>cut) calHits.push_back( CalHit(idx, val) );  // n-sigma noise cut
  }
}

// This version uses Sasha's fitting code to return pedestal subtracted hits
void TBEvent::GetCalHitsFit(vector<CalHit> &calHits, float* calconstants, float cut){
  calHits.clear();
  PulseFit fit;
  double ped,sig;

  for (Int_t i=0; i<NPadeChan(); i++){
    int idx=padeChannel[i].GetChannelIndex();

    // BH: speed things up by not fitting small pulses
    padeChannel[i].GetPedestal(ped,sig);
    float val=padeChannel[i].GetMax()-ped;
    if (val>0 && val/sig>cut) continue;

    // Hack here to fix bad channel
    // bad channel 11316, set equal to 11608 (set rear fiber to front fiber)
    // padechannels are ordered according to boardID*100+channelID
    // so 11316 = padeChannel[48],  11608 = padeChannel[104]
    if (i==48){
      fit=PadeChannel::FitPulse(&padeChannel[104]);
    }
    else fit=PadeChannel::FitPulse(&padeChannel[i]);
    if (calconstants) fit.aMaxValue*=calconstants[idx]; 
    if (fit.aMaxValue/fit.noise > cut)   // n-sigma noise cut
      calHits.push_back( CalHit(idx, fit.aMaxValue) );  
  }
}

void TBEvent::CalibrateCalHits(vector<CalHit> &calHits, float* calconstants){
  for (unsigned i=0; i<calHits.size(); i++){
    int idx=calHits[i].GetChannelIndex();
    double calib=calHits[i].Value()*calconstants[idx];
    calHits[i].SetValue(calib);
  }
}

