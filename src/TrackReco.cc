#include "TrackReco.h"
#include "TBEvent.h"
#include "TBTrack.h"
#include <algorithm>
#include <vector>
#include <iostream>

using std::vector;
using std::cout;
using std::endl;


int TrackReco::Process(TTree *rawTree, TTree *recTree){

  WCReco wcreco;
  wcreco.AddTree(rawTree);
  wcreco.GetTDCcuts(mean_, tLow_, tHigh_);   // do this first b/c it sets branch address!

  TBEvent *event = new TBEvent();
  rawTree->ResetBranchAddresses();
  rawTree->SetBranchAddress("tbevent",&event);

  // Add the TBTracks branch
  vector<TBTrack> *tracks = new vector<TBTrack>;
  cout << "Adding branch: tbtracks"<< endl;
  TBranch *brp=recTree->Branch("tbtracks","std::vector<TBTrack>",&tracks);

  // loop over the raw data tree
  int nEvents=rawTree->GetEntries();
  for (int i=0; i<nEvents; i++){
    if ( i % TMath::Max(1,(nEvents/25)) == 0) 
      cout << "TrackReco: Processing event " << i << " / " << nEvents << endl;
    rawTree->GetEntry(i);
    tracks->clear();

    hitsX1=event->GetWChitsX(1,tLow_,tHigh_);   // fetch x,y hits in chambers 1 and 2
    hitsY1=event->GetWChitsY(1,tLow_,tHigh_);   // only selecting in-time hits
    hitsX2=event->GetWChitsX(2,tLow_,tHigh_);
    hitsY2=event->GetWChitsY(2,tLow_,tHigh_);

    for(unsigned h1=0; h1<hitsX1.size(); ++h1){ // loop over X1 
      for(unsigned h2=0; h2<hitsY1.size(); ++h2){ // loop over Y1
	for(unsigned h3=0; h3<hitsX2.size(); ++h3){ // loop over X2 
	  for(unsigned h4=0; h4<hitsY2.size(); ++h4){ // loop over Y2
	    TBTrack track(hitsX1[h1], hitsY1[h2], hitsX2[h3], hitsY2[h4]);
	    float trackX1, trackY1;
	    // !!! Approximate scintillator confirmation
	    // Because we have no survey, the cuts on > 50 mm below
	    // ASSUME WC 1,2 and Scint 1,2 are centered
	    // Also assumed is that the scints are in fact 10 cm square!
	    // This needs to be replaced with an effective alignment derived 
            // from in situ data
	    track.Project(zSC1, trackX1, trackY1);
	    if(fabs(trackX1)<50 && fabs(trackY1)<50) track.AddStatus(TBTrack::kSC1);
	    float trackX2, trackY2;
	    track.Project(zSC2, trackX2, trackY2);
	    if(fabs(trackX2)<50 && fabs(trackY2)<50) track.AddStatus(TBTrack::kSC2);
	    tracks->push_back(track);
	  }
	}
      }
    }
    // order tracks by increasing 2D slope
    std::sort(tracks->begin(), tracks->end());
    brp->Fill();
  }
	 
  delete tracks;
  return 0;
}

