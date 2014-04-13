// Created 4/12/2014 B.Hirosky: Initial release

#ifndef SHASHLIK_H
#define SHASHLIK_H
#include "TMath.h"
#include <map>
#include <cstdlib>
#include <iostream>
using namespace std;

const int NMODULES=16;
const int MODULEXY[]={
   1, 4, 4,
   2, 3, 4,
   3, 3, 3,
   4, 4, 3,
   5, 2, 4,
   6, 2, 3,
   7, 2, 2,
   8, 3, 2,
   9, 4, 2,
   10,1, 4, 
   11,1, 3,
   12,1, 2,
   13,1, 1,
   14,2, 1,
   15,3, 1,
   16,4, 1
};


// look up table of detector fibers to pade channels
// naming conventions
//   PADE channels - boardID*100+channel_number
//   FIBER numbers - moduleID*100+fiber_number (+/- for front/back)
const int NPADECHANNELS=128;
// board/chan	module/fiber(Down Str)	board/chan   module/fiber(UP Stream)
const int FIBERMAP[]={
11501,	1001,		11206,	-1001,
11517,	1002,		11222,	-1002,
11500,	1003,		11207,	-1004,
11516,	1004,		11223,	-1003,
11519,	501,		11220,	-501,
11503,	502,		11204,	-502,
11518,	503,		11221,	-503,
11502,	504,		11205,	-504,
11520,	201,		11219,	-201,
11504,	202,		11203,	-202,
11521,	203,		11218,	-203,
11505,	204,		11202,	-204,
11506,	101,		11201,	-101,
11522,	102,		11217,	-102,
11507,	103,		11200,	-103,
11523,	104,		11216,	-104,
11524,	1101,		11231,	-1101,
11508,	1102,		11215,	-1102,
11525,	1103,		11230,	-1104,
11509,	1104,		11214,	-1103,
11510,	601,		11213,	-601,
11526,	602,		11229,	-602,
11511,	603,		11212,	-603,
11527,	604,		11228,	-604,
11513,	301,		11210,	-301,
11529,	302,		11226,	-302,
11512,	303,		11211,	-303,
11528,	304,		11227,	-304,
11531,	401,		11224,	-401,
11515,	402,		11208,	-402,
11530,	403,		11225,	-403,
11514,	404,		11209,	-404,
11301,	1201,		11606,	-1201,
11317,	1202,		11622,	-1202,
11300,	1203,		11607,	-1203,
11316,	1204,		11623,	-1204,
11319,	701,		11620,	-701,
11303,	702,		11604,	-702,
11318,	703,		11621,	-703,
11302,	704,		11605,	-704,
11320,	801,		11619,	-801,
11304,	802,		11603,	-802,
11321,	803,		11618,	-803,
11305,	804,		11602,	-804,
11306,	901,		11601,	-901,
11322,	902,		11617,	-902,
11307,	903,		11600,	-903,
11323,	904,		11616,	-904,
11324,	1301,		11631,	-1301,
11308,	1302,		11615,	-1302,
11325,	1303,		11630,	-1303,
11309,	1304,		11614,	-1304,
11310,	1401,		11613,	-1401,
11326,	1402,		11629,	-1402,
11311,	1403,		11612,	-1404,
11327,	1404,		11628,	-1403,
11313,	1501,		11610,	-1501,
11329,	1502,		11626,	-1502,
11312,	1503,		11611,	-1503,
11328,	1504,		11627,	-1504,
11331,	1601,		11624,	-1601,
11315,	1602,		11608,	-1602,
11330,	1603,		11625,	-1604,
11314,	1604,		11609,	-1603};


// bad channel map (PADE Channel, UTC time remove, UTC time return)
const unsigned long BADCHANMAP[]={11613, 1397186943, 0}; // stuck lower bit(s)


// mapping functions
class Mapper{
 public:
  static Mapper* Instance(){
    if (!_pInstance)   // Only allow one instance of class to be generated.
      _pInstance = new Mapper;
    return _pInstance;
  }

  // return X-Y coordiante of module starting for up stream view
  // for plotting in TH2 w/ bins (4,0.5,4.5,4,0.5,4.5)
  void ModuleXY(int module, int &x, int &y) const {
    module=TMath::Abs(module);
    if ( module <1 || module>16 ){ x=0; y=0; }
    else {
      module--;
      x=MODULEXY[TMath::Abs(module)*3+1];
      y=MODULEXY[TMath::Abs(module)*3+2];
    }
  }
  // return X-Y coordiante of module starting for up stream view
  // for plotting in TH2 w/ bins (8,0.5,4.5,8,0.5,4.5)
  void FiberXY(int fiberID, float &x, float &y) const {
    Bool_t isUpstream=(fiberID<0);
    int module=fiberID/100;
    int fiber=TMath::Abs(fiberID-module*100);
    int ix,iy;
    ModuleXY(module,ix,iy);
    x=ix; y=iy;
    if (fiber==1) {x+=0.25; y-=0.25;}
    else if (fiber==2) {x+=0.25; y+=0.25;}
    else if ( (isUpstream) &&  // handle case of swapped fibers in cookie B
	      (module==1||module==2||module==13||module==15) ){
      if (fiber==3) {x-=0.25; y-=0.25;}
      if (fiber==4) {x-=0.25; y+=0.25;}
    }
    else {
      if (fiber==3) {x-=0.25; y+=0.25;}
      if (fiber==4) {x-=0.25; y-=0.25;}
    }
  }

  int Pade2Fiber(int channelID) {
    if  (_padeMap.find(channelID) == _padeMap.end()) return 0;
    return _padeMap[channelID]; 
  }
  bool validChannel(int boardID, int channelNum){
    int channelID=boardID*100+channelNum;
    if (_padeMap.find(channelID) == _padeMap.end())return false;
    return true;
  }
  /// convert board ID and channel number to module and fiber (negative module # is rear side)
  void Pade2Fiber(int boardID, int channelNum, int &moduleNum, int &fiberNum) {
    int channelID=boardID*100+channelNum;
    int fiberID=Pade2Fiber(channelID);
    moduleNum=fiberID/100;
    fiberNum=fiberID-moduleNum*100;
  }
  int Fiber2Pade(int fiberID)  {
    if  (_fiberMap.find(fiberID) == _fiberMap.end()) return 0;
    return _fiberMap[fiberID];
  }
  void Fiber2Pade(int moduleNum, int fiberNum, int &boardID, int &channelNum) {
    int fiberID=moduleNum*100+fiberNum;
    int channelID=Fiber2Pade(fiberID);
    boardID=channelID/100;
    channelNum=channelID-boardID*100;
  }
 private:
  static Mapper* _pInstance;
  map<int,int> _padeMap;  // map pade channels to fibers
  map<int,int> _fiberMap; // map fiber to pade channel

  Mapper(){  // Private so that it can  not be called
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
  Mapper(Mapper const&){;}              // copy constructor is private
};

Mapper* Mapper::_pInstance=NULL;

#endif
