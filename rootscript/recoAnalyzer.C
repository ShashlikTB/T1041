// Created 7/19/2014 B.Hirosky: Initial release
// Example of access to RECO branch information

#include <TString.h>
#include <TFile.h>
#include <TTree.h>

#include <vector>
#include <TH1I.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TMath.h>
#include <TString.h>
#include <iostream>
#include <TBEvent.h>
#include <TBRecHit.h>


using std::cout;
using std::endl;
using std::vector;
using TMath::Min;


void recoAnalyzer(TString file="latest_reco.root"){

  TFile *f = new TFile(file);
  TTree *t1041 = (TTree*) f->Get("t1041");
 
  cout << "Analyzing: " << file << endl;
  
  TBEvent *tbevent=new TBEvent();
  TBSpill *tbspill=new TBSpill();
  vector<TBRecHit> *rechits=0;  // important to set this = 0!
  
  t1041->SetBranchAddress("tbevent",&tbevent);
  t1041->SetBranchAddress("tbspill",&tbspill);
  t1041->SetBranchAddress("tbrechits",&rechits);
  
  for (Int_t i=0; i<t1041->GetEntries(); i++) {
    t1041->GetEntry(i);
    for (unsigned c=0;c<rechits->size(); c++){
      cout << (*rechits)[c]<<endl;
    }
  }
}

