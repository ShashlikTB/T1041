#include <vector>

using namespace std;

void makeHistograms(TString input){

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

  for(unsigned int ui = 0; ui < histNames.size(); ui++) peakHeights[ui] = new TH1D(histNames[ui], histNames[ui], 400, 0, 800);

  // loop over events
  for (int i=0; i< BeamData->GetEntries(); i++) {

    if(i % 1000 == 0) cout << "Processing event " << i << " / " << (int)BeamData->GetEntries() << endl;

    BeamData->GetEntry(i);

    // loop over PADE channels
    for (int j=0; j<event->NPadeChan(); j++){

      UShort_t peak = 0;

      // loop over ADC samples
      UShort_t* wform=event->GetPadeChan(j).GetWform();
      for (int k=0; k<event->GetPadeChan(j).__SAMPLES(); k++){

	if(wform[k] > peak) peak = wform[k];

      }

      for(int k = 0; k < 4; k++) {
	if(boardIDs[k] == event->GetPadeChan(j).GetBoardID()) {
	  int pos = k*32 + j;
	  peakHeights[pos]->Fill(peak);
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
    peakHeights[0]->Draw();
    leg->AddEntry(peakHeights[0], legendTitles[0], "LP");
    for(unsigned int uFile = 1; uFile < fNames.size(); uFile++) {
      peakHeights[uFile]->SetLineWidth(3);
      peakHeights[uFile]->SetLineColor(uFile + 1);
      leg->AddEntry(peakHeights[uFile], legendTitles[uFile], "LP");
      peakHeights[uFile]->Draw("same");
    }
    leg->Draw("same");

    canv->SaveAs(TString(peakHeights[0]->GetTitle()) + ".gif");

    leg->Clear();
    peakHeights.clear();

  }
    
  files.clear();

}

      
