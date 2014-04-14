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
const UShort_t threshold = 200;

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

// inputs data file and event in file to display (default is to integrate all)
void calDisplay(TString fdat, int ndisplay=0){

  gStyle->SetOptStat(0);

  TFile *f = new TFile(fdat);
  if (f->IsZombie()){
    cout << "Cannot open file: " << fdat << endl;
    return;
  }

  TBEvent *event = new TBEvent();
  TTree *t1041 = (TTree*)f->Get("t1041"); 
  TBranch *bevent = t1041->GetBranch("tbevent");
  bevent->SetAddress(&event);

  Bool_t singleEvent = ndisplay > 0;

  Mapper * mapper = Mapper::Instance();

  TH2F * hModU = new TH2F("hModU", "Modules UpSteam RO", 4, 0.5, 4.5, 4, 0.5, 4.5);
  TH2F * hModD = new TH2F("hModD", "Modules DownStream RO", 4, 0.5, 4.5, 4, 0.5, 4.5);
  TH2F * hChanU = new TH2F("hChanU", "Channels UpStream RO", 8, 0.5, 4.5, 8, 0.5, 4.5);
  TH2F * hChanD = new TH2F("hChanD", "Channels DownStream RO", 8, 0.5, 4.5, 8, 0.5, 4.5);

  hModU->SetMinimum(threshold);
  hModD->SetMinimum(threshold);
  hChanU->SetMinimum(threshold);
  hChanD->SetMinimum(threshold);

  if (singleEvent){
    hModU->SetMaximum(MAXADC*4);
    hModD->SetMaximum(MAXADC*4);
    hChanU->SetMaximum(MAXADC);
    hChanD->SetMaximum(MAXADC);
  }

  TH2F * hModU_time = new TH2F("hModU_time", "Modules UpSteam Timing", 4, 0.5, 4.5, 4, 0.5, 4.5);
  TH2F * hModD_time = new TH2F("hModD_time", "Modules DownStream Timing", 4, 0.5, 4.5, 4, 0.5, 4.5);
  TH2F * hChanU_time = new TH2F("hChanU_time", "Channels UpStream Timing", 8, 0.5, 4.5, 8, 0.5, 4.5);
  TH2F * hChanD_time = new TH2F("hChanD_time", "Channels DownStream Timing", 8, 0.5, 4.5, 8, 0.5, 4.5);

  hModU_time->SetMinimum(0);
  hModD_time->SetMinimum(0);
  hChanU_time->SetMinimum(0);
  hChanD_time->SetMinimum(0);

  /*
  hModU_time->SetMaximum(120);
  hModD_time->SetMaximum(120);
  hChanU_time->SetMaximum(120);
  hChanD_time->SetMaximum(120);
  */

  TH2F * hModU_nhits = new TH2F("hModU_nhits", "Modules UpSteam nHits", 4, 0.5, 4.5, 4, 0.5, 4.5);
  TH2F * hModD_nhits = new TH2F("hModD_nhits", "Modules DownStream nHits", 4, 0.5, 4.5, 4, 0.5, 4.5);
  TH2F * hChanU_nhits = new TH2F("hChanU_nhits", "Channels UpStream nHits", 8, 0.5, 4.5, 8, 0.5, 4.5);
  TH2F * hChanD_nhits = new TH2F("hChanD_nhits", "Channels DownStream nHits", 8, 0.5, 4.5, 8, 0.5, 4.5);

  hModU_nhits->SetMaximum(t1041->GetEntries() / 16);
  hModD_nhits->SetMaximum(t1041->GetEntries() / 16);
  hChanU_nhits->SetMaximum(t1041->GetEntries() / 64);
  hChanD_nhits->SetMaximum(t1041->GetEntries() / 64);

  TH2F * hModU_ntriggers = new TH2F("hModU_ntriggers", "Modules UpSteam nTriggers", 4, 0.5, 4.5, 4, 0.5, 4.5);
  TH2F * hModD_ntriggers = new TH2F("hModD_ntriggers", "Modules DownStream nTriggers", 4, 0.5, 4.5, 4, 0.5, 4.5);
  TH2F * hChanU_ntriggers = new TH2F("hChanU_ntriggers", "Channels U_ntriggerspStream nTriggers", 8, 0.5, 4.5, 8, 0.5, 4.5);
  TH2F * hChanD_ntriggers = new TH2F("hChanD_ntriggers", "Channels DownStream nTriggers", 8, 0.5, 4.5, 8, 0.5, 4.5);

  hModU_ntriggers->SetMaximum(t1041->GetEntries() * 4);
  hModD_ntriggers->SetMaximum(t1041->GetEntries() * 4);
  hChanU_ntriggers->SetMaximum(t1041->GetEntries());
  hChanD_ntriggers->SetMaximum(t1041->GetEntries());

  Int_t start=0; Int_t end=t1041->GetEntries();
  if (singleEvent) {
    start=ndisplay-1;
    end=ndisplay;
  }
  for (Int_t i=start; i<end; i++) {
    t1041->GetEntry(i);
    for (Int_t j = 0; j < event->NPadeChan(); j++){
      PadeChannel pch = event->GetPadeChan(j);

      // loop over ADC samples
      UShort_t * wform = pch.GetWform();
      UShort_t max = 0;
      Int_t maxTime = 0;

      // find the value to plot (just using the peak sample value now)
      bool overMax = false;
      int nMaximumsFound = 0;

      for (Int_t k = 0; k < event->GetPadeChan(j).__SAMPLES(); k++){
	if(wform[k] > MAXADC) {
	  overMax = true;
	  break;
	}
	if (wform[k] > threshold && wform[k] > max) {
	  max = wform[k];
	  maxTime = k;
	  nMaximumsFound++;
	}
      }

      ///////////////////////////////////////////////////////////////
      int module,fiber;
      mapper->Pade2Fiber(pch.GetBoardID(), pch.GetChannelID(), module, fiber);
      int xm,ym;
      mapper->ModuleXY(module, xm, ym);

      float xf,yf;
      int fiberID = module * 100 + fiber;
      mapper->FiberXY(fiberID, xf, yf);

      if(module < 0) {
	hModU_ntriggers->Fill(xm, ym);
	hChanU_ntriggers->Fill(xf, yf);
      }
      else {
	hModD_ntriggers->Fill(xm, ym);
	hChanD_ntriggers->Fill(xf, yf);
      }

      if(overMax) continue;
      if(nMaximumsFound == 120) continue;
      if(max <= threshold) continue;

      if (module<0) {
	hModU->Fill(xm, ym, max);
	hModU_time->Fill(xm, ym, maxTime);
	hModU_nhits->Fill(xm, ym);

	hChanU->Fill(xf, yf, max);
	hChanU_time->Fill(xf, yf, maxTime);
	hChanU_nhits->Fill(xf, yf);
      }
      else {
	hModD->Fill(xm, ym, max);
	hModD_time->Fill(xm, ym, maxTime);
	hModD_nhits->Fill(xm, ym);

	hChanD->Fill(xf, yf, max);
	hChanD_time->Fill(xf, yf, maxTime);
	hChanD_nhits->Fill(xf, yf);
      }
      
    }
  }

  hModD->Divide(hModD_nhits);
  hModU->Divide(hModU_nhits);
  hChanD->Divide(hChanD_nhits);
  hChanU->Divide(hChanU_nhits);

  hModD_time->Divide(hModD_nhits);
  hModU_time->Divide(hModU_nhits);
  hChanD_time->Divide(hChanD_nhits);
  hChanU_time->Divide(hChanU_nhits);

  TCanvas *c1=new TCanvas("c1","Average Peak Height",800,800);
  c1->Divide(2,2);

  c1->cd(1);
  hModD->Draw("colz");
  c1->cd(2);
  hModU->Draw("colz");
  c1->cd(3)->SetGrid();
  hChanD->Draw("colz");
  c1->cd(4)->SetGrid();
  hChanU->Draw("colz");
  drawChannelMap(c1);
  c1->SaveAs("AvgPeakHeight.gif");

  TCanvas * c2 = new TCanvas("c2", "Average Peak Timing", 800, 800);
  c2->Divide(2, 2);

  c2->cd(1);
  hModD_time->Draw("colz");
  c2->cd(2);
  hModU_time->Draw("colz");
  c2->cd(3)->SetGrid();
  hChanD_time->Draw("colz");
  c2->cd(4)->SetGrid();
  hChanU_time->Draw("colz");
  drawChannelMap(c2);
  c2->SaveAs("AvgPeakTiming.gif");

  TCanvas * c3 = new TCanvas("c3", "Number of Hits", 800, 800);
  c3->Divide(2, 2);

  c3->cd(1);
  hModD_nhits->Draw("colz");
  c3->cd(2);
  hModU_nhits->Draw("colz");
  c3->cd(3)->SetGrid();
  hChanD_nhits->Draw("colz");
  c3->cd(4)->SetGrid();
  hChanU_nhits->Draw("colz");
  drawChannelMap(c3);
  c3->SaveAs("nHits.gif");

  TCanvas * c4 = new TCanvas("c4", "Number of Triggers", 800, 800);
  c4->Divide(2, 2);

  c4->cd(1);
  hModD_ntriggers->Draw("colz");
  c4->cd(2);
  hModU_ntriggers->Draw("colz");
  c4->cd(3)->SetGrid();
  hChanD_ntriggers->Draw("colz");
  c4->cd(4)->SetGrid();
  hChanU_ntriggers->Draw("colz");
  drawChannelMap(c4);
  c4->SaveAs("nTriggers.gif");


}

