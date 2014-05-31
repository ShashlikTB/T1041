// Created 4/12/2014 B.Hirosky: Initial release

// to do: get smater that GetMax, at least apply a timing cut!

#include <iostream>
#include <stdio.h>
#include "TTree.h"
#include "TFile.h"
#include "TString.h"
#include "WC.h"
#include "calConstants.h"
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

// return X hits in a WC (if min/ma given, use these to calculate in-time hits)
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


void PadeChannel::Dump() const{
  cout << "Header ==> timestamp: " <<  _ts << " size: " 
       << _transfer_size << " board: " << _board_id << " xfer#: " 
       << _hw_counter << " ch#: " <<  _ch_number << " event#: " 
       << _eventnum << endl << "samples=> " << (hex);
  for (int i=0; i<N_PADE_SAMPLES; i++) cout << _wform[i] << " ";
  cout << (dec) << endl;
}


void PadeChannel::Fill(ULong64_t ts, UShort_t transfer_size, 
		      UShort_t board_id, UInt_t hw_counter, 
		      UInt_t  ch_number,  UInt_t eventnum, Int_t *wform){
  _ts = ts;
  _transfer_size = transfer_size;
  _board_id = board_id;
  _hw_counter = hw_counter;
  _ch_number = ch_number;
  _eventnum = eventnum;
  _max=0;

  // HACK, needs improvement
  // find in-time window
  int idx=_board_id-112;   // BAD practice!
  if (idx>1) idx--;
  int tmin=PADE_SAMPLE_TIMES[idx]-PADE_SAMPLE_RANGE;
  int tmax=PADE_SAMPLE_TIMES[idx]+PADE_SAMPLE_RANGE;

  for (int i=0; i<N_PADE_SAMPLES; i++) {
    _wform[i]=wform[i];
    if (i<tmin || i>tmax) continue;
    if ((unsigned)wform[i]>_max) {
      _max=wform[i];
      _peak=i;  // sample number for peak
    }
  }
  _flag = 0;
}

void PadeChannel::Reset(){
  _ts=0;
  _transfer_size=0;
  _board_id=0;
  _hw_counter=0;
  _ch_number=0;
  _eventnum=0;
  _max=0;
  for (int i=0; i<N_PADE_SAMPLES; i++) _wform[i]=0;
}


void PadeChannel::GetHist(TH1F *h){
  TString ti;
  ti.Form("Event %d : Board %d, channel %d",_eventnum, GetBoardID(),GetChannelNum());
  h->Reset();
  h->SetTitle(ti);
  h->SetBins(N_PADE_SAMPLES,-0.5,N_PADE_SAMPLES-0.5);
  for (int i=0; i<N_PADE_SAMPLES; i++){
    h->SetBinContent(i,_wform[i]);
  }
}

void PadeChannel::GetXYZ(float &x, float &y, float &z){
  Mapper *mapper=Mapper::Instance();
  mapper->ChannelXYZ(GetChannelID(),x,y,z);
}

// trivial pedistal estimation
Float_t PadeChannel::GetPedistal(){
  float ped=0;
  for (int i=0;i<5;i++) {ped+=_wform[i];}
  return ped/5;
}


void WCChannel::Dump() const {
  cout << "TDC# " << (int)_tdcNumber <<  " Wire " << (int)_tdcWire 
       << " Count " << (int)_tdcCount << endl;
}



void TBSpill::SetSpillData(Int_t spillNumber, ULong64_t pcTime, 
			   Int_t nTrigWC, ULong64_t wcTime){
  _spillNumber=spillNumber;
  _pcTime=pcTime;
  _nTrigWC=nTrigWC;
  _wcTime=wcTime;
}

void TBSpill::Reset(){
  _spillNumber=0;
  _pcTime=0;
  _nTrigWC=0;
  _wcTime=0;
  _padeHeader.clear();
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

Int_t PadeChannel::GetChannelIndex(){
  Mapper *mapper=Mapper::Instance();
  return mapper->ChannelID2ChannelIndex(GetChannelID());
}


void TBEvent::GetCalHits(vector<CalHit> &calHits, float* calconstants){
  Mapper *mapper=Mapper::Instance();
  calHits.clear();
  for (Int_t i=0; i<NPadeChan(); i++){
    int idx=padeChannel[i].GetChannelIndex();
    float val=padeChannel[i].GetMax()-padeChannel[i].GetPedistal();  // replace w/ fit!!!
    if (calconstants) val*=calconstants[idx];     // relative calibration

    // Hack here to fix bad channel
    // bad channel 11316, set equal to 11608 (set rear fiber to front fiber)
    // padechannels are ordered according to boardID*100+channelID
    // so 11316 = padeChannel[48],  11608 = padeChannel[104]
    if (i==48){
      val=padeChannel[104].GetMax()-padeChannel[104].GetPedistal();
    }

    calHits.push_back( CalHit(idx, val) );
  }
}
