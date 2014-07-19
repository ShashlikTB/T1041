#include "TBRecHit.h"
#include "Mapper.h"


TBRecHit::TBRecHit(){
  Clear();
}

void TBRecHit::Clear(Option_t *o){
  _channelIndex=-1;
  _maxADC=-1;
  _pedistal=-999;
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



std::ostream& operator<<(std::ostream& s, const TBRecHit& hit) {
  double x,y,z;
  hit.GetXYZ(x,y,z);
  return s << "TBRecHit (index,x,y,z,aMax,tRise) " 
	   << hit.ChannelIndex() << "," << x << "," << y << "," << z
	   << hit.AMax() << "," << hit.TRise();
}
