#ifndef PADECHANNEL_H 
#define PADECHANNEL_H

#include <TH1F.h>
#include "pulseShapeForFit.h"


/// TODO ad these as status data members
const Int_t N_PADE_SAMPLES=120;     // fixed in FW
//const Int_t PADE_THRESHOLD=100;
const Int_t PADE_PED_SAMPLES=10;

// approximate sample times for board 112, 113, 115, 116  !! too hacky, needs improvement
const Int_t PADE_SAMPLE_TIMES[4]={27,21,14,17};
const Int_t PADE_SAMPLE_RANGE=3;  // +-3 count window = ~5sigma


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
  UInt_t GetChannelNum() {return _ch_number;}
  UInt_t GetChannelID() {return _board_id*100+_ch_number;}
  Int_t GetChannelIndex();  // index 0--127, following Ledovskoy convention
  UShort_t* GetWform() {return _wform;}
  UInt_t GetMax() {return _max;}
  float GetMaxCalib();
  Int_t GetPeak() {return _peak;}
  Int_t __SAMPLES() const {return  N_PADE_SAMPLES;}
  void GetXYZ(double &x, double &y, double &z);
  void GetPedestal(double &ped, double &stdev);
  void GetHist(TH1F* h);
  static PulseFit FitPulse(PadeChannel *pc, bool laserShape=false);
  double GetPedestal();
  // private:
  ULong64_t     _ts;
  UShort_t      _transfer_size;
  UShort_t      _board_id;
  UInt_t        _hw_counter ;
  UInt_t        _ch_number;
  UInt_t        _eventnum;
  UShort_t      _wform[N_PADE_SAMPLES];
  UInt_t        _max;    // max ADC sample
  Int_t         _peak;   // sample number for peak
  Int_t         _flag;
/*
  // pulse fit results
  double _fitPed;
  double _errPed;
  double _fitMax;
  double _errMax;
  double _fitRise;
  double _errRise;
  double _noise;
  double _chi2;
*/
};


#endif
