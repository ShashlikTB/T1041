// Created 7/8/2014 B.Hirosky: Initial release

#ifndef TBRECHIT_H
#define TBRECHIT_H

#include "PadeChannel.h"
#include "TBEvent.h"
#include "TMath.h"
#include <TObject.h>
#include <ostream>
#include <vector>

using std::vector;

/// based on CMSSW EcalRecHit
/**
   A simple class to provide reconstucted calorimeter pulses.<br>
 **/

class TBRecHit : public TObject {
  ClassDef(TBRecHit,2);
 public:
   /// recHit flags (mostly not implemented yet!)
  enum Flags { 
    kGood=0,       ///< channel ok, the energy,time,pedistal measurements are reliable
    kNoFit=1,      ///< the pulse fit is skipped, use simple peak finder only 
    kPoorFit=2,    ///< energy,time,pedistal approximate (bad shape, large chi2)
    kFaultyHardware=4,  ///< channel is faulty at some hardware level
    kNoisy=8,           ///< the channel is very noisy
    kSaturated=16,      ///< saturated channel
    kPileup=32,         ///< multiple peaks detected
    kMirrored=64,       ///< channel was replaced w/ TBRecHit from opposing channel
    kZSP=128,           ///< signal is below ZSP threshold
    kMonitor=256,       ///< monitor for laser pulse, no calorimeter connection
    kCalibrated=512,    ///< set if calibration applied
    kUnknown=2<<31      ///< set for weirdness
  };
  /// Constructor
  TBRecHit(PadeChannel *pc=0, Float_t zsp=0, UInt_t options=0);
  /// Alternate copy constructor: useful for mirroring dead channels 
  TBRecHit(const TBRecHit &hit, UShort_t idx, UInt_t newstatus);
  void Init(PadeChannel *pc=0,  Float_t zsp=0);
  Int_t ChannelIndex() const {return channelIndex;} ///< Channel indx [0..127]
  Int_t GetChannelID() const;  ///< board_id*100+ch_number
  Int_t GetBoardID() const {return  GetChannelID()/100;} ///< PADE board ID
  /// Channel number in PADE board [0...31]
  Int_t GetChannelNumber() const {return GetChannelID()-GetBoardID()*100;}
  /// Module +/-[1...16] for rear/front face and fiber [1...4] indicies 
  void GetModuleFiber(int &moduleID, int &fiberID) const;
  /// Channel X,Y,Z
  void GetXYZ(double &x, double &y, double &z) const;
  /// Channel X,Y,Z
  void GetXYZ(float &x, float &y, float &z) const;
  /// Max ADC reading around expected peak location
  unsigned short MaxADC() const {return maxADC;}
  void AddStatus(enum Flags flag) {status|=flag;} ///< Add bit(s) to status flag
  void SetStatus(unsigned flags) {status=flags;}  ///< Set the status flag
  Float_t AMax() const {return aMaxValue;} ///< Amplitude fom pulse fit
  Float_t TRise() const {return tRiseValue;} ///< Starting location of pulse
  Float_t Pedestal() const {return pedestal;}
  Float_t NoiseRMS() const {return noise;} ///< RMS noise of pedestal
  Float_t Chi2() const {return chi2;} ///< chi^2 fit calculated in pulse region
  Float_t NZsp() const {return nzsp;} ///< 0-suppression applied, n-sigma noise
  Float_t Ndof() const {return ndof;} ///< ndof for fit in peak region
  Float_t Prob() const {return TMath::Prob(chi2,ndof);} ///< chi^2 p-value
  UInt_t Status() const {return status;}  ///< status word
  void SetOptNoFit() {status&=kNoFit;}  ///< not implemented
  bool IsCalibrated() const {return status&kCalibrated;} 
  Float_t CalFactor() const {return cfactor;} ///< return cailbration factor
  void Calibrate(float *calconstants); ///< apply calibration from array
 ///< calibrate all rechits 
  static void Calibrate(vector<TBRecHit> *rechits, float *calconstants);
 private:
  void FitPulse(PadeChannel *pc);

  UShort_t channelIndex;   ///< channel index, S.L. convention
  UShort_t maxADC;         ///< max value of ADC samples (in expected signal region)
  Float_t pedestal;        ///< from average of samples before peak
  Float_t noise;           ///< RMS of pedestal samples
  Float_t aMaxValue;       ///< Peak from fit to pulse shape
  Float_t tRiseValue;      ///< beginning of pulse from fit
  Float_t aMaxError;       
  Float_t tRiseError;    
  Float_t chi2;            ///< chi2 calculated in the peak region (chi2Peak)
  Float_t ndof;            ///< ndof calculated in the peak region (ndofPeak)
  Float_t nzsp;            ///< nSigma zero suppression applied in creating TBecHit
  UInt_t status;           ///< see definition under TBRecHit::Flags
  Float_t cfactor;         ///< applied calibration factor
  ULong64_t ts;            ///< timestamp from PADE channel
};
	       
std::ostream& operator<<(std::ostream& s, const TBRecHit& hit);

#endif


