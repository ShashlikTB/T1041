// simple tool to dump events from a TB tree

copyTBTree(TString in_root, TString out_root, int nevents=100, int start=0){
  if (!TClassTable::GetDict("TBEvent")) {
    gSystem->Load("$TBHOME/build/lib/libTB.so");  // n.b. make sure to compile if changed
  }
  TFile *oldfile = new TFile(in_root);
  TTree *oldtree = (TTree*)oldfile->Get("t1041");
  int nentries = oldtree->GetEntries();
  TBEvent *event=0;
  TBSpill *spill=0;
  oldtree->SetBranchAddress("tbevent",&event);
  oldtree->SetBranchAddress("tbspill",&spill);


  TFile *newfile=new TFile(out_root,"recreate");
  TTree *newtree = oldtree->CloneTree(0);

  int end=start+nevents;
  if (end>nentries) end=nentries;
  cout << "Copying " << nevents << " events" << endl;
  for (int i=start;i<end; i++) {
    oldtree->GetEntry(i);
    newtree->Fill();
  }

  newtree->Print();
  newfile->Write();
  delete oldfile;
  delete newfile;
}
