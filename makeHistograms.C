#include <vector>

using namespace std;

void makeHistograms(TString input, bool twoPeaksPerTrigger){

  if (!TClassTable::GetDict("TBEvent")) {
    gSystem->Load("TBEvent_cc.so");  // n.b. make sure to compile if changed
  }

  TFile *f = new TFile(input);
  // create a pointer to an event object for reading the branch values.
  TBEvent *event = new TBEvent();

  TBranch *bevent = BeamData->GetBranch("event");
  bevent->SetAddress(&event);

  char char[100];
  int boardIDs[4] = {112, 113, 115, 116};

  vector<TString> histNames;

  vector<TH1D*> peakHeights;
  peakHeights.resize(128);

  vector<TH1D*> firstPeakHeights;
  firstPeakHeights.resize(128);

  vector<TH1D*> secondPeakHeights;
  secondPeakHeights.resize(128);

  for(int i = 0; i < 4; i++) {
    for(int j = 0; j < 32; j++) {
      sprintf(char, "peakHeight_board-%d_chan-%d", boardIDs[i], j);
      histNames.push_back(TString(char));
    }
  }

  for(unsigned int ui = 0; ui < histNames.size(); ui++) {
    peakHeights[ui] = new TH1D(histNames[ui], histNames[ui]+";Peak Height in ADC Counts;Events", 800, 0, 800);
    peakHeights[ui]->Sumw2();

    firstPeakHeights[ui] = new TH1D("first_"+histNames[ui], "first_"+histNames[ui]+";Peak Height in ADC Counts;Events", 800, 0, 800);
    firstPeakHeights[ui]->Sumw2();

    secondPeakHeights[ui] = new TH1D("second_"+histNames[ui], "second_"+histNames[ui]+";Peak Height in ADC Counts;Events", 800, 0, 800);
    secondPeakHeights[ui]->Sumw2();
  }

  // loop over events
  for (int i=0; i< BeamData->GetEntries(); i++) {

    if(i % 1000 == 0) cout << "Processing event " << i << " / " << (int)BeamData->GetEntries() << endl;

    BeamData->GetEntry(i);

    // loop over PADE channels
    for (int j=0; j<event->NPadeChan(); j++){

      UShort_t firstPeak = 0;
      UShort_t secondPeak = 0;

      // loop over ADC samples

      int firstLow, firstHigh;
      int secondLow, secondHigh;
      if(event->GetPadeChan(j).GetBoardID() == 112) {
	firstLow = 32;
	firstHigh = 38;

	secondLow = 81;
	secondHigh = 87;
      }
      else if(event->GetPadeChan(j).GetBoardID() == 113) {
	firstLow = 22;
	firstHigh = 28;

	secondLow = 71;
	secondHigh = 77;
      }
      else if(event->GetPadeChan(j).GetBoardID() == 115) {
	firstLow = 19;
	firstHigh = 25;

	secondLow = 68;
	secondHigh = 74;
      }
      else if(event->GetPadeChan(j).GetBoardID() == 116) {
	firstLow = 26;
	firstHigh = 32;

	secondLow = 75;
	secondHigh = 81;
      }
      else continue;

      UShort_t * wform=event->GetPadeChan(j).GetWform();
      for (int k = 0; k < event->GetPadeChan(j).__SAMPLES(); k++){
        if(k >= firstLow && k <= firstHigh && wform[k] > firstPeak) firstPeak = wform[k];
        if(k >= secondLow && k <= secondHigh && wform[k] > secondPeak) secondPeak = wform[k];
      }
  
      peakHeights[j]->Fill(firstPeak);
      firstPeakHeights[j]->Fill(firstPeak);
      if(twoPeaksPerTrigger) {
	peakHeights[j]->Fill(secondPeak);
	secondPeakHeights[j]->Fill(secondPeak);
      }

    } // pade channels

  } // events

  TFile * out = new TFile("hist_"+input, "RECREATE");

  for(unsigned int ui = 0; ui < peakHeights.size(); ui++) {
    peakHeights[ui]->Write();
    firstPeakHeights[ui]->Write();
    secondPeakHeights[ui]->Write();
  }

  out->Write();
  out->Close();


}

