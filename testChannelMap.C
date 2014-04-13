// Display geometrical mapping of shashlik modules and PADE channels
// Usage: root -l testChannelMap.C+
// Created 4/12/2014 B.Hirosky: Initial release


#include "TH2I.h"
#include "shashlik.h"
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
  TH2I *hModU=new TH2I("hModU","Modules UpSteam",4,0.5,4.5,4,0.5,4.5);
  TH2I *hModD=new TH2I("hModD","Modules DownStream",4,0.5,4.5,4,0.5,4.5);
  TH2I *hChanU=new TH2I("hChanU","Channels UpStream",8,0.5,4.5,8,0.5,4.5);
  TH2I *hChanD=new TH2I("hChanD","Channels DownStream",8,0.5,4.5,8,0.5,4.5);
  TCanvas *c1=new TCanvas("maps","maps",800,800);
  c1->Divide(2,2);

  for (int i=1; i<=NMODULES; i++){
    int x,y;
    mapper->ModuleXY(i,x,y);
    hModU->Fill(x,y,-i);
    hModD->Fill(x,y,i);
  }
  c1->cd(1);
  hModD->Draw("text");
  c1->cd(2);
  hModU->Draw("text");

  for (int i=0; i<NPADECHANNELS/2; i++){
    // downstream channels
    int channelID=FIBERMAP[i*4];
    int fiberID=FIBERMAP[i*4+1];
    float x,y;
    //    cout << i<<" "<< channelID << " " << fiberID << "  ";
    mapper->FiberXY(fiberID, x, y);
    //    cout << x << " " << y << endl;
    hChanD->Fill(x,y,channelID);
    // upstream channels
    channelID=FIBERMAP[i*4+2];
    fiberID=FIBERMAP[i*4+3];

    hChanU->Fill(x,y,channelID);
  }
 

  c1->cd(3);
  hChanD->Draw("text");
  c1->cd(4);
  hChanU->Draw("text");
}



