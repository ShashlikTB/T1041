// Created 4/19/2014 B.Hirosky: Initial release


#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <TH1F.h>
#include "../include/TBEvent.h"


void pulseHeights(Int_t board=112, TString file="latest.root", 
		  Int_t xmin=300, Int_t xmax=50){

  TFile *f = new TFile(file);
  // create a pointer to an event object for reading the branch values.
  TBEvent *event = new TBEvent();
  TTree *t1041 = (TTree*) f->Get("t1041");
  TBranch *bevent = t1041->GetBranch("tbevent");
  bevent->SetAddress(&event);
  
  TH1F *hpulse[32];
  for (int i=0; i<32; i++){
    TString name;
    name.Form("h%d_%2d",board,i);
    hpulse[i]=new TH1F(name,name,xmax-xmin,xmin,xmax);
  }


  // loop over events
  for (Int_t i=0; i< t1041->GetEntries(); i++) {
    t1041->GetEntry(i);
    // loop over PADE channels
    for (Int_t j=0; j<event->NPadeChan(); j++){
      if (event->GetPadeChan(j).GetBoardID() != board) continue;
      int chan=event->GetPadeChan(j).GetChannelID();
      hpulse[chan]->Fill(event->GetPadeChan(j).GetMax());
    }
  }
  
}


