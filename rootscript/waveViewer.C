#include <iostream>
#include "TString.h"
#include "TFile.h"
#include "TBEvent.h"
#include "PadeChannel.h"
#include "Mapper.h"
#include "TTree.h"
#include "TBranch.h"
#include "TF1.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TSystem.h"

const int MAXADC=4095;

// inputs data file and event in file to display (default is to integrate all)
void waveViewer(TString fdat, Int_t board=-1, Int_t channel=-1){
  TFile *f = new TFile(fdat);
  if (f->IsZombie()){
    cout << "Cannot open file: " << fdat << endl;
    return;
  }

  gStyle->SetOptStat(0);

  TBEvent *event = new TBEvent();
  TTree *t1041 = (TTree*)f->Get("t1041"); 
  TBranch *bevent = t1041->GetBranch("tbevent");
  bevent->SetAddress(&event);

  TCanvas *c=new TCanvas();
  c->cd();
  PulseFit fit;
  TH1F *hw=new TH1F("hw","waveform",120,0,120);
  for (Int_t i=0; i<t1041->GetEntriesFast(); i++) {
    t1041->GetEntry(i);
    for (Int_t j=0; j<event->NPadeChan(); j++){
      PadeChannel pch=event->GetPadeChan(j);
      if (board>0 && (int)pch.GetBoardID()!=board) continue;
      if (channel>0 && (int)pch.GetChannelID()!=channel) continue;
      // pch.GetHist(hw);
      // hw->Draw();
      fit=PadeChannel::FitPulse(&pch);
      pch.GetHist(hw);
      hw->Draw();
      fit.func.Draw("same");
      c->Update();
      gSystem->Sleep(1000);
    }
  }
}



