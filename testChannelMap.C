#include "TH2I.h"
#include "shashlik.h"
#include "TStyle.h"
#include "TCanvas.h"
#include <iostream>
using namespace std;

void testChannelMap(){
  gStyle->SetOptStat(0);
  Mapper *mapper=Mapper::Instance();
  TH2I *hModF=new TH2I("hmap","Modules Front",4,0.5,4.5,4,0.5,4.5);
  TH2I *hModR=new TH2I("hmap","Modules Rear",4,0.5,4.5,4,0.5,4.5);
  TH2I *hChanF=new TH2I("hmap","Channels Front",8,0.5,4.5,8,0.5,4.5);
  TH2I *hChanR=new TH2I("hmap","Channels Rear",8,0.5,4.5,8,0.5,4.5);
  TCanvas *c1=new TCanvas("maps","maps",800,800);
  c1->Divide(2,2);

  for (int i=1; i<NMODULES+1; i++){
    int x,y;
    mapper->ModuleXY(i,x,y);
    hModF->Fill(x,y,i);
    hModR->Fill(x,y,-i);
  }
  c1->cd(1);
  hModF->Draw("text");
  c1->cd(2);
  hModR->Draw("text");

  for (int i=0; i<NPADECHANNELS/2; i++){
    // font
    int channelID=FIBERMAP[i*4];
    int fiberID=FIBERMAP[i*4+1];
    int module=fiberID/100;
    int fiber=fiberID-module*100;
    int ix,iy;
    mapper->ModuleXY(module,ix,iy);
    float x=ix, y=iy;
    //   cout << channelID << " " << fiberID << " " << module << " " << fiber << " "
    //	 << x << " " << y << endl;
    if (fiber==1) {x-=0.25; y+=0.25;}
    else if (fiber==2) {x+=0.25; y+=0.25;}
    else if (fiber==3) {x+=0.25; y-=0.25;}
    else if (fiber==4) {x-=0.25; y-=0.25;}
    hChanF->Fill(x,y,channelID);
    // rear
    channelID=FIBERMAP[i*4+2];
    fiberID=FIBERMAP[i*4+3];
    module=fiberID/100;
    fiber=fiberID-module*100;
    mapper->ModuleXY(module,ix,iy);
    if (fiber==1) {x-=0.25; y+=0.25;}
    else if (fiber==2) {x+=0.25; y+=0.25;}
    else if (fiber==3) {x+=0.25; y-=0.25;}
    else if (fiber==4) {x-=0.25; y-=0.25;}
    hChanR->Fill(x,y,channelID);
  }
  c1->cd(3);
  hChanF->Draw("text");
  c1->cd(4);
  hChanR->Draw("text");
}



