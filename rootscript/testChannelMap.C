// Display geometrical mapping of shashlik modules and PADE channels
// Usage: root -l testChannelMap.C+
// Created 4/12/2014 B.Hirosky: Initial release
// 5/1/2014 BH: update to mapping in mm units

#include "TH2I.h"
#include "../include/Mapper.h"
#include "TStyle.h"
#include "TCanvas.h"
#include <iostream>
using namespace std;

void testChannelMap(){
  cout<<"========================================================"<<endl;
  cout<<"All views are looking towards the detector from UPSTREAM"<<endl;
  cout<<"========================================================"<<endl;

  gStyle->SetOptStat(0);
  Mapper *mapper=Mapper::Instance();

  // module numbers
  TH2I *hModU=new TH2I();
  mapper->GetModuleMap(hModU,-1);
  TH2I *hModD=new TH2I();
  mapper->GetModuleMap(hModD,1);
  // hardware channelIDs
  TH2I *hChanU=new TH2I();
  mapper->GetChannelMap(hChanU,-1);
  TH2I *hChanD=new TH2I();
  mapper->GetChannelMap(hChanD,1);
  // contiguous channel Indexs
  TH2I *hIdxU=new TH2I();
  mapper->GetChannelIdx(hIdxU,-1);
  TH2I *hIdxD=new TH2I();
  mapper->GetChannelIdx(hIdxD,1);

  TCanvas *c1=new TCanvas("maps","maps",1200,800);
  c1->Divide(3,2);

  c1->cd(1);
  hModD->Draw("text");
  c1->cd(4);
  hModU->Draw("text");

  c1->cd(2);
  hChanD->Draw("text");
  c1->cd(5);
  hChanU->Draw("text");

  c1->cd(3);
  hIdxD->Draw("text");
  c1->cd(6);
  hIdxU->Draw("text");

}



