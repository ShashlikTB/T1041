// Created 7/8/2014 B.Hirosky: Initial release
// based on CMSSW EcalRecHit

#ifndef TBRECHIT_H
#define TBRECHIT_H

#include <TObject.h>
#include <ostream>

class TBRecHit : public TObject {
  ClassDef(TBRecHit,1);
 public:
   // recHit flags
  enum Flags { 
    kGood=0,  // channel ok, the energy,time,pedistal measurements are reliable
    kPoorFit, // energy,time,pedistal approximate (bad shape, large chi2)
    FaultyHardware,  // channel is faulty at some hardware level
    kNoisy,          // the channel is very noisy
    kSaturated,      // saturated channel
    kPileup,         // multiple peaks detected
    kMirrored,       // channel was replaced w/ TBRecHit form opposing channel
    kUnknown           
  };
  TBRecHit();
  void Clear(Option_t *o="");  // reset all values
  Int_t ChannelIndex() const {return _channelIndex;}
  void GetXYZ(double &x, double &y, double &z) const;
  void GetXYZ(float &x, float &y, float &z) const;
  float AMax() const {return _aMaxValue;}
  float TRise() const {return _tRiseValue;}
  unsigned short MaxADC() const {return _maxADC;}
  int status() const {return _status;}
  void SetChannelIndex(Int_t idx) {_channelIndex=idx;}
 private:
  Short_t _channelIndex;   // channel index, S.L. convention
  UShort_t _maxADC;        // max value of ADC samples (in expected signal region)
  Float_t _pedistal;       // from aveage of samples before peak
  Float_t _noise;          // RMS of pedistal samples
  Float_t _aMaxValue;      // Peak from fit to pulse shape
  Float_t _tRiseValue;
  Float_t _aMaxError;       // beginning of pulse from fit
  Float_t _tRiseError;    
  Float_t _chi2;            
  Float_t _ndof;
  Int_t _status;
};
	       
std::ostream& operator<<(std::ostream& s, const TBRecHit& hit);

#endif


