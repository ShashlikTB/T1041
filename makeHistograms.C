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

  for(unsigned int ui = 0; ui < histNames.size(); ui++) peakHeights[ui] = new TH1D(histNames[ui], histNames[ui], 500, 0, 500);

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

  TFile * out = new TFile("hist_"+input+".root", "RECREATE");

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
  for(unsigned int ui = 0; ui < fNames.size(); ui++) files[i] = new TFile(fNames[ui], "READ");

  char char[100];
  int boardIDs[4] = {112, 113, 115, 116};

  vector<TString> histNames;
  for(int i = 0; i < 4; i++) {
    for(int j = 0; j < 32; j++) {
      sprintf(char, "peakHeight_board-%d_chan-%d", boardIDs[i], j);
      histNames.push_back(TString(char));
    }
  }

  vector<vector<TH1D*> > peakHeights;

  for(unsigned int ui = 0; ui < fNames.size(); ui++) {

    vector<TH1D*> v;
    v.resize(128);

    for(unsigned int uj = 0; uj < v.size(); uj++) v[ui] = (TH1D*)files[ui]->Get(histNames[uj]);
    v[ui]->SetLineColor(ui + 1);

    peakHeights.push_back(v);
    v.clear();

  }

  TLegend * leg = new TLegend(0.50, 0.65, 0.85, 0.85, NULL, "brNDC");
  for(unsigned int ui = 0; ui < fNames.size(); ui++) leg->AddEntry(peakHeights[ui][0], legendTitles[ui], "LP");
  leg->SetFillColor(0);
  leg->SetTextSize(0.028);

  TCanvas * canv = new TCanvas("canv", "Plot", 10, 10, 2000, 2000);
  canv->SetLogy(true);

  for(unsigned int ui = 0; ui < peakHeights[0].size(); ui++) {
    peakHeights[0][ui]->Draw();
    for(unsigned int uj = 1; uj < peakHeights.size(); uj++) peakHeights[uj][ui]->Draw("same");
    leg->Draw("same");
    canv->SaveAs(peakHeights[0][ui]->GetTitle() + ".png");
  }
      
  peakHeights.clear();
  files.clear();
  legendTitles.clear();
}

      
