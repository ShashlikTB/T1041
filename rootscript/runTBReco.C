#include <TString.h>
#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>

void runTBReco(TString rawFile, TString recFile=""){
  if (recFile=="") {
    recFile=rawFile;
    recFile.ReplaceAll(".root","_reco.root");
  }
  TFile *tfRaw=new TFile(rawFile);
  TTree *rawTree=(TTree*)tfRaw->Get("t1041");

  // turn on raw data branches in case we reco'd this file before, drop others
  rawTree->SetBranchStatus("*",0);
  rawTree->SetBranchStatus("tbevent",1);
  rawTree->SetBranchStatus("tbspill",1);

  TFile *tfRec=new TFile(recFile,"recreate");
  TTree *recTree=(TTree*)rawTree->Clone();
  
  Float_t pt;
  TBranch *calHits=recTree->Branch("pt",&pt,"pt/F");

  // finish
  tfRec->Write();
  tfRec->Print();
  
  delete tfRec;
  delete tfRaw;
}

