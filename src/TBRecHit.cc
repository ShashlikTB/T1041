#include "TBRecHit.h"
#include "Mapper.h"


TBRecHit::TBRecHit(PadeChannel *pc){
  Clear();
  if (pc) Init(pc);
}

void TBRecHit::Init(PadeChannel *pc){
  SetChannelIndex(pc->GetChannelIndex());
  if (_status&kNoFit) return;
}

void TBRecHit::Clear(Option_t *o){
  TObject::Clear(o);
  _channelIndex=-1;
  _maxADC=-1;
  _pedestal=-999;
  _noise=-999;
  _aMaxValue=-999;
  _tRiseValue=-1;
  _aMaxError=0;
  _tRiseError=0;
  _ndof=0;
  _status=0;
}

void TBRecHit::GetXYZ(double &x, double &y, double &z) const {
  Mapper *mapper=Mapper::Instance();
  int channelID=mapper->ChannelIndex2ChannelID(_channelIndex);
  mapper->ChannelXYZ(channelID,x,y,z);
}
void TBRecHit::GetXYZ(float &x, float &y, float &z) const {
  double p[3];
  GetXYZ(p[0],p[1],p[2]);
  x=p[0];
  y=p[1];
  z=p[2];
}

void TBRecHit::FitPulse(PadeChannel *pc){
  PulseFit fit=PadeChannel::FitPulse(pc);
  _pedestal=fit.pedestal;
  _noise=fit.noise;
  _aMaxValue=fit.aMaxValue;
  _tRiseValue=fit.tRiseValue;
  _chi2=fit.chi2Peak;
  _ndof=fit.ndofPeak;
  if (fit.status>0) _status|=kPoorFit;
}

std::ostream& operator<<(std::ostream& s, const TBRecHit& hit) {
  double x,y,z;
  hit.GetXYZ(x,y,z);
  return s << "TBRecHit (index,x,y,z,aMax,tRise) " 
	   << hit.ChannelIndex() << "," << x << "," << y << "," << z
	   << hit.AMax() << "," << hit.TRise();
}
