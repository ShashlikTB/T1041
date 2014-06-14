#ifndef WC_H
#define WC_H

////////////////////////////////////////////
// Wire chamber information
////////////////////////////////////////////
/*  Mapping
  shashlik   WC2   WC1   SC2     SC1         SC0 (position unknown)
 +Z <---------------0---------------------------------

 Use LH coordinte system with +Z=downstream, +Y=up, +X to the right 
 when facing downstream

 (0,0,0) is defined at center of wire chamber 1 (WC1)

 TDC to WC mapping 
 X wires                  Y wires
  1      1X [1-64]         3      1Y [1-64]
  2      1X [65-128]       4      1Y [65-128]
  5      2X [1-64]         7      2Y [1-64]
  6      2X [65-128]       8      2Y [65-128]
  9      3X [1-64]        11      3Y [1-64]
  10     3X [65-128]      12      3Y [65-128]
  13     4X [1-64]        15      4Y [1-64]
  14     4X [65-128]      16      4Y [65-128]
 */



const float tdcRange     = 13.0;      // Originally it was 8.0

const int NTDC=16;

// origin = location of WC 1, downstream is positive z
const float zWC1=0;
const float zWC2=3476.625;

const float dWC1toWC2     = 3476.625;   // Distance between WC1 and WC2     (mm)
const float dWC2toShash   = 533.4;      // Distance between WC2 and Shashlik(mm)
const float dScin1toWC1   = 1231.9;     // Distance between Scint 1 and WC1 (mm)
const float dWC1toScin1   = 1231.9;
const float dScin1toScin2 = 4445.0;     //        ""    Scint 1 and Scint 2 (mm)

// derived distances
const float zShash = zWC2+dWC2toShash;
const float zSC1 = zWC1-dScin1toWC1;
const float zSC2 = zSC1-dScin1toScin2;

int tdc2WC(int tdcNum);

#endif
