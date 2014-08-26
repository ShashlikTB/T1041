#include "TBRecHit.h"
#include "Mapper.h"


TBRecHit::TBRecHit(PadeChannel *pc, Float_t zsp, UInt_t options){
  nzsp=zsp;
  if (options&kNoFit) {
    status|=kNoFit;
    Init(pc,false);
  }
  else Init(pc);
}

TBRecHit::TBRecHit(const TBRecHit &hit, UShort_t idx, UInt_t newstatus) :
  TObject(hit),
  maxADC(hit.maxADC), pedestal(hit.pedestal),
  noise(hit.noise), aMaxValue(hit.aMaxValue), tRiseValue(hit.tRiseValue),
  chi2(hit.chi2), ndof(hit.ndof), nzsp(hit.nzsp), status(hit.status|newstatus),
  cfactor(hit.cfactor), ts(hit.ts)
{
  if (idx<=127) channelIndex=idx;
}

void TBRecHit::Init(PadeChannel *pc,  Float_t zsp){
  channelIndex=-1;
  maxADC=-1;
  pedestal=-999;
  noise=-999;
  aMaxValue=-999;
  tRiseValue=-1;
  aMaxError=0;
  tRiseError=0;
  ndof=0;
  status=0;
  nzsp=zsp;
  cfactor=1;
  if (!pc) return;
  channelIndex=pc->GetChannelIndex();
  ts=pc->GetTimeStamp();
  maxADC=pc->GetMax();
  double ped,sig;
  pc->GetPedestal(ped,sig);
  pedestal=ped;
  noise=sig;
  if (status&kNoFit) return;
  FitPulse(pc);
}


void TBRecHit::GetXYZ(double &x, double &y, double &z) const {
  Mapper *mapper=Mapper::Instance(ts);
  mapper->ChannelIdxXYZ(channelIndex,x,y,z); // does not depend on run epoch
}

void TBRecHit::GetXYZ(float &x, float &y, float &z) const {
  double p[3];
  GetXYZ(p[0],p[1],p[2]);
  x=p[0];
  y=p[1];
  z=p[2];
}


Int_t TBRecHit::GetChannelID() const{  // depends on run epoch b/c of PADE id's
  Mapper *mapper=Mapper::Instance(ts);
  return mapper->ChannelIndex2ChannelID(channelIndex);
}

void TBRecHit::GetModuleFiber(int &moduleID, int &fiberID) const{
  Mapper *mapper=Mapper::Instance(ts);  
  mapper->ChannelIndex2ModuleFiber(channelIndex,moduleID,fiberID); // result independent of run epoch
}

void TBRecHit::FitPulse(PadeChannel *pc){
  if ( TMath::Abs(maxADC-pedestal) / (noise+0.001) < nzsp ) { // avoid div by 0
    status|=kZSP;
    return;
  }
  PulseFit fit=PadeChannel::FitPulse(pc);
  pedestal=fit.pedestal;
  noise=fit.noise;
  aMaxValue=fit.aMaxValue;
  tRiseValue=fit.tRiseValue;
  aMaxError=fit.aMaxError;
  tRiseError=fit.tRiseError;
  chi2=fit.chi2Peak;
  ndof=fit.ndofPeak;
  if (fit.status>0) status|=kPoorFit;
}

std::ostream& operator<<(std::ostream& s, const TBRecHit& hit) {
  double x,y,z;
  hit.GetXYZ(x,y,z);
  return s << "TBRecHit (index,x,y,z,noise,aMax,tRise,Chi2) " 
	   << hit.ChannelIndex() << "," << x << "," << y << "," << z<< ","
	   << hit.NoiseRMS() << "," << hit.AMax() << "," 
	   << hit.TRise() << "," << hit.Chi2();
}

void TBRecHit::Calibrate(float *calconstants){
  float c=calconstants[channelIndex];
  if (IsCalibrated()) c/=cfactor;  // undo pevious calibration
  aMaxValue*=c;
  aMaxError*=c;
  noise*=c;
  cfactor=calconstants[channelIndex];
}
void TBRecHit::Calibrate(vector<TBRecHit> *rechits, float *calconstants){
  for (unsigned i=0; i<rechits->size(); i++)
    (rechits->at(i)).Calibrate(calconstants);
}

