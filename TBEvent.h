#ifndef TBEVENT_H
#define TBEVENT_H

const Int_t N_PADE_CHANNELS=128;    // n.b. enlarge later for Chrenkov data
const Int_t N_PADE_SAMPLES=120;     // fixed in FW



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
  Int_t __SAMPLES() const {return  N_PADE_SAMPLES;}
  Int_t GetModule();
  Int_t GetFiber();

  // private:
  ULong64_t     _ts;
  UShort_t      _transfer_size;
  UShort_t      _board_id ;
  UInt_t        _hw_counter ;
  UInt_t        _ch_number;
  UInt_t        _eventnum;
  UShort_t      _wform[N_PADE_SAMPLES];
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


class TBEvent : public TObject {
  ClassDef(TBEvent,1);  //Event structure
public:
  void ResetData();
  void cp(const TBEvent &e);  // copy constructor interfce for python, ugh!

  // getters
  Int_t NPadeChan() const {return padeChannel.size();}
  PadeChannel GetPadeChan(const int idx) const {return padeChannel[idx];}
  PadeChannel GetLastPadeChan() const {return padeChannel.back();}
  Int_t GetSpillNumber() const {return spillNumber;}
  ULong64_t GetPCTime() const {return pcTime;}
  ULong64_t GetSpillTime() const {return spillTime;}
  WCChannel GetWCChan(const int idx) {return wc[idx];}
  Int_t GetWCHits() const {return wc.size();}


  // setters
  void SetSpill(Int_t s) {spillNumber=s;}
  void SetPCTime(ULong64_t t) {pcTime=t;}
  void SetSpillTime(ULong64_t t) {spillTime=t;}
  void SetEventNumber(Int_t n) {eventNumber=n;}
  void SetPadeChannel(const PadeChannel p, Int_t i) {padeChannel[i]=p;}
  void FillPadeChannel(ULong64_t ts, UShort_t transfer_size, 
		       UShort_t  board_id, UInt_t hw_counter, 
		       UInt_t ch_number,  UInt_t eventnum, Int_t *wform);
  void AddWCHit(UChar_t num, UChar_t wire, UShort_t count);


private:
  Int_t         spillNumber;
  ULong64_t     pcTime;                   // spill time stamp from PC
  ULong64_t     spillTime;                // spill time stamp from WC controller
  Int_t         eventNumber;              // from pade
  vector<PadeBoard> padeBoard;
  vector<PadeChannel> padeChannel;
  vector<WCChannel> wc;   
};


#endif
