// ftp://root.cern.ch/root/doc/ROOTUsersGuideHTML/ch12s17.html
// Created 4/12/2014 B.Hirosky: Initial release

#include <TString.h>
#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <TString.h>
#include <iostream>
#include <vector>
#include <TH1I.h>
#include <TH1F.h>
#include "../include/TBReco.h"
#include "../include/WC.h"
#include "../include/TBEvent.h"

using std::cout;
using std::endl;
using std::vector;

void readerExample(TString file="latest.root"){

  TFile *f = new TFile(file);
  TTree *t1041 = (TTree*) f->Get("t1041");
 
  TString outname=file;
  outname.ReplaceAll(".root","_reco.root");
  TFile *tfo=new TFile(outname,"recreate");

  //// find definition of in-time hits ////
  WCReco wcReco;
  wcReco.AddTree(t1041);
  Int_t tMean[NTDC], tLow[NTDC], tHigh[NTDC];
  wcReco.GetTDCcuts(tMean, tLow, tHigh); 
  TH1I* hTDC[NTDC];
  wcReco.GetTDChists(hTDC);    // use to see the histograms
  /////////////////////////////////////////

  TH1F *hslopeX=new TH1F("hslopeX","Beam slope in X",50,-0.005,0.005);
  TH1F *hslopeY=new TH1F("hslopeY","Beam slope in Y",50,-0.005,0.005);

  // histograms of peak times
  TH1I *htime112=new TH1I("time112","Timing board 112",120,0,120);
  TH1I *htime113=new TH1I("time113","Timing board 113",120,0,120);
  TH1I *htime115=new TH1I("time115","Timing board 115",120,0,120);
  TH1I *htime116=new TH1I("time116","Timing board 116",120,0,120);

  // histograms for cluster x,y positions
  TH1F *hClusterX=new TH1F("hXCluster","Cluster X",56,-28,28);  // 1mm bins
  TH1F *hClusterY=new TH1F("hYCluster","Cluster Y",56,-28,28);


  // create a pointer to an event object for reading the branch values.
  TBEvent *event = new TBEvent(); 
  TBranch *bevent = t1041->GetBranch("tbevent");
  bevent->SetAddress(&event);

  // loop over events
  CalCluster calCluster;
  for (Int_t i=0; i< t1041->GetEntries(); i++) {
    t1041->GetEntry(i);

    // loop over PADE channels
    // cout << (dec) << "Spill number: " << event->GetSpillNumber()<<endl;
    for (Int_t j=0; j<event->NPadeChan(); j++){
      PadeChannel pc=event->GetPadeChan(j);
      int board=pc.GetBoardID();
      if (board==112) htime112->Fill(pc.GetPeak());
      if (board==113) htime113->Fill(pc.GetPeak());
      if (board==115) htime115->Fill(pc.GetPeak());
      if (board==116) htime116->Fill(pc.GetPeak());
    }

    vector<WCChannel> hitsX1, hitsY1, hitsX2, hitsY2;
    hitsX1=event->GetWChitsX(1,tLow,tHigh);   // fetch x,y hits in chambers 1 and 2
    hitsY1=event->GetWChitsY(1,tLow,tHigh);   // only selecting in-time hits
    hitsX2=event->GetWChitsX(2,tLow,tHigh);
    hitsY2=event->GetWChitsY(2,tLow,tHigh);
    bool haveTrack= (hitsX1.size()==1 && hitsY1.size()==1 && 
		     hitsX2.size()==1 && hitsY2.size()==1);   // require only 2 x,y hits

    // find slopes of tracks
    if (haveTrack){
      WCtrack track(hitsX1[0],hitsY1[0],hitsX2[0],hitsY2[0]);
      hslopeX->Fill(track.GetSlopeX());
      hslopeY->Fill(track.GetSlopeY());   
    }
    
    // plot x,y positions of "energy" (really ADC value) clusters
    // Warning clustering code is not vetted
    // calCluster.MakeCluster(event);  
    calCluster.MakeCluster(event,5);  // cut requiring 5 adc counts>pedistal
    hClusterX->Fill(calCluster.GetX());
    hClusterY->Fill(calCluster.GetY());

    // to do
    //       track extrapolation to face of calorimeter
    //       delta_x,y plots between cal cluster and track
  }

  tfo->Write();
  tfo->Close();
    
  cout << "Created: " << tfo->GetName() << endl;
}

