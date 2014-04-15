#include <iostream>

#include "TString.h"
#include "TFile.h"
#include "TBEvent.h"
#include "shashlik.h"
#include "TTree.h"
#include "TBranch.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TStyle.h"

const int MAXADC=4095;

TH2F * moduleHistogram(bool isUpstream, TString title, UShort_t zmin, UShort_t zmax) {

  TString hName = isUpstream ? "hModU_"+title : "hModD_"+title;
  TString hTitle = isUpstream ? "Modules Upstream "+title : "Modules Downstream "+title;

  TH2F * h = new TH2F(hName, hTitle, 4, 0.5, 4.5, 4, 0.5, 4.5);

  return h;
}

TH2F * channelHistogram(bool isUpstream, TString title, UShort_t zmin, UShort_t zmax) {

  TString hName = isUpstream ? "hChanU_"+title : "hChanD_"+title;
  TString hTitle = isUpstream ? "Channels Upstream "+title : "Channels Downstream "+title;

  TH2F * h = new TH2F(hName, hTitle, 8, 0.5, 4.5, 8, 0.5, 4.5);

  if(zmin > 0) h->SetMinimum(zmin);
  if(zmax > 0) h->SetMaximum(zmax);

  return h;
}

void drawChannelMap(TCanvas*& can) {

  Mapper * mapper=Mapper::Instance();

  TH2I *hModU=new TH2I(TString(can->GetName()) + "_label_a","Modules UpSteam", 4, 0.5, 4.5, 4, 0.5, 4.5);
  TH2I *hModD=new TH2I(TString(can->GetName()) + "_label_b","Modules DownStream", 4, 0.5, 4.5, 4, 0.5, 4.5);
  TH2I *hChanU=new TH2I(TString(can->GetName()) + "_label_c","Channels UpStream", 8, 0.5, 4.5, 8, 0.5, 4.5);
  TH2I *hChanD=new TH2I(TString(can->GetName()) + "_label_d","Channels DownStream", 8, 0.5, 4.5, 8, 0.5, 4.5);

  for (int i=1; i<=NMODULES; i++){
    int x,y;
    mapper->ModuleXY(i,x,y);
    hModU->Fill(x,y,-i);
    hModD->Fill(x,y,i);
  }
  can->cd(1);
  hModD->SetMarkerSize(3);
  hModD->Draw("text same");
  can->cd(2);
  hModU->SetMarkerSize(3);
  hModU->Draw("text same");

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
 
  can->cd(3);
  hChanD->SetMarkerSize(1.5);
  hChanD->Draw("text same");
  can->cd(4);
  hChanU->SetMarkerSize(1.5);
  hChanU->Draw("text same");
}

void drawCalorimeterPlot(TString name, TH2F * hModU, TH2F * hModD, TH2F * hChanU, TH2F * hChanD) {

  TCanvas * canv =new TCanvas("canv_"+name, name, 800, 800);
  canv->Divide(2,2);

  canv->cd(1);
  hModD->Draw("colz");

  canv->cd(2);
  hModU->Draw("colz");

  canv->cd(3)->SetGrid();
  hChanD->Draw("colz");

  canv->cd(4)->SetGrid();
  hChanU->Draw("colz");

  drawChannelMap(canv);

  canv->SaveAs(name+".gif");

  delete canv;
}
