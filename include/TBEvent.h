//Created 4/12/2014 B.Hirosky: Initial release

#ifndef TBEVENT_H
#define TBEVENT_H
#include "PadeChannel.h"
#include "Mapper.h"
#include <vector>

using std::vector;

class PadeHeader : public TObject{
  ClassDef(PadeHeader,1); 
 public:
  PadeHeader(){;}
 PadeHeader(Bool_t master, UShort_t board, UShort_t stat,
	    UShort_t tstat, UShort_t events, UShort_t mreg,
	    UShort_t pTrg, UShort_t pTmp, UShort_t sTmp, UShort_t gain):
  _isMaster(master), _boardID(board), _status(stat), _trgStatus(tstat), 
    _events(events), _memReg(mreg), _trigPtr(pTrg), 
    _pTemp(pTmp), _sTemp(sTmp), _gain(gain), _bias(0){;}
  
  /// PADE gain in packed format
  /** 
      Packing format LNA [bits 1:0]  PGA [bits 3:2]  VGA [bits 15:4]
   **/
  UShort_t Gain() const {return _gain;}
  UShort_t BoardID() const {return _boardID;}
  UShort_t Events() const {return _events;}
  Bool_t IsMaster() const {return _isMaster;}
  UShort_t PadeTemp() const {return _pTemp;}
  UShort_t SipmTemp() const {return _sTemp;}

 private:
  Bool_t _isMaster;
  UShort_t _boardID;
  UShort_t _status;
  UShort_t _trgStatus;
  UShort_t _events;
  UShort_t _memReg;
  UShort_t _trigPtr;
  UShort_t _pTemp;    ///< temperature on PADE board
  UShort_t _sTemp;    ///< temperature on SIPM board
  UShort_t _gain;     ///< LNA [bits 1:0]  PGA [bits 3:2]  VGA [bits 15:4]
  UShort_t _bias;     ///< main bias setting
};

/// for now ASSUME we are only dealing with WC1 and WC2
class WCChannel : public TObject{
  ClassDef(WCChannel,1); 
 public:
  WCChannel(){;}
  WCChannel(UChar_t num, UChar_t wire, UShort_t count) :
  _tdcNumber(num), _tdcWire(wire), _tdcCount(count){;}
  void Dump() const;

  // getters
  UChar_t GetTDCNum() const {return _tdcNumber;}
  UChar_t GetWire() const {return _tdcWire;}
  UShort_t GetCount() const {return _tdcCount;}
  float GetX();
  float GetY();
 private:
  UChar_t       _tdcNumber;			
  UChar_t       _tdcWire;			
  UShort_t      _tdcCount;
};

/// Container for spill-related data
/**
   Container for spill-related data
   Beam types are given usin PDG ID's
    11 : electron
   -11 : positron
    12 : muon
    211 : pion
    2212 : proton
    -22 : Laser
 **/
