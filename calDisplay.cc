#include "calDisplay.h"

const UShort_t threshold = 200;

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

  Bool_t singleEvent = ndisplay >= 0;

  Mapper * mapper = Mapper::Instance();

  channelHistogram(bool isUpstream, TString title, Float_t zmin, Float_t zmax);
  TH2F * hModU = (TH2F*)moduleHistogram(true, "RO", threshold, MAXADC);
  TH2F * hModD = (TH2F*)moduleHistogram(false, "RO", threshold, MAXADC);
  TH2F * hChanU = (TH2F*)channelHistogram(true, "RO", threshold, MAXADC);
  TH2F * hChanD = (TH2F*)channelHistogram(false, "RO", threshold, MAXADC);

  TH2F * hModU_time = (TH2F*)moduleHistogram(true, "Timing", 0, -1);
  TH2F * hModD_time = (TH2F*)moduleHistogram(false, "Timing", 0, -1);
  TH2F * hModU_time = (TH2F*)channelHistogram(true, "Timing", 0, -1);
  TH2F * hChanD_time = (TH2F*)channelHistogram(false, "Timing", 0, -1);

  TH2F * hModU_nhits = (TH2F*)moduleHistogram(true, "nHits", -1, t1041->GetEntries() / 16);
  TH2F * hModD_nhits = (TH2F*)moduleHistogram(false, "nHits", -1, t1041->GetEntries() / 16);
  TH2F * hModU_nhits = (TH2F*)channelHistogram(true, "nHits", -1, t1041->GetEntries() / 64);
  TH2F * hChanD_nhits = (TH2F*)channelHistogram(false, "nHits", -1, t1041->GetEntries() / 64);

  TH2F * hModU_ntriggers = (TH2F*)moduleHistogram(true, "nTriggers", -1, t1041->GetEntries() * 4);
  TH2F * hModD_ntriggers = (TH2F*)moduleHistogram(false, "nTriggers", -1, t1041->GetEntries() * 4);
  TH2F * hModU_ntriggers = (TH2F*)channelHistogram(true, "nTriggers", -1, t1041->GetEntries());
  TH2F * hChanD_ntriggers = (TH2F*)channelHistogram(false, "nTriggers", -1, t1041->GetEntries());

  Int_t start = 0; 
  Int_t end = t1041->GetEntries();

  if (singleEvent) {
    start=ndisplay;
    end=ndisplay + 1;
  }

  for (Int_t i=start; i<end; i++) {
    t1041->GetEntry(i);

    for (Int_t j = 0; j < event->NPadeChan(); j++){
      PadeChannel pch = event->GetPadeChan(j);

      UShort_t * wform = pch.GetWform();
      UShort_t max = 0;
      Int_t maxTime = 0;

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

  drawCalorimeterPlot("AvgPeakHeight", hModU, hModD, hChanU, hChanD);

  drawCalorimeterPlot("AvgPeakTiming", hModU_time, hModD_time, hChanU_time, hChanD_time);

  drawCalorimeterPlot("NHits", hModU_nhits, hModD_nhits, hChanU_nhits, hChanD_nhits);

  drawCalorimeterPlot("NTriggers", hModU_ntriggers, hModD_ntriggers, hChanU_ntriggers, hChanD_ntriggers);

}

void displaySingleChannel(TString fdat, int board, int channel) {

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

void displaySingleEvent(TString fdat, int display) {

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
  t1041->GetEntry(display);

  for(int i = 0; i < event->NPadeChan(); i++) {
    PadeChannel pch = event->GetPadeChan(i);

    pch.GetHist(wave);

    nplots++;
    TH1F * wavecopy = (TH1F*)wave->Clone("wave_"+TString(Form("%d", nplots)));
    waves.push_back(wavecopy);
    
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


void displayAllBigPeaks(TString fdat) {

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
  t1041->GetEntry(display);

  for(int i = 0; i < event->NPadeChan(); i++) {
    PadeChannel pch = event->GetPadeChan(i);

    pch.GetHist(wave);

    if(wave->GetMaximum() < threshold) continue;

    nplots++;
    TH1F * wavecopy = (TH1F*)wave->Clone("wave_"+TString(Form("%d", nplots)));
    waves.push_back(wavecopy);
    
  }
    
  int nBigPeaks = 0;
  for(unsigned int ui = 0; ui < waves.size(); ui++) {
    waves[ui]->SetLineColor(nBigPeaks+2);
    nBigPeaks++;

    waves[ui]->Draw("same");
  }
  
}