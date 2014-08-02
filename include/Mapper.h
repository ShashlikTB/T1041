// Created 4/12/2014 B.Hirosky: Initial release

#ifndef SHASHLIK_H
#define SHASHLIK_H
#include "TMath.h"
#include "fibermap.h"
#include <TH2I.h>
#include <map>
#include <cstdlib>
#include <iostream>
using namespace std;

/* Overview of Mapping for the Shashlik Testbeam
There are many mapping schemes (alas!)

ModuleID:
  Run from -16...-1 (upstream face) 1..16 (downstream face)
FiberNumber 1..4 
  Numbered CCW from the lower right corner of a module viewed from upstream
FiberID: 
  = ModuleID*100-FiberNumber [upstream face]
  = ModuleID*100+FiberNumber [downstream face]
Pade BoardID = 112,115,116,117 for testbeam2
Pade ChannelNumber 0..31 ADC channels (groups of 8 correspond to physical ADCs)
ChannelID:
  = BoardID*100+ChannelNumber
ChannelIndex 0..127 (good for histogram x-axis)
  = -1*ModuleID+(FiberNumber-1)  [upstream face]
  = ModuleID+(FiberNumber-1)     [downstream face]
 */


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

// x,y offsets of fiber placements from center of module (not acurate!)
const float FIBER_OFFSET_X=3.5;
const float FIBER_OFFSET_Y=3.5;
const float MIN_EDGE_X=-28;
const float MIN_EDGE_Y=-28;
const float MAX_EDGE_X=28;
const float MAX_EDGE_Y=28;


// bad channel map (PADE Channel, UTC time remove, UTC time return)
// not yet defined for testbeam2
// const unsigned long BADCHANMAP[]={11316, 1397186943, 0}; // stuck lower bit(s)


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
  void ChannelIndex2ModuleFiber(int channelIndex, int &moduleID, int &fiberID) const;
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
