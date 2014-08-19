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
  rawTree->ResetBranchAddresses();
  rawTree->SetBranchAddress("tbevent",&event);

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
    // Summer 2013: 
    //    Replace cut/dead channels(29,51,60) w/ copy of opposing channel 
    for (Int_t nch=0; nch<event->NPadeChan(); nch++){
      PadeChannel pc=event->GetPadeChan(nch);
      int idx=pc.GetChannelIndex();

      if ( event->GetRunPeriod()>=TBEvent::TBRun2a
	   && (idx==29 || idx==51 || idx==60 ) ) continue; // dead channels
      
      hit.Init(&pc, _nSigmaCut);
      if ( (hit.Status() & TBRecHit::kZSP) == 0 ) {
	rechits->push_back(hit);  // only save hits over ZSP
      }
      if ( event->GetRunPeriod()>=TBEvent::TBRun2a
	   && (idx==(29+64) || idx==(51+64) || idx==(60+64) ) ){
	TBRecHit mirror(hit,idx-64,TBRecHit::kMirrored);
	rechits->push_back(mirror);
      }
    }
    brp->Fill();
    if (i==0) {
      cout << " *********** " << endl;
      for (unsigned n=0; n<rechits->size(); n++) cout << rechits->at(n) << endl;
    }
  }
  delete rechits;
  return 0;
}

