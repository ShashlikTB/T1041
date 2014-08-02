void makeTestFiles(){
TString in,out;
  gROOT->ProcessLine(".L rootscript/copyTBTree.C");
  in="/raids/nas0/testbeam2/reco/rec_capture_20140802_160330_reco.root";
  out="data/20events_16Gev_electrons_20140802_160330_reco.root";
  copyTBTree(in,out,20);
}


