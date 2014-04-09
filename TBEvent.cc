
#include <iostream>
#include <stdio.h>
#include "TTree.h"
#include "TFile.h"
#include "shashlik.h"

#include "TBEvent.h"

using namespace std;
void TBEvent::ResetData(){
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
  PadeChannel pc;  // todo make consructor w/ fill inputs
  pc.Fill(ts, transfer_size, board_id, hw_counter, ch_number, eventnum, wform);
  padeChannel.push_back(pc);
}


void TBEvent::AddWCHit(UChar_t num, UChar_t wire, UShort_t count){
  WCChannel wctmp(num,wire,count);
  wc.push_back(wctmp);
}

void TBEvent::cp(const TBEvent &e){
  spillNumber = e.spillNumber;
  pcTime = e.pcTime;
  spillTime = e.spillTime;
  eventNumber = e.eventNumber;
  padeChannel = e.padeChannel;
  wc = e.wc;
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
  for (int i=0; i<N_PADE_SAMPLES; i++) _wform[i]=wform[i];
  _flag = 0;
}

void PadeChannel::Reset(){
  _ts=0;
  _transfer_size=0;
  _board_id=0;
  _hw_counter=0;
  _ch_number=0;
  _eventnum=0;
  for (int i=0; i<N_PADE_SAMPLES; i++) _wform[i]=0;
}

Int_t PadeChannel::GetModule(){
  return 0;
}
Int_t PadeChannel::GetFiber(){return 0;}


void WCChannel::Dump() const {
  cout << "TDC# " << (int)_tdcNumber <<  " Wire " << (int)_tdcWire 
       << " Count " << (int)_tdcCount << endl;
}

