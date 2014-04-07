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

  for(int i = 0; i < 4; i++) {
    for(int j = 0; j < 32; j++) {
      sprintf(char, "peakHeight_board-%d_chan-%d", boardIDs[i], j);
      histNames.push_back(TString(char));
    }
  }

  for(unsigned int ui = 0; ui < histNames.size(); ui++) peakHeights[ui] = new TH1D(histNames[ui], histNames[ui]+";Peak Height in ADC Counts;Events", 800, 0, 800);

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
      for (int k = firstLow; k <= firstHigh && k < event->GetPadeChan(j).__SAMPLES(); k++){
	if(wform[k] > firstPeak) firstPeak = wform[k];
      }

      for (int k = secondLow; k <= secondHigh && k < event->GetPadeChan(j).__SAMPLES(); k++){
	if(wform[k] > secondPeak) secondPeak = wform[k];
      }

      for(int k = 0; k < 4; k++) {
	if(boardIDs[k] == event->GetPadeChan(j).GetBoardID()) {
	  int pos = k*32 + j;
	  peakHeights[pos]->Fill(firstPeak);
	  if(twoPeaksPerTrigger) peakHeights[pos]->Fill(secondPeak);
	  break;
	}
      }

    } // pade channels

  } // events

  TFile * out = new TFile("hist_"+input, "RECREATE");

  for(unsigned int ui = 0; ui < peakHeights.size(); ui++) peakHeights[ui]->Write();

  out->Write();
  out->Close();

}

void overlayPlots(vector<TString> fNames, vector<TString> legendTitles) {

  if(fNames.size() != legendTitles.size()) {
    cout << endl << "Give me the same number of files as titles!" << endl << endl;
    return;
  }

  if(fNames.size() == 0) {
    cout << endl << "No files, nothing to do!" << endl << endl;
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

  TLegend * leg = new TLegend(0.50, 0.65, 0.85, 0.85, NULL, "brNDC");
  leg->SetFillColor(0);
  leg->SetTextSize(0.028);

  TCanvas * canv = new TCanvas("canv", "Plot", 10, 10, 2000, 2000);
  canv->SetLogy(false);

  vector<TH1D*> peakHeights;

  for(unsigned int uChannel = 0; uChannel < 128; uChannel++) {

    for(unsigned int uFile = 0; uFile < fNames.size(); uFile++) peakHeights.push_back((TH1D*)files[uFile]->Get(histNames[uChannel]));
    
    peakHeights[0]->SetLineWidth(3);
    peakHeights[0]->Scale(1./peakHeights[0]->Integral());
    peakHeights[0]->Draw();
    leg->AddEntry(peakHeights[0], legendTitles[0]+Form(" (%.0f events)", peakHeights[0]->GetEntries()), "LP");
    for(unsigned int uFile = 1; uFile < fNames.size(); uFile++) {
      peakHeights[uFile]->SetLineWidth(3);
      peakHeights[uFile]->SetLineColor(uFile + 1);
      peakHeights[uFile]->Scale(1./peakHeights[uFile]->Integral());
      leg->AddEntry(peakHeights[uFile], legendTitles[uFile]+Form(" (%.0f events)", peakHeights[uFile]->GetEntries()), "LP");
      peakHeights[uFile]->Draw("same");
    }
    leg->Draw("same");

    canv->SaveAs(TString(peakHeights[0]->GetTitle()) + ".gif");

    leg->Clear();
    peakHeights.clear();

  }
    
  files.clear();

}

      
