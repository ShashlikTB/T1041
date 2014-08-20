#include "CalReco.h"
#include "TBRecHit.h"
#include "TBEvent.h"
#include <vector>
#include <iostream>

using std::vector;
using std::cout;
using std::endl;


int CalReco::Process(TTree *rawTree, TTree *recTree){
  TBEvent *event = new TBEvent();
  TBSpill *tbspill=new TBSpill();
  rawTree->ResetBranchAddresses();
  rawTree->SetBranchAddress("tbevent",&event);
  rawTree->SetBranchAddress("tbspill",&tbspill);

  // Add the TBRecHit branch
  vector<TBRecHit> *rechits = new vector<TBRecHit>;
  cout << "Adding branch: tbrechits"<< endl;
  TBranch *brp=recTree->Branch("tbrechits","std::vector<TBRecHit>",&rechits);


  TBRecHit hit;

  // loop over the raw data tree
  int nEvents=rawTree->GetEntries();
  for (int i=0; i<nEvents; i++){
    if ( i % TMath::Max(1,(nEvents/25)) == 0) 
      cout << "CalReco: Processing event " << i << " / " << nEvents << endl;
    rawTree->GetEntry(i);
    rechits->clear();  

    // Special cases  
    // April 2014
    //    Replace dead channel idx= ???
    // Summer 2013: 
    //    Replace cut/dead channels(idx=29,51,60) w/ copy of opposing channel 
    bool tbrun1 = event->GetRunPeriod()==TBEvent::TBRun1;
    bool tbrun2 = !tbrun1 && event->GetRunPeriod()<=TBEvent::TBRun2c;
    for (Int_t nch=0; nch<event->NPadeChan(); nch++){

      PadeChannel pc=event->GetPadeChan(nch);
      int idx=pc.GetChannelIndex();

      if ( tbrun2
	   && (idx==29 || idx==51 || idx==60 ) ) continue; // dead channels
      if ( tbrun1 && idx==123 ) continue; // dead channel
      
      hit.Init(&pc, _nSigmaCut);
      if ( (hit.Status() & TBRecHit::kZSP) == 0 ) {
	rechits->push_back(hit);  // only save hits over ZSP
      }
      else continue;  // no hit to add

      if ( tbrun2
	   && (idx==(29+64) || idx==(51+64) || idx==(60+64) ) ){
	TBRecHit mirror(hit,idx-64,TBRecHit::kMirrored);
	rechits->push_back(mirror);
      }
      if ( tbrun1 && idx==123-64 ){
	TBRecHit mirror(hit,idx+64,TBRecHit::kMirrored);
	rechits->push_back(mirror);
      }

    }
    brp->Fill();

  }
  delete rechits;
  return 0;
}

