#include <TString.h>
#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <TSystem.h>
#include "TBEvent.h"
#include "CalReco.h"
#include "TrackReco.h"

TString runTBReco(TString rawFile, TString recFile="", TString outdir=""){
  if (recFile=="") {
    recFile=rawFile;
    recFile.ReplaceAll(".root","_reco.root");
    if (outdir != ""){
      recFile=gSystem->BaseName(recFile);
      recFile=outdir+"/"+recFile;
    }
  }
  TFile *tfRaw=new TFile(rawFile);
  TTree *rawTree=(TTree*)tfRaw->Get("t1041");

  // turn on raw data branches in case we reco'd this file before, drop others
  TBEvent *tbevent=new TBEvent();
  rawTree->SetBranchAddress("tbevent",&tbevent);
  rawTree->SetBranchStatus("*",0);
  rawTree->SetBranchStatus("tbevent",1);
  rawTree->SetBranchStatus("tbspill",1);

  TFile *tfRec=new TFile(recFile,"recreate");
  TTree *recTree=(TTree*)rawTree->CloneTree();

  TrackReco *trackreco=new TrackReco();
  trackreco->Process(rawTree,recTree);

  CalReco *calreco=new CalReco(2);   // 2 sigma cut for pulse fitting
  calreco->Process(rawTree,recTree);


  // finish
  recTree->Write();
  tfRec->Write();
  recTree->Print();

  
  delete tfRec;
  delete tfRaw;
  return recFile;
}

