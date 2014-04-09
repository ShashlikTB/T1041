#include "makeHistograms.C"

void createPlots() {

  gROOT->Reset();
  gROOT->SetBatch(true);
  gROOT->SetStyle("Plain");
  gStyle->SetOptStat(0000);

  bool twoPeaksPerTrigger = true;

  makeHistograms("0ns_LED_HighEndBias.root", twoPeaksPerTrigger);
  makeHistograms("0ns_LED_LowEndBias.root", twoPeaksPerTrigger);
  makeHistograms("0ns_LED_PeakBias.root", twoPeaksPerTrigger);

  vector<TString> fNames, legendTitles;

  fNames.push_back("hist_0ns_LED_HighEndBias.root");
  fNames.push_back("hist_0ns_LED_LowEndBias.root");
  fNames.push_back("hist_0ns_LED_PeakBias.root");

  legendTitles.push_back("HighEnd Bias");
  legendTitles.push_back("LowEnd Bias");
  legendTitles.push_back("Peak Bias");

  TString legendHeader = "No Light";

  overlayPlots(fNames, legendTitles, legendHeader);

}
