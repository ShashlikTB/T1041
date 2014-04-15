#include "makeHistograms.C"
#include "findPeaks.C"

void createPlots() {

  gROOT->Reset();
  gROOT->SetBatch(true);
  gROOT->SetStyle("Plain");
  gStyle->SetOptStat(0000);

  bool twoPeaksPerTrigger = true;

  makeHistograms("0ns_HighEndBias.root", twoPeaksPerTrigger);
  makeHistograms("0ns_LowEndBias.root", twoPeaksPerTrigger);
  makeHistograms("0ns_PeakBias.root", twoPeaksPerTrigger);

  makeHistograms("20ns_HighEndBias.root", twoPeaksPerTrigger);
  makeHistograms("20ns_LowEndBias.root", twoPeaksPerTrigger);
  makeHistograms("20ns_PeakBias.root", twoPeaksPerTrigger);

  vector<TString> fNames, legendTitles;

  fNames.push_back("hist_0ns_HighEndBias.root");
  fNames.push_back("hist_0ns_LowEndBias.root");
  fNames.push_back("hist_0ns_PeakBias.root");

  legendTitles.push_back("HighEnd Bias");
  legendTitles.push_back("LowEnd Bias");
  legendTitles.push_back("Peak Bias");

  TString legendHeader = "No Light";

  overlayPlots(fNames, legendTitles, legendHeader);

  for(unsigned int i = 0; i < fNames.size(); i++) findPeaks(fNames[i]);

  calibrationData();

}
