#include "CalReco.h"
#include "TBRecHit.h"
#include "TBEvent.h"
#include <vector>
#include <iostream>

using std::vector;
using std::cout;
using std::endl;


int CalReco::Process(TTree *recTree){
  // Add the TBRecHit branch
  vector<TBRecHit> *rechits = new vector<TBRecHit>;
  cout << "Adding branch: tbrechits"<< endl;
  TBranch *brp=recTree->Branch("tbrechits","std::vector<TBRecHit>",&rechits);

  TBRecHit hit;
  TBEvent *event = new TBEvent();
  recTree->SetBranchAddress("tbevent",&event);

  // loop over the raw data tree
  int nEvents=recTree->GetEntries();
  for (int i=0; i<nEvents; i++){
    if ( i % TMath::Max(1,(nEvents/50)) == 0) 
      cout << "CalReco: Processing event " << i << " / " << nEvents << endl;
    rechits->clear();  
    recTree->GetEntry(i);

    for (Int_t nch=0; nch<event->NPadeChan(); nch++){
      PadeChannel pc=event->GetPadeChan(nch);
      hit.Init(&pc, _nSigmaCut);
      if ( (hit.Status() & TBRecHit::kZSP) ==0 ) 
	rechits->push_back(hit);  // only save hits 
    }
    brp->Fill();
  }
  delete rechits;
  return 0;
}

