#include "Mapper.h"

// defaulf map based on initial 4 PADE cards of July 2014 Test Beam  
Mapper::Mapper(){  // Private so that it cannot be called
  FIBERMAP=FIBERMAP_JULY14;
  // fill maps
  MakeMaps();
}

void Mapper::SetEpoch(unsigned long ts){  // redundant data here, clean this up
  if (ts<=635337576077954884L && FIBERMAP!=FIBERMAP_APRIL14){
    FIBERMAP=FIBERMAP_APRIL14;
    MakeMaps(); 
  }
  else if (ts>635337576077954884L && FIBERMAP!=FIBERMAP_JULY14){
    FIBERMAP=FIBERMAP_JULY14;
    MakeMaps(); 
  }
}

void Mapper::MakeMaps(){
  _padeMap.clear();
  _fiberMap.clear();
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

bool Mapper::validChannel(int boardID, int channelNum) const{
  int channelID=boardID*100+channelNum;
  std::map<int,int>::const_iterator search = _padeMap.find(channelID);
  if (search == _padeMap.end()) return false;
  return true;
}

int Mapper::ChannelID2FiberID(int channelID) const {
  std::map<int,int>::const_iterator search=_padeMap.find(channelID);
  if  (search == _padeMap.end()) return 0;
  return search->second; 
}

void Mapper::ChannelID2ModuleFiber(int channelID, int &moduleID, int &fiberID) const{
  std::map<int,int>::const_iterator search=_padeMap.find(channelID);
  fiberID=search->second;
  moduleID=fiberID/100;
}

void Mapper::ChannelIndex2ModuleFiber(int channelIndex, int &moduleID, int &fiberID) const{
  int channelID=ChannelIndex2ChannelID(channelIndex);
  std::map<int,int>::const_iterator search=_padeMap.find(channelID);
  fiberID=search->second;
  moduleID=fiberID/100;
}


int Mapper::ChannelID2ChannelIndex(int channelID) const {
  int module,fiberID;
  ChannelID2ModuleFiber(channelID,module,fiberID);
  int fiber=TMath::Abs(fiberID)%5;
  if (module<0) return 4*(TMath::Abs(module)-1) + (fiber-1);  // front/upsteam channels
  else return 64 + 4*(module-1) + (fiber-1);                  // rear/downstream 
}

int Mapper::ChannelIndex2ChannelID(int channelIndex) const{
  int fiber, module, fiberID;
  fiber=(channelIndex%4)+1;
  module=(channelIndex/4)+1;
  if (module<=16) fiberID=-1*(module*100+fiber);
  else fiberID=(module-16)*100+fiber;
  return FiberID2ChannelID(fiberID);
}

int Mapper::FiberID2ChannelID(int fiberID) const {
  std::map<int,int>::const_iterator search =_fiberMap.find(fiberID);
  if  (search == _fiberMap.end()) return 0;
  return search->second;
}


// return X-Y coordinate of module from upstream view
void Mapper::ModuleXY(int module, double &x, double &y) const {
  module=TMath::Abs(module);
  if ( module <1 || module>16 ){ x=-999; y=-999; }
  else {
    module--;
    x=MODULEXY[TMath::Abs(module)*3+1];
    y=MODULEXY[TMath::Abs(module)*3+2];
  }
}


// return X-Y coordinate of fiber from upstream view
void Mapper::FiberXY(int fiberID, double &x, double &y) const{
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

void Mapper::ChannelXYZ(int channelID, double &x, double &y, double &z) const{
  int fiberID=ChannelID2FiberID(channelID);
  FiberXY(fiberID,x,y);
  if (fiberID<0) z=-1;
  else z=1;
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
  if (z==-1) h->SetTitle("Module IDs UpStream;X [mm];Y [mm]");
  else h->SetTitle("Module IDs DownStream;X [mm];Y [mm]");
  double x,y;
  for (int i=1; i<=NMODULES; i++){
    ModuleXY(i,x,y);
    h->Fill(x,y,i*z);
  }
}

void Mapper::GetChannelMap(TH2I* h, int z) const{
  h->Reset();
  SetChannelBins(h);
  double x,y;
  int channelID,fiberID;
  for (int i=0; i<NPADECHANNELS/2; i++){
    if (z==1){
      // downstream channels
      h->SetTitle("Channel IDs DownStream;X [mm];Y [mm]");
      channelID=FIBERMAP[i*4];
      fiberID=FIBERMAP[i*4+1];
    }
    else{
      // upstream channels
      h->SetTitle("Channel IDs UpStream;X [mm];Y [mm]");
      channelID=FIBERMAP[i*4+2];
      fiberID=FIBERMAP[i*4+3];
    }
    FiberXY(fiberID, x, y); 
    h->Fill(x,y,channelID);
  }
}


void Mapper::GetChannelIdx(TH2I* h, int z) const{
  h->Reset();
  h->SetMinimum(-1);
  SetChannelBins(h);
  double x,y;
  int channelID,fiberID;
  for (int i=0; i<NPADECHANNELS/2; i++){
    if (z==1){
      // downstream channels
      h->SetTitle("Channel Idx DownStream;X [mm];Y [mm]");
      channelID=FIBERMAP[i*4];
      fiberID=FIBERMAP[i*4+1];
    }
    else{
      // upstream channels
      h->SetTitle("Channel Idx UpStream;X [mm];Y [mm]");
      channelID=FIBERMAP[i*4+2];
      fiberID=FIBERMAP[i*4+3];
    }
    FiberXY(fiberID, x, y); 
    int index=ChannelID2ChannelIndex(channelID);
    h->Fill(x,y,index);
  }
}

void CalHit::GetXYZ(double &x, double &y, double &z) const{
   Mapper *mapper=Mapper::Instance();
   int channelID=mapper->ChannelIndex2ChannelID(_channelIndex);
   mapper->ChannelXYZ(channelID,x,y,z);
}

void CalHit::Print() const{
  double x,y,z;
  GetXYZ(x,y,z);
  cout << "Calhit (index,x,y,z,val) = ( " << _channelIndex << "," 
       << x << "," << y << "," << z << "," << _val << " )" << endl;
}


Mapper* Mapper::_pInstance=NULL;
const int *Mapper::FIBERMAP=0;
