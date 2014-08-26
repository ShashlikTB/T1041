// Created 8/25/2014 B.Hirosky: Initial release
// Basic DQM plotter 

#include <TString.h>
#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TROOT.h>
#include <vector>
#include <TH1F.h>
#include <TH2F.h>
#include <TH1I.h>
#include <TH2I.h>
#include <TMath.h>
#include <TString.h>
#include <iostream>
#include "TBEvent.h"
#include "TBRecHit.h"
#include "TBTrack.h"
#include "calConstants.h"

using std::cout;
using std::endl;
using std::vector;
using TMath::Min;


TBEvent *event = new TBEvent();
vector<TBTrack> *tracks=0;
vector<TBRecHit> *rechits=0;

// plot X-Y hit distributions in WC 1 and 2
// and show TDC peak fits for corresponding 8 TDCs
void WC_DQM(TFile *f,TTree *tree){
  TCanvas *cWC=new TCanvas("cWC","WCDQM: TDC fits, hit distributions",1400,800);
  cWC->Divide(6,2);
  TString hname;
  for (int i=1; i<=8; i++) {
    cWC->cd(i);
    hname.Form("TDC%d",i);
    //   cout << hname<< endl;
    TH1F *h=(TH1F*)f->Get(hname);
    h->GetXaxis()->SetRange(0,200);
    h->Draw();
  }
  // all hit locations
  TH1I *hWC1x=new TH1I("hWC1x","WC1 x-Hit Locations",128,0,128);
  TH1I *hWC1y=new TH1I("hWC1y","WC1 y-Hit Locations",128,0,128);
  TH1I *hWC2x=new TH1I("hWC2x","WC2 x-Hit Locations",128,0,128);
  TH1I *hWC2y=new TH1I("hWC2y","WC2 y-Hit Locations",128,0,128);

  for (int i = 0; i < tree->GetEntries(); i++) {
    tree->GetEntry(i);
    for(int j = 0; j < event->GetWCHits(); j++){
      for(int j = 0; j < event->GetWCHits(); j++){
	WCChannel wc=event->GetWCChan(j);
	Int_t module = wc.GetTDCNum();
	Int_t wire = wc.GetWire();	
	if (module==1) hWC1x->Fill(wire);
	else if(module==2) hWC1x->Fill(wire+64);
	else if(module==3) hWC1y->Fill(wire);
	else if(module==4) hWC1y->Fill(wire+64);
	else if(module==5) hWC2x->Fill(wire);
	else if(module==6) hWC2x->Fill(wire+64);
	else if(module==7) hWC2y->Fill(wire);
	else if(module==8) hWC2y->Fill(wire+64);
      }
    }
  }
  cWC->cd(9);
  hWC1x->Draw();
  cWC->cd(10);
  hWC1y->Draw();
  cWC->cd(11);
  hWC2x->Draw();
  cWC->cd(12);
  hWC2y->Draw();
}

void track_DQM(TTree *tree){
  TCanvas *cTrk=new TCanvas("cTrk","TrackDQM: Slopes, Projections",900,300);
  cTrk->Divide(3,1);
  TH1F *hslopeX=new TH1F("hslopeX","Track x-slope;mRad",50,-10,10);
  TH1F *hslopeY=new TH1F("hslopeY","Track y-slope;mRad",50,-10,10);
  TH2I *hproj=new TH2I("hproj","Approximate projection to detector;X [mm];Y[mm]",128,-64,64,128,-64,64);


  for (int i = 0; i < tree->GetEntries(); i++) {
    tree->GetEntry(i);
    
    if (tracks->size()==0) continue;

    TBTrack &track=tracks->at(0);

    hslopeX->Fill(track.GetSlopeX()*1000);
    hslopeY->Fill(track.GetSlopeY()*1000);
    float x,y;
    track.Project(zShash,x,y);
    hproj->Fill(x,y);
  }
  cTrk->cd(1);
  hslopeX->Draw();
  cTrk->cd(2);
  hslopeY->Draw();
  cTrk->cd(3);
  hproj->Draw();
}

void calo_DQM(TTree *tree){
  TCanvas *cCal=new TCanvas("cCal","CaloDQM: Slopes, Projections",900,900);
  cCal->Divide(2,2);
  TH2F *hNoise=new TH2F("hNoise","Noise vs Channel;channel index;ADC counts",128,0,128,20,0,2);
  TH2F *hAmp=new TH2F("hAmp","ln(Amplitude) vs Channel;channel index;ln(ADC counts)",128,0,128,40,0,8);
  TH2F *hChi2=new TH2F("hChi2","ln(Chi2) vs Channel;channel number;ln(Chi2)",128,0,128,40,0,9);
  TH2F *hNdof=new TH2F("hNdof","Ndof vs Channel;channel number;NDOF fit",128,0,128,40,0,40);
  
  hAmp->SetStats(0);
  hNoise->SetStats(0);
  hChi2->SetStats(0);
  hNdof->SetStats(0);

  for (Int_t i=0; i<tree->GetEntries(); i++) {
    tree->GetEntry(i);
    for (unsigned c=0;c<rechits->size(); c++){
      TBRecHit &hit=rechits->at(c);
      double amax=hit.AMax();
      double chi2=hit.Chi2();
      if (amax>0) hAmp->Fill( hit.ChannelIndex(), TMath::Log(amax) );
      if (chi2>0) hChi2->Fill( hit.ChannelIndex(), TMath::Log(chi2) );
      hNoise->Fill( hit.ChannelIndex(), hit.NoiseRMS() );
      hNdof->Fill( hit.ChannelIndex(), hit.Ndof());
    }
  }
  cCal->cd(1);
  hAmp->Draw("Colz");
  cCal->cd(2);
  hNoise->Draw("Colz");
  cCal->cd(3);
  hChi2->Draw("Colz");
  cCal->cd(4);
  hNdof->Draw("Colz");
}


void DQMplots(TString file="latest_reco.root"){
  gROOT->SetStyle("Plain");

  TFile *f = new TFile(file);
  TTree *t1041 = (TTree*) f->Get("t1041");
  t1041->SetBranchAddress("tbevent",&event);
  t1041->SetBranchAddress("tbtracks",&tracks);
  t1041->SetBranchAddress("tbrechits",&rechits);  
 
  cout << "Analyzing: " << file << endl;
  
  WC_DQM(f,t1041);
  track_DQM(t1041);
  calo_DQM(t1041);
  return;
  
}