void overlayPlots(vector<TString> fNames, vector<TString> legendTitles, TString legendHeader) {

  if(fNames.size() != legendTitles.size()) {
    cout << endl << "Give me the same number of files as titles!" << endl << endl;
    return;
  }

  if(fNames.size() == 0) {
    cout << endl << "No files, nothing to do!" << endl << endl;
    return;
  }

  if(fNames.size() > 6) {
    cout << endl << "Only 6 colors defined -- don't overlay so many!" << endl << endl;
    return;
  }

  vector<TFile*> files;
  files.resize(fNames.size());
  for(unsigned int ui = 0; ui < fNames.size(); ui++) files[ui] = new TFile(fNames[ui], "READ");

  char char[100];
  int boardIDs[4] = {112, 113, 115, 116};

  vector<TString> histNames;
  for(int i = 0; i < 4; i++) {
    for(int j = 0; j < 32; j++) {
      sprintf(char, "peakHeight_board-%d_chan-%d", boardIDs[i], j);
      histNames.push_back(TString(char));
    }
  }

  int goodColors[6] = {kBlack,
		       kRed,
		       kBlue,
		       8,
		       kAzure-2,
		       kCyan+3};

  TLegend * leg = new TLegend(0.50, 0.65, 0.85, 0.85, NULL, "brNDC");
  if(legendHeader != "") leg->SetHeader(legendHeader);
  leg->SetFillColor(0);
  leg->SetTextSize(0.028);

  TCanvas * canv = new TCanvas("canv", "Plot", 10, 10, 2000, 2000);
  canv->SetLogy(false);

  vector<TH1D*> peakHeights;

  for(unsigned int uChannel = 0; uChannel < 128; uChannel++) {

    for(unsigned int uFile = 0; uFile < fNames.size(); uFile++) peakHeights.push_back((TH1D*)files[uFile]->Get(histNames[uChannel]));
    
    peakHeights[0]->SetLineWidth(3);
    peakHeights[0]->SetLineColor(goodColors[0]);
    peakHeights[0]->Scale(1./peakHeights[0]->Integral());
    peakHeights[0]->Draw("hist");
    leg->AddEntry(peakHeights[0], legendTitles[0]+Form(" (%.0f events)", peakHeights[0]->GetEntries()), "LP");

    for(unsigned int uFile = 1; uFile < fNames.size(); uFile++) {
      peakHeights[uFile]->SetLineWidth(3);
      peakHeights[uFile]->SetLineColor(goodColors[uFile]);
      peakHeights[uFile]->Scale(1./peakHeights[uFile]->Integral());
      leg->AddEntry(peakHeights[uFile], legendTitles[uFile]+Form(" (%.0f events)", peakHeights[uFile]->GetEntries()), "LP");
      peakHeights[uFile]->Draw("same hist");
    }
    leg->Draw("same");

    canv->SaveAs(TString(peakHeights[0]->GetTitle()) + ".gif");

    leg->Clear();
    peakHeights.clear();

  }
    
  files.clear();

}

