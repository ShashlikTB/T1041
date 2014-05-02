// Display geometrical mapping of shashlik modules and PADE channels
// Usage: root -l testChannelMap.C+
// Created 4/12/2014 B.Hirosky: Initial release
// 5/1/2014 BH: update to mapping in mm units

#include "TH2I.h"
#include "../include/shashlik.h"
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

  TH2I *hModU=new TH2I();
  mapper->GetModuleMap(hModU,-1);
  TH2I *hModD=new TH2I();
  mapper->GetModuleMap(hModD,1);
  TH2I *hChanU=new TH2I();
  mapper->GetChannelMap(hChanU,-1);
  TH2I *hChanD=new TH2I();
  mapper->GetChannelMap(hChanD,1);

  TCanvas *c1=new TCanvas("maps","maps",800,800);
  c1->Divide(2,2);

  c1->cd(1);
  hModD->Draw("text");
  c1->cd(2);
  hModU->Draw("text");

  c1->cd(3);
  hChanD->Draw("text");
  c1->cd(4);
  hChanU->Draw("text");
}



