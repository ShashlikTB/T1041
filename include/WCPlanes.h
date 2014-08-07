#ifndef WCPlanes_H
#define WCPlanes_H
#include "TBEvent.h"
#include "TCanvas.h"
#include "TBTrack.h"
#include <TH2.h>
#include "THStack.h"
#include "Util.h"

class WCPlanes{
 public:
  WCPlanes();
  WCPlanes(TCanvas* canvas);
  ~WCPlanes();
  void Draw(TBEvent* event, Util& util);
  void GetWCMeans(string meanfile, int *tLow, int *mean, int *tHigh);
  void CacheWCMeans(string meanfile, string rootfilename);

 private:
  TCanvas* c1;
  vector<WCChannel> hitsX1, hitsY1, hitsX2, hitsY2;
  float TableX;
  float TableY;
  TH2F *Scint1;
  TH2F *Scint2;
  TH2F *Shashlik;

  WCReco wcReco;
  TH2I *WC1_Beam;
  TH2I *WC2_Beam;
  TH2I *WC1_hits;
  TH2I *WC2_hits;
  TH2F *WC_Shashlik;
  
  TH2I *hEmpty;
  THStack *hStack1;
  THStack *hStack2;


  int mean[NTDC];
  int tLow[NTDC]; 
  int tHigh[NTDC]; 



  TH1I* TDC[NTDC];
  TCanvas C;

  bool isFirstEvent;

};
#endif