void calibrationData(int board) {

  TFile * f0_low = new TFile("peakSpacings_hist_0ns_LowEndBias.root", "READ");
  TFile * f0_peak = new TFile("peakSpacings_hist_0ns_PeakBias.root", "READ");
  TFile * f0_high = new TFile("peakSpacings_hist_0ns_HighEndBias.root", "READ");

  TFile * f20_low = new TFile("peakSpacings_hist_20ns_LowEndBias.root", "READ");
  TFile * f20_peak = new TFile("peakSpacings_hist_20ns_PeakBias.root", "READ");
  TFile * f20_high = new TFile("peakSpacings_hist_20ns_HighEndBias.root", "READ");

  TTree * tree20_low = (TTree*)f20_low->Get("tree");
  TTree * tree20_peak = (TTree*)f20_peak->Get("tree");
  TTree * tree20_high = (TTree*)f20_high->Get("tree");

  TTree * tree0_low = (TTree*)f0_low->Get("tree");
  TTree * tree0_peak = (TTree*)f0_peak->Get("tree");
  TTree * tree0_high = (TTree*)f0_high->Get("tree");

  Int_t boardID_20, npeaks_20, channel_20;
  Int_t boardID_0, npeaks_0, channel_0;

  Float_t spacing01_20, spacing12_20, spacing23_20;
  Float_t spacing01_0, spacing12_0, spacing23_0;

  tree20_low->SetBranchAddress("boardID", &boardID_20);
  tree20_low->SetBranchAddress("npeaks", &npeaks_20);
  tree20_low->SetBranchAddress("channel", &channel_20);
  tree20_low->SetBranchAddress("spacing01", &spacing01_20);
  tree20_low->SetBranchAddress("spacing12", &spacing12_20);
  tree20_low->SetBranchAddress("spacing23", &spacing23_20);
  tree20_peak->SetBranchAddress("boardID", &boardID_20);
  tree20_peak->SetBranchAddress("npeaks", &npeaks_20);
  tree20_peak->SetBranchAddress("channel", &channel_20);
  tree20_peak->SetBranchAddress("spacing01", &spacing01_20);
  tree20_peak->SetBranchAddress("spacing12", &spacing12_20);
  tree20_peak->SetBranchAddress("spacing23", &spacing23_20);
  tree20_high->SetBranchAddress("boardID", &boardID_20);
  tree20_high->SetBranchAddress("npeaks", &npeaks_20);
  tree20_high->SetBranchAddress("channel", &channel_20);
  tree20_high->SetBranchAddress("spacing01", &spacing01_20);
  tree20_high->SetBranchAddress("spacing12", &spacing12_20);
  tree20_high->SetBranchAddress("spacing23", &spacing23_20);

  tree0_low->SetBranchAddress("boardID", &boardID_0);
  tree0_low->SetBranchAddress("npeaks", &npeaks_0);
  tree0_low->SetBranchAddress("channel", &channel_0);
  tree0_low->SetBranchAddress("spacing01", &spacing01_0);
  tree0_low->SetBranchAddress("spacing12", &spacing12_0);
  tree0_low->SetBranchAddress("spacing23", &spacing23_0);
  tree0_peak->SetBranchAddress("boardID", &boardID_0);
  tree0_peak->SetBranchAddress("npeaks", &npeaks_0);
  tree0_peak->SetBranchAddress("channel", &channel_0);
  tree0_peak->SetBranchAddress("spacing01", &spacing01_0);
  tree0_peak->SetBranchAddress("spacing12", &spacing12_0);
  tree0_peak->SetBranchAddress("spacing23", &spacing23_0);
  tree0_high->SetBranchAddress("boardID", &boardID_0);
  tree0_high->SetBranchAddress("npeaks", &npeaks_0);
  tree0_high->SetBranchAddress("channel", &channel_0);
  tree0_high->SetBranchAddress("spacing01", &spacing01_0);
  tree0_high->SetBranchAddress("spacing12", &spacing12_0);
  tree0_high->SetBranchAddress("spacing23", &spacing23_0);
  
  TFile * out = new TFile("calibration_"+TString(Form("%d", board))+".root", "RECREATE");

  TH1D * h_seen_20_low_01 = new TH1D("seen_20_low_01", "seen_20_low_01", 100, 0, 100);
  TH1D * h_seen_20_low_12 = new TH1D("seen_20_low_12", "seen_20_low_12", 100, 0, 100);
  TH1D * h_seen_20_low_23 = new TH1D("seen_20_low_23", "seen_20_low_23", 100, 0, 100);
  TH1D * h_notSeen_20_low_01 = new TH1D("notSeen_20_low_01", "notSeen_20_low_01", 100, 0, 100);
  TH1D * h_notSeen_20_low_12 = new TH1D("notSeen_20_low_12", "notSeen_20_low_12", 100, 0, 100);
  TH1D * h_notSeen_20_low_23 = new TH1D("notSeen_20_low_23", "notSeen_20_low_23", 100, 0, 100);

  TH1D * h_seen_20_peak_01 = new TH1D("seen_20_peak_01", "seen_20_peak_01", 100, 0, 100);
  TH1D * h_seen_20_peak_12 = new TH1D("seen_20_peak_12", "seen_20_peak_12", 100, 0, 100);
  TH1D * h_seen_20_peak_23 = new TH1D("seen_20_peak_23", "seen_20_peak_23", 100, 0, 100);
  TH1D * h_notSeen_20_peak_01 = new TH1D("notSeen_20_peak_01", "notSeen_20_peak_01", 100, 0, 100);
  TH1D * h_notSeen_20_peak_12 = new TH1D("notSeen_20_peak_12", "notSeen_20_peak_12", 100, 0, 100);
  TH1D * h_notSeen_20_peak_23 = new TH1D("notSeen_20_peak_23", "notSeen_20_peak_23", 100, 0, 100);

  TH1D * h_seen_20_high_01 = new TH1D("seen_20_high_01", "seen_20_high_01", 100, 0, 100);
  TH1D * h_seen_20_high_12 = new TH1D("seen_20_high_12", "seen_20_high_12", 100, 0, 100);
  TH1D * h_seen_20_high_23 = new TH1D("seen_20_high_23", "seen_20_high_23", 100, 0, 100);
  TH1D * h_notSeen_20_high_01 = new TH1D("notSeen_20_high_01", "notSeen_20_high_01", 100, 0, 100);
  TH1D * h_notSeen_20_high_12 = new TH1D("notSeen_20_high_12", "notSeen_20_high_12", 100, 0, 100);
  TH1D * h_notSeen_20_high_23 = new TH1D("notSeen_20_high_23", "notSeen_20_high_23", 100, 0, 100);

  TH1D * h_byChannel_20_low_01 = new TH1D("byChannel_20_low_01", "byChannel_20_low_01", 32, 0, 32);
  TH1D * h_byChannel_20_low_12 = new TH1D("byChannel_20_low_12", "byChannel_20_low_12", 32, 0, 32);
  TH1D * h_byChannel_20_low_23 = new TH1D("byChannel_20_low_23", "byChannel_20_low_23", 32, 0, 32);
  TH1D * h_byChannel_20_peak_01 = new TH1D("byChannel_20_peak_01", "byChannel_20_peak_01", 32, 0, 32);
  TH1D * h_byChannel_20_peak_12 = new TH1D("byChannel_20_peak_12", "byChannel_20_peak_12", 32, 0, 32);
  TH1D * h_byChannel_20_peak_23 = new TH1D("byChannel_20_peak_23", "byChannel_20_peak_23", 32, 0, 32);
  TH1D * h_byChannel_20_high_01 = new TH1D("byChannel_20_high_01", "byChannel_20_high_01", 32, 0, 32);
  TH1D * h_byChannel_20_high_12 = new TH1D("byChannel_20_high_12", "byChannel_20_high_12", 32, 0, 32);
  TH1D * h_byChannel_20_high_23 = new TH1D("byChannel_20_high_23", "byChannel_20_high_23", 32, 0, 32);

  TH1D * h_byChannel_0_low_01 = new TH1D("byChannel_0_low_01", "byChannel_0_low_01", 32, 0, 32);
  TH1D * h_byChannel_0_low_12 = new TH1D("byChannel_0_low_12", "byChannel_0_low_12", 32, 0, 32);
  TH1D * h_byChannel_0_low_23 = new TH1D("byChannel_0_low_23", "byChannel_0_low_23", 32, 0, 32);
  TH1D * h_byChannel_0_peak_01 = new TH1D("byChannel_0_peak_01", "byChannel_0_peak_01", 32, 0, 32);
  TH1D * h_byChannel_0_peak_12 = new TH1D("byChannel_0_peak_12", "byChannel_0_peak_12", 32, 0, 32);
  TH1D * h_byChannel_0_peak_23 = new TH1D("byChannel_0_peak_23", "byChannel_0_peak_23", 32, 0, 32);
  TH1D * h_byChannel_0_high_01 = new TH1D("byChannel_0_high_01", "byChannel_0_high_01", 32, 0, 32);
  TH1D * h_byChannel_0_high_12 = new TH1D("byChannel_0_high_12", "byChannel_0_high_12", 32, 0, 32);
  TH1D * h_byChannel_0_high_23 = new TH1D("byChannel_0_high_23", "byChannel_0_high_23", 32, 0, 32);

  for(int i = 0; i < tree0_low->GetEntries(); i++) {
    tree0_low->GetEntry(i);
  
    if(boardID_0 != board) continue;

    for(int j = 0; j < tree20_low->GetEntries(); j++) {
      tree20_low->GetEntry(j);

      if(boardID_20 != boardID_0) continue;
      if(channel_20 != channel_0) continue;

      if(npeaks_20 > 0) h_byChannel_20_low_01->SetBinContent(channel_20 + 1, spacing01_20);
      if(npeaks_20 > 1) h_byChannel_20_low_12->SetBinContent(channel_20 + 1, spacing12_20);
      if(npeaks_20 > 2) h_byChannel_20_low_23->SetBinContent(channel_20 + 1, spacing23_20);

      if(npeaks_0 > 0) h_byChannel_0_low_01->SetBinContent(channel_0 + 1, spacing01_0);
      if(npeaks_0 > 1) h_byChannel_0_low_12->SetBinContent(channel_0 + 1, spacing12_0);
      if(npeaks_0 > 2) h_byChannel_0_low_23->SetBinContent(channel_0 + 1, spacing23_0);

      if(npeaks_0 <= 1) {
        h_notSeen_20_low_01->Fill(spacing01_20);
        h_notSeen_20_low_12->Fill(spacing12_20);
        h_notSeen_20_low_23->Fill(spacing23_20);
      }
      else {
        h_seen_20_low_01->Fill(spacing01_20);
        h_seen_20_low_12->Fill(spacing12_20);
        h_seen_20_low_23->Fill(spacing23_20);
      }
  
      break;
    }

  }

  for(int i = 0; i < tree0_peak->GetEntries(); i++) {
    tree0_peak->GetEntry(i);
  
    if(boardID_0 != board) continue;

    for(int j = 0; j < tree20_peak->GetEntries(); j++) {
      tree20_peak->GetEntry(j);

      if(boardID_20 != boardID_0) continue;
      if(channel_20 != channel_0) continue;

      if(npeaks_20 > 0) h_byChannel_20_peak_01->SetBinContent(channel_20 + 1, spacing01_20);
      if(npeaks_20 > 1) h_byChannel_20_peak_12->SetBinContent(channel_20 + 1, spacing12_20);
      if(npeaks_20 > 2) h_byChannel_20_peak_23->SetBinContent(channel_20 + 1, spacing23_20);

      if(npeaks_0 > 0) h_byChannel_0_peak_01->SetBinContent(channel_0 + 1, spacing01_0);
      if(npeaks_0 > 1) h_byChannel_0_peak_12->SetBinContent(channel_0 + 1, spacing12_0);
      if(npeaks_0 > 2) h_byChannel_0_peak_23->SetBinContent(channel_0 + 1, spacing23_0);

      if(npeaks_0 <= 1) {
        h_notSeen_20_peak_01->Fill(spacing01_20);
        h_notSeen_20_peak_12->Fill(spacing12_20);
        h_notSeen_20_peak_23->Fill(spacing23_20);
      }
      else {
        h_seen_20_peak_01->Fill(spacing01_20);
        h_seen_20_peak_12->Fill(spacing12_20);
        h_seen_20_peak_23->Fill(spacing23_20);
      }
  
      break;
    }

  }
  
  for(int i = 0; i < tree0_high->GetEntries(); i++) {
    tree0_high->GetEntry(i);
  
    if(boardID_0 != board) continue;

    for(int j = 0; j < tree20_high->GetEntries(); j++) {
      tree20_high->GetEntry(j);

      if(boardID_20 != boardID_0) continue;
      if(channel_20 != channel_0) continue;

      if(npeaks_20 > 0) h_byChannel_20_high_01->SetBinContent(channel_20 + 1, spacing01_20);
      if(npeaks_20 > 1) h_byChannel_20_high_12->SetBinContent(channel_20 + 1, spacing12_20);
      if(npeaks_20 > 2) h_byChannel_20_high_23->SetBinContent(channel_20 + 1, spacing23_20);

      if(npeaks_0 > 0) h_byChannel_0_high_01->SetBinContent(channel_0 + 1, spacing01_0);
      if(npeaks_0 > 1) h_byChannel_0_high_12->SetBinContent(channel_0 + 1, spacing12_0);
      if(npeaks_0 > 2) h_byChannel_0_high_23->SetBinContent(channel_0 + 1, spacing23_0);

      if(npeaks_0 <= 1) {
        h_notSeen_20_high_01->Fill(spacing01_20);
        h_notSeen_20_high_12->Fill(spacing12_20);
        h_notSeen_20_high_23->Fill(spacing23_20);
      }
      else {
        h_seen_20_high_01->Fill(spacing01_20);
        h_seen_20_high_12->Fill(spacing12_20);
        h_seen_20_high_23->Fill(spacing23_20);
      }
  
      break;
    }

  }

  out->Write();
  out->Close();

}
