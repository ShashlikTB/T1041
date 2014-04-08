// ftp://root.cern.ch/root/doc/ROOTUsersGuideHTML/ch12s17.html

void readerExample(){
  if (!TClassTable::GetDict("TBEvent")) {
    gSystem->Load("TBEvent_cc.so");  // n.b. make sure to compile if changed
  }

  TFile *f = new TFile("outputNtuple.root");
  // create a pointer to an event object for reading the branch values.
  TBEvent *event = new TBEvent();

  TBranch *bevent = BeamData->GetBranch("event");
  bevent->SetAddress(&event);

  // loop over events
  for (Int_t i=0; i< BeamData->GetEntries(); i++) {
    BeamData->GetEntry(i);
    // loop over PADE channels
    cout << (dec) << "Spill number: " << event->GetSpillNumber()<<endl;
    for (Int_t j=0; j<event->NPadeChan(); j++){
      cout << "board : " << (dec) << event->GetPadeChan(j).GetBoardID() << endl;
      // loop over ADC samples
      UShort_t* wform=event->GetPadeChan(j).GetWform();
      for (Int_t k=0; k<event->GetPadeChan(j).__SAMPLES(); k++){
	cout << (hex) << wform[k];
      }
      cout << endl;
    }
  }
}

