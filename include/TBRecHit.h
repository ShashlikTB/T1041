// Created 7/8/2014 B.Hirosky: Initial release
// based on CMSSW EcalRecHit

#ifndef TBRECHIT_H
#define TBRECHIT_H

#include "PadeChannel.h"
#include <TObject.h>
#include <ostream>

class TBRecHit : public TObject {
  ClassDef(TBRecHit,1);
 public:
   // recHit flags
  enum Flags { 
    kGood=0,  // channel ok, the energy,time,pedistal measurements are reliable
    kNoFit=1,  // the pulse fit is skipped, use simple peak finder only 
    kPoorFit=2, // energy,time,pedistal approximate (bad shape, large chi2)
    kFaultyHardware=4,  // channel is faulty at some hardware level
    kNoisy=8,          // the channel is very noisy
    kSaturated=16,     // saturated channel
    kPileup=32,        // multiple peaks detected
    kMirrored=64,      // channel was replaced w/ TBRecHit from opposing channel
    kZSP=128,          // signal is below ZSP threshold
    kUnknown=256           
  };
  TBRecHit(PadeChannel *pc=0, Float_t zsp=0, UInt_t options=0);
  void Init(PadeChannel *pc=0,  Float_t zsp=0);
  Int_t ChannelIndex() const {return _channelIndex;}
  void FitPulse(PadeChannel *pc);
  void GetXYZ(double &x, double &y, double &z) const;
  void GetXYZ(float &x, float &y, float &z) const;
  unsigned short MaxADC() const {return _maxADC;}
  Float_t AMax() const {return _aMaxValue;}
  Float_t TRise() const {return _tRiseValue;}
  Float_t Pedestal() const {return _pedestal;}
  Float_t NoiseRMS() const {return _noise;}
  Float_t Chi2() const {return _chi2;}
  UInt_t Status() const {return _status;}
  void SetChannelIndex(Int_t idx) {_channelIndex=idx;} 
  void SetOptNoFit() {_status&=kNoFit;}
 private:
  Short_t _channelIndex;   // channel index, S.L. convention
  UShort_t _maxADC;        // max value of ADC samples (in expected signal region)
  Float_t _pedestal;       // from average of samples before peak
  Float_t _noise;          // RMS of pedistal samples
  Float_t _aMaxValue;      // Peak from fit to pulse shape
  Float_t _tRiseValue;
  Float_t _aMaxError;      // beginning of pulse from fit
  Float_t _tRiseError;    
  Float_t _chi2;            
  Float_t _ndof;
  Float_t _zsp;            // nSigma zero suppression
  UInt_t _status;
};
	       
std::ostream& operator<<(std::ostream& s, const TBRecHit& hit);

#endif


