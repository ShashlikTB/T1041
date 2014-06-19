#ifndef WCPlanes_H
#define WCPlanes_H
#include "TBEvent.h"
#include "TCanvas.h"
#include "TBReco.h"
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
  WCReco wcReco;
  TH2I *WC1_Beam;
  TH2I *WC2_Beam;
  TH2F *WC_Shashlik;
  TH2I *WC1;
  TH2I *WCO1;
  TH2I *WC2; 
  TH2I *WCO2;
  THStack *hStack1;
  THStack *hStack2;
  Int_t WireChamber1Hit, WireChamber2Hit;
  Int_t X_pos_WC1, X_pos_WC2;
  Int_t Y_pos_WC1, Y_pos_WC2;
  Int_t X_tim_WC1, X_tim_WC2;
  Int_t Y_tim_WC1, Y_tim_WC2;

  int mean[NTDC];
  int tLow[NTDC]; 
  int tHigh[NTDC]; 
  int tdcRange;

  bool isFirstEvent;

};
#endif
