#ifndef SHASHLIK_H
#define SHASHLIK_H
#include <map>
#include <cstdlib>
#include <iostream>
using namespace std;

const int NMODULES=16;
const int MODULEXY[]={
   1, 1, 4,
   2, 2, 4,
   3, 2, 3,
   4, 1, 3,
   5, 3, 4,
   6, 3, 3,
   7, 3, 2,
   8, 2, 2,
   9, 1, 2,
   10,4, 4, 
   11,4, 3,
   12,4, 2,
   13,4, 1,
   14,3, 1,
   15,2, 1,
   16,1, 1
};


// look up table of detector fibers to pade channels
// naming conventions
//   PADE channels - boardID*100+channel_number
//   FIBER numbers - moduleID*100+fiber_number (+/- for front/back)
const int NPADECHANNELS=128;
// board/chan, module/fiber(Front)   board/chan,module/fiber(Rear)
const int FIBERMAP[]={
11206,	101,		11501,	-901,
11222,	102,		11517,	-902,
11207,	103,		11500,	-903,
11223,	104,		11516,	-904,
11220,	201,		11519,	-801,
11204,	202,		11503,	-802,
11221,	203,		11518,	-803,
11205,	204,		11502,	-804,
11219,	501,		11520,	-701,
11203,	502,		11504,	-702,
11218,	503,		11521,	-703,
11202,	504,		11505,	-704,
11201,	1001,		11506,	-1201,
11217,	1002,		11522,	-1202,
11200,	1003,		11507,	-1203,
11216,	1004,		11523,	-1204,
11231,	401,		11524,	-1601,
11215,	402,		11508,	-1602,
11230,	403,		11525,	-1603,
11214,	404,		11509,	-1604,
11213,	301,		11510,	-1501,
11229,	302,		11526,	-1502,
11212,	303,		11511,	-1503,
11228,	304,		11527,	-1504,
11210,	601,		11513,	-1401,
11226,	602,		11529,	-1402,
11211,	603,		11512,	-1403,
11227,	604,		11528,	-1404,
11224,	1101,		11531,	-1301,
11208,	1102,		11515,	-1302,
11225,	1103,		11530,	-1303,
11209,	1104,		11514,	-1304,
11606,	901,		11301,	-901,
11622,	902,		11317,	-902,
11607,	903,		11300,	-903,
11623,	904,		11316,	-904,
11620,	801,		11319,	-801,
11604,	802,		11303,	-802,
11621,	803,		11318,	-803,
11605,	804,		11302,	-804,
11619,	701,		11320,	-701,
11603,	702,		11304,	-702,
11618,	703,		11321,	-703,
11602,	704,		11305,	-704,
11601,	1201,		11306,	-1201,
11617,	1202,		11322,	-1202,
11600,	1203,		11307,	-1203,
11616,	1204,		11323,	-1204,
11631,	1601,		11324,	-1601,
11615,	1602,		11308,	-1602,
11630,	1603,		11325,	-1603,
11614,	1604,		11309,	-1604,
11613,	1501,		11310,	-1501,
11629,	1502,		11326,	-1502,
11612,	1503,		11311,	-1503,
11628,	1504,		11327,	-1504,
11610,	1401,		11313,	-1401,
11626,	1402,		11329,	-1402,
11611,	1403,		11312,	-1403,
11627,	1404,		11328,	-1404,
11624,	1301,		11331,	-1301,
11608,	1302,		11315,	-1302,
11625,	1303,		11330,	-1303,
11609,	1304,		11314,	-1304};

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
  void ModuleXY(int module, int &x, int &y) const {
    module=abs(module);
    if ( module <1 || module>16 ){ x=0; y=0; }
    else {
      module--;
      x=MODULEXY[abs(module)*3+1];
      y=MODULEXY[abs(module)*3+2];
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
  /// convert board ID and channel number to module and fiber  (negative module # is rear side)
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
