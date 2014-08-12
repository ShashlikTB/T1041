#ifndef PADECHANNEL_H 
#define PADECHANNEL_H

#include <TH1F.h>
#include "pulseShapeForFit.h"

  // approximate sample times for board 112, 115, 116, 117  
  // !! too hacky, needs improvement
  static const Int_t PADE_SAMPLE_TIMES[4]={31,29,36,32};  // peaking time, defined after porch!
  const Int_t PADE_SAMPLE_RANGE=4; // +-4 count window b/c above is guesstimate

class PadeChannel : public TObject {
  ClassDef(PadeChannel,1); 
 public:
  void Fill(ULong64_t ts, UShort_t transfer_size, 
	    UShort_t  board_id, UInt_t hw_counter, 
	    UInt_t ch_number,  UInt_t eventnum, Int_t *wform, Bool_t isLaser=false);
  void Reset();
  void Dump() const;

  // getters
  ULong64_t GetTimeStamp() {return _ts;}  ///< C# time in pade channel data
  UInt_t GetBoardID() {return _board_id;}
  UInt_t GetChannelNum() {return _ch_number;}
  UInt_t GetChannelID() {return _board_id*100+_ch_number;}
  Int_t GetChannelIndex();  ///< index 0--127, following Ledovskoy convention
  UShort_t* GetWform() {return _wform;}
  UInt_t GetMax() {return _max;}  ///< NOT PEDESTAL Subtracted!
  float GetMaxCalib();            ///< PEDESTAL Subtracted!
  Int_t GetPeak() {return _peak;}
  Int_t __SAMPLES() const {return N_PADE_SAMPLES;}
  Int_t __DATASIZE() const {return N_PADE_DATA;}
  void GetXYZ(double &x, double &y, double &z);
  /// Return pedesdal and its sigma.  
  /** This method does the calculation.  The first 10 wave form samples are used.**/
  void GetPedestal(double &ped, double &stdev);
  Double_t GetPedestal() {return _ped;}
  Double_t GetPedSigma() {return _pedsigma;}
  Double_t GetAmplitude() {return _max-_ped;}
  void GetHist(TH1F* h);
  Bool_t LaserData(){return _status & kLaser;}
  static PulseFit FitPulse(PadeChannel *pc);
  int GetPorch(ULong64_t ts=0) const;
  void SetAsLaser();
  

  static const Int_t N_PADE_DATA=120;     ///< fixed in FW
  static const Int_t N_PADE_PORCH=15;     ///< diagnostic info in data payload
  static Int_t N_PADE_SAMPLES;
  static const Int_t PADE_PED_SAMPLES=20;

  /// PadeChannel flags
  enum Flags {
    kPorch0=0,   ///< no porch
    kPorch15=1,  ///< 15 sample porch
    kPorch32=2,  ///< 32 sample porch 
    kLaser=16,   ///< Laser data flag
  };

  // private:
  ULong64_t     _ts;
  UShort_t      _transfer_size;
  UShort_t      _board_id;
  UInt_t        _hw_counter ;
  UInt_t        _ch_number;
  UInt_t        _eventnum;
  UShort_t      _wform[N_PADE_DATA];
  UInt_t        _max;    // max ADC sample
  Float_t       _ped;
  Float_t       _pedsigma;
  Int_t         _peak;   // sample number for peak
  UShort_t      _status;
};


#endif
