#include "shashlik.h"



Mapper::Mapper(){  // Private so that it cannot be called
  // fill maps
  for (int i=0; i<NPADECHANNELS*2; i+=2){
    int channelID=FIBERMAP[i];
    int fiberID=FIBERMAP[i+1];
    if (_padeMap.find(channelID)!=_padeMap.end()) 
      cout << "_padeMap: duplicate entry:" << channelID << endl; 
    else
      _padeMap[channelID]=fiberID;
    _fiberMap[fiberID]=channelID;
  }
}




// return X-Y coordinate of module from upstream view
void Mapper::ModuleXY(int module, float &x, float &y) const {
  module=TMath::Abs(module);
  if ( module <1 || module>16 ){ x=-999; y=-999; }
  else {
    module--;
    x=MODULEXY[TMath::Abs(module)*3+1];
    y=MODULEXY[TMath::Abs(module)*3+2];
  }
}


// return X-Y coordinate of fiber from upstream view
void Mapper::FiberXY(int fiberID, float &x, float &y) const{
  Bool_t isUpstream=(fiberID<0);
  int module=fiberID/100;
  int fiber=TMath::Abs(fiberID-module*100);
  ModuleXY(module,x,y);
  if (fiber==1) {x+=FIBER_OFFSET_X; y-=FIBER_OFFSET_Y;}
  else if (fiber==2) {x+=FIBER_OFFSET_X; y+=FIBER_OFFSET_Y;}
  else if ( (isUpstream) &&  // handle case of swapped fibers in cookie B
	    (module==1||module==2||module==13||module==15) ){
    if (fiber==3) {x-=FIBER_OFFSET_X; y-=FIBER_OFFSET_Y;}
    if (fiber==4) {x-=FIBER_OFFSET_X; y+=FIBER_OFFSET_Y;}
  }
  else {
    if (fiber==3) {x-=FIBER_OFFSET_X; y+=FIBER_OFFSET_Y;}
    if (fiber==4) {x-=FIBER_OFFSET_X; y-=FIBER_OFFSET_Y;}
  }
}

void Mapper::SetModuleBins(TH2 *h) const{
  h->SetBins(4,MIN_EDGE_X,MAX_EDGE_X,4,MIN_EDGE_Y,MAX_EDGE_Y);
  h->SetXTitle("X [mm]");
  h->SetYTitle("Y [mm]");
}
void Mapper::SetChannelBins(TH2 *h) const{
  h->SetBins(8,MIN_EDGE_X,MAX_EDGE_X,8,MIN_EDGE_Y,MAX_EDGE_Y);
  h->SetXTitle("X [mm]");
  h->SetYTitle("Y [mm]");
}
void Mapper::GetModuleMap(TH2I* h, int z) const{
  h->Reset();
  SetModuleBins(h);
  if (z==-1) h->SetTitle("Modules UpStream;X [mm];Y [mm]");
  else h->SetTitle("Modules DownStream;X [mm];Y [mm]");
  float x,y;
  for (int i=1; i<=NMODULES; i++){
    ModuleXY(i,x,y);
    h->Fill(x,y,i*z);
  }
}

void Mapper::GetChannelMap(TH2I* h, int z) const{;
  h->Reset();
  SetChannelBins(h);
  float x,y;
  int channelID,fiberID;
  for (int i=0; i<NPADECHANNELS/2; i++){
    if (z==1){
      // downstream channels
      h->SetTitle("Channels DownStream;X [mm];Y [mm]");
      channelID=FIBERMAP[i*4];
      fiberID=FIBERMAP[i*4+1];
    }
    else{
      // upstream channels
      h->SetTitle("Channels UpStream;X [mm];Y [mm]");
      channelID=FIBERMAP[i*4+2];
      fiberID=FIBERMAP[i*4+3];
    }
    FiberXY(fiberID, x, y); 
    h->Fill(x,y,channelID);
  }
}



Mapper* Mapper::_pInstance=NULL;
