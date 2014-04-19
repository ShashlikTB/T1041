// Created 4/12/2014 B.Hirosky: Initial release


#ifndef TBEVENT_H
#define TBEVENT_H

#include "TH1F.h"

const Int_t N_PADE_SAMPLES=120;     // fixed in FW
const Int_t PADE_THRESHOLD=100;

class PadeChannel : public TObject {
  ClassDef(PadeChannel,1); 
 public:
  void Fill(ULong64_t ts, UShort_t transfer_size, 
	    UShort_t  board_id, UInt_t hw_counter, 
	    UInt_t ch_number,  UInt_t eventnum, Int_t *wform);
  void Reset();
  void Dump() const;

  // getters
  UInt_t GetBoardID() {return _board_id;}
  UInt_t GetChannelID() {return _ch_number;}
  UShort_t* GetWform() {return _wform;}
  UInt_t GetMax() {return _max;}
  Int_t __SAMPLES() const {return  N_PADE_SAMPLES;}
  Int_t GetModule();
  Int_t GetFiber();
  void GetHist(TH1F* h);

  // private:
  ULong64_t     _ts;
  UShort_t      _transfer_size;
  UShort_t      _board_id ;
  UInt_t        _hw_counter ;
  UInt_t        _ch_number;
  UInt_t        _eventnum;
  UShort_t      _wform[N_PADE_SAMPLES];
  UInt_t        _max;
  Int_t         _flag;
};

class PadeBoard : public TObject{
  ClassDef(PadeBoard,1); 
 public:
  PadeBoard(){;}
 PadeBoard(Bool_t master, UShort_t board, UShort_t stat,
	    UShort_t tstat, UShort_t events, UShort_t mreg,
	    UShort_t pTrg, UShort_t pTmp, UShort_t sTmp):
  _isMaster(master), _boardID(board), _status(stat), _trgStatus(tstat), 
    _events(events), _memReg(mreg), _trigPtr(pTrg), 
    _pTemp(pTmp), _sTemp(sTmp){;}

 private:
  Bool_t _isMaster;
  UShort_t _boardID;
  UShort_t _status;
  UShort_t _trgStatus;
  UShort_t _events;
  UShort_t _memReg;
  UShort_t _trigPtr;
  UShort_t _pTemp;
  UShort_t _sTemp;
};

class WCChannel : public TObject{
  ClassDef(WCChannel,1); 
 public:
  WCChannel(){;}
  WCChannel(UChar_t num, UChar_t wire, UShort_t count) :
  _tdcNumber(num), _tdcWire(wire), _tdcCount(count){;}
  void Dump() const;

  // getters
  UChar_t GetTDCNum() {return _tdcNumber;}
  UChar_t GetWire() {return _tdcWire;}
  UShort_t GetCount() {return _tdcCount;}
 private:
  UChar_t       _tdcNumber;			
  UChar_t       _tdcWire;			
  UShort_t      _tdcCount;
};


class TBSpill : public TObject {
  ClassDef(TBSpill,1);  // Spill header info
public:
 TBSpill(Int_t spillNumber=0, ULong64_t pcTime=0, ULong64_t wcReadTime=0, ULong64_t spillTime=0) : 
  _spillNumber(spillNumber), _pcTime(pcTime), _wcReadTime(wcReadTime), _spillTime(spillTime) {;}
  Int_t GetSpillNumber() const {return _spillNumber;}
  ULong64_t GetPCTime() const {return _pcTime;}
  ULong64_t GetWCReadTime() const {return _wcReadTime;}
  ULong64_t GetSpillTime() const {return _spillTime;}
  // setters
  void SetSpill(Int_t s) {_spillNumber=s;}
  void SetPCTime(ULong64_t t) {_pcTime=t;}
  void SetWCReadTime(ULong64_t t) {_wcReadTime=t;}
  void SetSpillTime(ULong64_t t) {_spillTime=t;}
private:
  Int_t         _spillNumber;
  ULong64_t     _pcTime;                   // spill time stamp from PC
  ULong64_t     _wcReadTime;               // WC time read by PADE PC  
  ULong64_t     _spillTime;                // spill time stamp from WC controller
  vector<PadeBoard> _padeBoard;
};


class TBEvent : public TObject {
  ClassDef(TBEvent,1);  //Event structure
public:
  void ResetData();
  void cp(const TBEvent &e);  // copy constructor interfce for python, ugh!

  // getters (tbd - return const references, not copies, where appopriate)
  Int_t NPadeChan() const {return padeChannel.size();}
  PadeChannel GetPadeChan(const int idx) const {return padeChannel[idx];}
  PadeChannel GetLastPadeChan() const {return padeChannel.back();}
  Int_t GetSpillNumber() const {return spillNumber;}
  WCChannel GetWCChan(const int idx) {return wc[idx];}
  Int_t GetWCHits() const {return wc.size();}
  TBSpill GetHeader() const {return header;}

  // setters
  void SetSpill(Int_t s) {spillNumber=s;}
  void SetNtrigWC(Int_t n) {nTrigWC=n;}
  void SetPadeChannel(const PadeChannel p, Int_t i) {padeChannel[i]=p;}
  void FillPadeChannel(ULong64_t ts, UShort_t transfer_size, 
		       UShort_t  board_id, UInt_t hw_counter, 
		       UInt_t ch_number,  UInt_t eventnum, Int_t *wform);
  void AddWCHit(UChar_t num, UChar_t wire, UShort_t count);
  void SetHeader(TBSpill hdr) {header=hdr;}

private:
  Int_t         spillNumber;
  Int_t         nTrigWC;
  vector<PadeChannel> padeChannel;
  vector<WCChannel> wc; 
  TBSpill       header;
};



#endif
