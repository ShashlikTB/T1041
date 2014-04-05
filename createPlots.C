#include "makeHistograms.C"

void createPlots() {

  gROOT->Reset();
  gROOT->SetBatch(true);
  gROOT->SetStyle("Plain");
  gStyle->SetOptStat(0000);

  makeHistograms("rec_capture_20140404_185141.root");
  makeHistograms("rec_capture_20140404_194302.root");

  vector<TString> fNames, legendTitles;
  fNames.push_back("hist_rec_capture_20140404_185141.root");
  fNames.push_back("hist_rec_capture_20140404_194302.root");

  legendTitles.push_back("15 ns");
  legendTitles.push_back("20 ns");

  overlayPlots(fNames, legendTitles);

}