class TBSpill : public TObject {
  ClassDef(TBSpill,1);  // Spill header info
public:
 TBSpill(Int_t spillNumber=0, ULong64_t pcTime=0, Int_t nTrigWC=0, ULong64_t wcTime=0, 
	 Int_t pdgID=0, Float_t nomMomentum=0,
	 Float_t tableX=-999, Float_t tableY=-999, Float_t angle=0, 
	 Float_t boxTemp=0, Float_t roomTemp=0) : 
  _spillNumber(spillNumber), _pcTime(pcTime), 
    _nTrigWC(nTrigWC), _wcTime(wcTime), _pdgID(pdgID), _nomMomentum(nomMomentum),
    _tableX(tableX), _tableY(tableY), _angle(angle), 
    _boxTemp(boxTemp), _roomTemp(roomTemp) {;}
  Int_t GetSpillNumber() const {return _spillNumber;}
  ULong64_t GetPCTime() const {return _pcTime;}
  Int_t GetnTrigWC() const {return _nTrigWC;}
  ULong64_t GetWCTime() const {return _wcTime;}
  Float_t GetTableX() const {return _tableX;}
  Float_t GetTableY() const {return _tableY;}
  Float_t GetAngle() const {return _angle;}
  Int_t GetPID() const {return _pdgID;}
  Float_t GetMomentum() const {return _nomMomentum;}
  Int_t NPades() const {return _padeHeader.size();}
  void Dump() const;
  /// index 0:n-1 
  PadeHeader const* GetPadeHeader(Int_t i) {
    if (i<NPades()) return &(_padeHeader[i]);
    return 0;
  }
  // setters
  void Reset();
  void SetSpillData(Int_t spillNumber, ULong64_t pcTime, Int_t nTrigWC, ULong64_t wcTime,
		    Int_t pdgID=0, Float_t nomMomentum=0, 
		    Float_t tableX=-999, Float_t tableY=-999, Float_t angle=0,
		    Float_t boxTemp=0, Float_t roomTemp=0);
  void SetSpillNumber(Int_t s) {_spillNumber=s;}
  void SetPCTime(ULong64_t t) {_pcTime=t;}
  void SetnTrigWC(Int_t n) {_nTrigWC=n;}
  void SetWCTime(ULong64_t t) {_wcTime=t;}
  void AddPade(PadeHeader pade){_padeHeader.push_back(pade);}
private:
  Int_t         _spillNumber;              ///< spill # counted by PADE
  ULong64_t     _pcTime;                   ///< spill time stamp from PC
  Int_t         _nTrigWC;                  ///< triggers reported by WC
  ULong64_t     _wcTime;                   ///< WC time read by PADE PC  
  vector<PadeHeader> _padeHeader;
  // beam and detector parameters
  Int_t         _pdgID;                    ///< particle ID for beam
  Float_t       _nomMomentum;              ///< beam momentum setting
  Float_t       _tableX;                   ///< table position
  Float_t       _tableY;                   ///< table position
  Float_t       _angle;                    ///< table angle
  Float_t       _boxTemp;                  ///< temperature in environmental box
  Float_t       _roomTemp;                 ///< temperature in test beam area
};

/// Storage container for raw data from test beam
class TBEvent : public TObject {
  ClassDef(TBEvent,1);  //Event structure
public:
  enum TBRun { 
    TBRun1=0,   ///< April 2014
    TBRun2a=1,  ///< Start of July-Aug 2014 run (32 ADC samples used for porch)
    TBRun2b=2,  ///< Final July-Aug 2014 cfg. (15 ADC samples used for porch)
    TBRun2c=3,  ///< More precise WC time stamps in PADE DAQ
    TBUndef=10
  };
  /// earliest TB data run
  static const ULong64_t START_TBEAM1=635321637512389603L;   
  /// end of TBRun1
  static const ULong64_t END_TBEAM1=635337576077954884L;      
  /// beginning of TBRun2b
  static const ULong64_t START_PORCH15=635421671607690753L;   
  /// More precise end of spill time reported
  /** Mod of PADE WC Spill time stamp.  Previously, reported end of spill + PADE RO time.
      Now end of spill time reported.  This is 0-1 seconds behind spill time reported by WC DAQ **/
  static const ULong64_t START_NEWWCSYNC=635432861909176340L;  
  static const ULong64_t END_TBEAM2=635440566331915360L;

  void Reset();    // clear data

  // getters (tbd - return (const) references, not copies, where appopriate)
  Int_t NPadeChan() const {return padeChannel.size();}
  PadeChannel GetPadeChan(const int idx) const {return padeChannel[idx];}
  PadeChannel GetLastPadeChan() const {return padeChannel.back();}
  WCChannel GetWCChan(const int idx) {return wc[idx];}
  Int_t GetWCHits() const {return wc.size();}
  vector<WCChannel> GetWChitsX(Int_t wc, Int_t *min=0, Int_t* max=0) const;
  vector<WCChannel> GetWChitsY(Int_t wc, Int_t *min=0, Int_t* max=0) const;
  static TBRun GetRunPeriod(ULong64_t padeTime);
  TBRun GetRunPeriod() const;
  void GetCalHits(vector<CalHit> &calHits, float* calconstants=0, float cut=0);
  void GetCalHitsFit(vector<CalHit> &calHits, float* calconstants=0, float cut=0);
  void CalibrateCalHits(vector<CalHit> &calHits, float* calconstants);


  // setters
  void SetPadeChannel(const PadeChannel p, Int_t i) {padeChannel[i]=p;}
  void FillPadeChannel(ULong64_t ts, UShort_t transfer_size, 
		       UShort_t  board_id, UInt_t hw_counter, 
		       UInt_t ch_number,  UInt_t eventnum, Int_t *wform, Bool_t isLaser=false);
  void AddWCHit(UChar_t num, UChar_t wire, UShort_t count);


private:
  vector<PadeChannel> padeChannel;
  vector<WCChannel> wc; 
};



#endif