void channelDisplay(TString fdat, int board, int channel) {

  gStyle->SetOptStat(0);

  TFile *f = new TFile(fdat);
  if (f->IsZombie()){
    cout << "Cannot open file: " << fdat << endl;
    return;
  }

  TBEvent *event = new TBEvent();
  TTree *t1041 = (TTree*)f->Get("t1041"); 
  TBranch *bevent = t1041->GetBranch("tbevent");
  bevent->SetAddress(&event);

  TCanvas * canv = new TCanvas("canv", "canv", 800, 800);
  canv->cd();

  TH1F * dummy = new TH1F("dummy", "dummy", 120, 0, 120);
  dummy->GetYaxis()->SetRangeUser(0, 2500);
  dummy->Draw();
        
  vector<TH1F*> waves;

  TH1F * wave = new TH1F("wave", "wave", 120, 0, 120);

  int nplots = 0;

  for (Int_t i = 0; i < t1041->GetEntries(); i++) {
    t1041->GetEntry(i);
    
    for (int j = 0; j < event->NPadeChan(); j++){
      PadeChannel pch = event->GetPadeChan(j);
      
      if((int)pch.GetBoardID() != board || (int)pch.GetChannelID() != channel) continue;
      
      pch.GetHist(wave);
      
      nplots++;
      TH1F * wavecopy = (TH1F*)wave->Clone("wave_"+TString(Form("%d", nplots)));
      waves.push_back(wavecopy);
      
    }
    
  }
  
  int nBigPeaks = 0;
  for(unsigned int ui = 0; ui < waves.size(); ui++) {
    if(waves[ui]->GetMaximum() > 400) {
      waves[ui]->SetLineColor(nBigPeaks+2);
      nBigPeaks++;
    }
    waves[ui]->Draw("same");
  }
  
}
