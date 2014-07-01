// Created 4/12/2014 B.Hirosky: Initial release

#ifndef SHASHLIK_H
#define SHASHLIK_H
#include "TMath.h"
#include <TH2I.h>
#include <map>
#include <cstdlib>
#include <iostream>
using namespace std;

/* Module x,y positions are relative to the center of the detector 
   Looking downstream: +x is to the right, +y is up
*/

const int NMODULES=16;

// locations in mm, from center of detector
const double MODULEXY[]={
   1, 21.0, 21.0,
   2, 7.0, 21.0,
   3, 7.0,  7.0,
   4, 21.0, 7.0,
   5, -7.0, 21.0,
   6, -7.0,  7.0,
   7, -7.0, -7.0,
   8,  7.0, -7.0,
   9,  21.0, -7.0,
   10,-21.0, 21.0, 
   11,-21.0, 7.0,
   12,-21.0, -7.0,
   13,-21.0, -21.0,
   14, -7.0, -21.0,
   15,  7.0, -21.0,
   16, 21.0, -21.0
};

// x,y offsets of fiber placements from center of module
const float FIBER_OFFSET_X=3.5;
const float FIBER_OFFSET_Y=3.5;
const float MIN_EDGE_X=-28;
const float MIN_EDGE_Y=-28;
const float MAX_EDGE_X=28;
const float MAX_EDGE_Y=28;

// look-up table of detector fibers to pade channels
// naming conventions
//   PADE channels - boardID*100+channel_number
//   FIBER numbers - moduleID*100+fiber_number (+/- for front/back)
const int NPADECHANNELS=128;
// board/chan	module/fiber(Down Str)	board/chan   module/fiber(UP Stream)
// ChannelID    FiberID                 ChannelID    FiberID                            
const int FIBERMAP[]={
11506,	1001,		11230,	-1001,
11504,	1002,		11228,	-1002,
11503,	1003,		11227,	-1004,
11501,	1004,		11225,	-1003,
11514,	501,		11222,	-501,
11512,	502,		11220,	-502,
11511,	503,		11219,	-503,
11509,	504,		11217,	-504,
11522,	201,		11214,	-201,
11520,	202,		11212,	-202,
11519,	203,		11211,	-203,
11517,	204,		11209,	-204,
11530,	101,		11206,	-101,
11528,	102,		11204,	-102,
11527,	103,		11203,	-103,
11525,	104,		11201,	-104,
11500,	1101,		11224,	-1101,
11502,	1102,		11226,	-1102,
11507,	1103,		11231,	-1104,
11505,	1104,		11229,	-1103,
11508,	601,		11216,	-601,
11510,	602,		11218,	-602,
11515,	603,		11223,	-603,
11513,	604,		11221,	-604,
11516,	301,		11208,	-301,
11518,	302,		11210,	-302,
11523,	303,		11215,	-303,
11521,	304,		11213,	-304,
11524,	401,		11200,	-401,
11526,	402,		11202,	-402,
11531,	403,		11207,	-403,
11529,	404,		11205,	-404,
11306,	1201,		11630,	-1201,
11304,	1202,		11628,	-1202,
11303,	1203,		11627,	-1203,
11301,	1204,		11625,	-1204,
11314,	701,		11622,	-701,
11312,	702,		11620,	-702,
11311,	703,		11619,	-703,
11309,	704,		11617,	-704,
11322,	801,		11614,	-801,
11320,	802,		11612,	-802,
11319,	803,		11611,	-803,
11317,	804,		11609,	-804,
11330,	901,		11606,	-901,
11328,	902,		11604,	-902,
11327,	903,		11603,	-903,
11325,	904,		11601,	-904,
11300,	1301,		11624,	-1301,
11302,	1302,		11626,	-1302,
11307,	1303,		11631,	-1303,
11305,	1304,		11629,	-1304,
11308,	1401,		11616,	-1401,
11310,	1402,		11618,	-1402,
11315,	1403,		11623,	-1404,
11313,	1404,		11621,	-1403,
11316,	1501,		11608,	-1501,
11318,	1502,		11610,	-1502,
11323,	1503,		11615,	-1503,
11321,	1504,		11613,	-1504,
11324,	1601,		11600,	-1601,
11326,	1602,		11602,	-1602,
11331,	1603,		11607,	-1604,
11329,	1604,		11605,	-1603};



// bad channel map (PADE Channel, UTC time remove, UTC time return)
const unsigned long BADCHANMAP[]={11316, 1397186943, 0}; // stuck lower bit(s)


// mapping functions
class Mapper{
 public:
  static Mapper* Instance(){
    if (!_pInstance)   // Only allow one instance of class to be generated.
      _pInstance = new Mapper;
    return _pInstance;
  }
  void ModuleXY(int module, double &x, double &y) const;
  void FiberXY(int fiberID, double &x, double &y) const; 
  void ChannelXYZ(int channelID, double &x, double &y, double &z) const;

  void SetModuleBins(TH2 *h) const;
  void SetChannelBins(TH2 *h) const;
  void GetModuleMap(TH2I* h, int z=1 /*-1 for upstream*/ ) const;
  void GetChannelMap(TH2I* h, int z=1 /*-1 for upstream*/ ) const;
  void GetChannelIdx(TH2I* h, int z=1 /*-1 for upstream*/ ) const;

  bool validChannel(int boardID, int channelNum) const;
  int ChannelID2FiberID(int channelID) const;
  int FiberID2ChannelID(int fiberID) const;
  void ChannelID2ModuleFiber(int channelID, int &moduleID, int &fiberID) const;
  int ChannelID2ChannelIndex(int channelID) const;
  int ChannelIndex2ChannelID(int channelIndex) const;


  int ChannelID2FiberID(int channelID) {
    if  (_padeMap.find(channelID) == _padeMap.end()) return 0;
    return _padeMap[channelID]; 
  }

  double* ChannelID2XY(int chanID) {
    int fiberID = ChannelID2FiberID(chanID);
    double x, y;
    static double xy[2];
    FiberXY(fiberID, x, y);
    xy[0] = x;
    xy[1] = y;
    return xy;
  }

 private:
  static Mapper* _pInstance;
  map<int,int> _padeMap;  // map pade channels to fibers
  map<int,int> _fiberMap; // map fiber to pade channel

  Mapper();
  Mapper(Mapper const&){;}              // copy constructor is private
};

class CalHit{
 public:
 CalHit(int idx, double val): _channelIndex(idx), _val(val) {;}
  int GetChannelIndex() const {return _channelIndex;}
  double Value() const {return _val;}
  void GetXYZ(double &x, double &y, double &z) const;
  void Print() const;
  void SetValue(double val) {_val=val;}
 private:
  int _channelIndex;
  double _val;
};


#endif
