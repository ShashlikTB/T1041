// Created 7/19/2014 B.Hirosky: Initial release
// Example of access to RECO branch information

#include <TString.h>
#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>

#include <vector>
#include <TH1F.h>
#include <TH2F.h>
#include <TMath.h>
#include <TString.h>
#include <iostream>
#include "TBEvent.h"
#include "TBRecHit.h"
#include "TBTrack.h"

using std::cout;
using std::endl;
using std::vector;
using TMath::Min;
// see runTBReco.C for more details

void recoAnalyzer(TString file="latest_reco.root"){

  TFile *f = new TFile(file);
  TTree *t1041 = (TTree*) f->Get("t1041");
 
  cout << "Analyzing: " << file << endl;
  
  TBEvent *tbevent=new TBEvent();
  TBSpill *tbspill=new TBSpill();
  vector<TBRecHit> *rechits=0;  // important to set this = 0!
  vector<TBTrack> *tracks=0;
  
  t1041->SetBranchAddress("tbevent",&tbevent);
  t1041->SetBranchAddress("tbspill",&tbspill);
  t1041->SetBranchAddress("tbrechits",&rechits);
  t1041->SetBranchAddress("tbtracks",&tracks);

  // find mean pulse amplitude
  double meanA=0;
  for (Int_t i=0; i<t1041->GetEntries(); i++) {
    t1041->GetEntry(i);
    for (unsigned c=0;c<rechits->size(); c++){
      if (i==0) cout << c << " , " << rechits->at(c) << endl;
      meanA+=rechits->at(c).AMax();
    }
  }
  meanA/=rechits->size()/t1041->GetEntries();
  
  TH1F *hAmp=new TH1F("hAmp","Pulse Fit Amplitudes",50,meanA/2,meanA*2);
  TH1F *hRise1=new TH1F("hRise1","tRise Board 112",50,25,75);
  TH1F *hRise2=new TH1F("hRise2","tRise Board 115",50,25,75);
  TH1F *hRise3=new TH1F("hRise3","tRise Board 116",50,25,75);
  TH1F *hRise4=new TH1F("hRise4","tRise Board 117",50,25,75);
  TH2F *hNoise1=new TH2F("hNoise1","Noise vs Channel Board 112;channel number;ADC counts",32,0,32,20,0,2);
  TH2F *hChi21=new TH2F("hChi21","Chi^2 vs Channel Board 112;channel number;Chi^2",32,0,32,20,0,100);
  TH2F *hAmax2D=new TH2F("hAmax","Amplitude vs Channel;channel number;ADC counts",128,0,128,40,0,2048);
  TH1F *hslopeX=new TH1F("hslopeX","Track x-slope",50,-0.01,0.01);
  TH1F *hslopeY=new TH1F("hslopeY","Track y-slope",50,-0.01,0.01);

  for (Int_t i=0; i<t1041->GetEntries(); i++) {
    t1041->GetEntry(i);
    for (unsigned c=0;c<rechits->size(); c++){
      TBRecHit &hit=rechits->at(c);
      hAmp->Fill( hit.AMax() );
      hAmax2D->Fill ( hit.ChannelIndex(), hit.AMax() );
      int boardID=hit.GetBoardID();
      
      if (boardID==112) {
	hRise1->Fill(hit.TRise());
	hNoise1->Fill(hit.GetChannelNumber(),hit.NoiseRMS());
	hChi21->Fill(hit.GetChannelNumber(),hit.Chi2());
      }
      else if (boardID==115) hRise2->Fill(hit.TRise());
      else if (boardID==116) hRise3->Fill(hit.TRise());
      else if (boardID==117) hRise4->Fill(hit.TRise());
    }
    if (tracks->size()>0) { 
      TBTrack &track=(*tracks)[0];
      hslopeX->Fill(track.GetSlopeX());
      hslopeY->Fill(track.GetSlopeY());
    }
  }
  // using the last event, print out some run summary info
  cout << "===== Run Summary Info =====" << endl;
  cout << "Beam Type: " << tbspill->GetPID() << "   "
       << "Gain: " << (tbspill->GetPadeHeader(0))->Gain() << endl;
  cout << "Table X Y: " << tbspill->GetTableX() << " " << tbspill->GetTableY() << endl;
  cout << "============================" << endl;
  

  TCanvas *c1=new TCanvas("c1","recoAnayzer",1600,800);
  c1->Divide(5,2);
  c1->cd(1); hAmp->Draw();
  c1->cd(2); hRise1->Draw();
  c1->cd(3); hRise2->Draw();
  c1->cd(4); hRise3->Draw();
  c1->cd(5); hRise4->Draw();
  c1->cd(6); hNoise1->Draw("COLZ");
  c1->cd(7); hChi21->Draw("COLZ");
  c1->cd(8); hAmax2D->Draw("COLZ");
  c1->cd(9); hslopeX->Draw();
  c1->cd(10); hslopeY->Draw();
}

