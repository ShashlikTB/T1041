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
  for (int i=0; i<recTree->GetEntries(); i++){
    rechits->clear();  
    recTree->GetEntry(i);
    for (Int_t nch=0; nch<event->NPadeChan(); nch++){
      hit.Clear();
      PadeChannel pc=event->GetPadeChan(nch);
      hit.SetChannelIndex(pc.GetChannelIndex());
      rechits->push_back(hit);
    }
    brp->Fill();
  }
  delete rechits;
  return 0;
}

