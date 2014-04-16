#include <vector>

#include "TSpectrum.h"

using namespace std;

Double_t poissonFunc(Double_t * x, Double_t * par) {
  return par[0] * TMath::Poisson(x[0], par[1]);
}

void findPeaks(TString input) {

  gROOT->Reset();
  gROOT->SetBatch(true);
  gROOT->SetStyle("Plain");
  gStyle->SetOptStat(0000);
  gStyle->SetOptTitle(1);
  gStyle->SetOptFit(1);

  doFits(input);

  TFile * f = new TFile("peakSpacings_"+input, "READ");

  TH1D * h_exp_112 = new TH1D("exp_112", "Expected Number of #gammas;Channel;<N_{#gamma}>", 128, 0, 128); h_exp_112->Sumw2();
  TH1D * h_exp_113 = new TH1D("exp_113", "Expected Number of #gammas;Channel;<N_{#gamma}>", 128, 0, 128); h_exp_113->Sumw2();
  TH1D * h_exp_115 = new TH1D("exp_115", "Expected Number of #gammas;Channel;<N_{#gamma}>", 128, 0, 128); h_exp_115->Sumw2();
  TH1D * h_exp_116 = new TH1D("exp_116", "Expected Number of #gammas;Channel;<N_{#gamma}>", 128, 0, 128); h_exp_116->Sumw2();
  
  TH1D * h_exp_perBoard = new TH1D("all_expected", "Expected Number of #gamma for each SiPM;;<N_{#gamma}>", 4, 0, 128); h_exp_perBoard->Sumw2();

  TH1D * h_temp = new TH1D("temp", "temp", 120, 0, 12); h_temp->Sumw2();

  for(int i = 0; i < 128; i++) {
    
    TH1D * h = (TH1D*)f->Get("peakIntegral_"+TString(Form("%d", i)));

    TF1 * func = new TF1("func", poissonFunc, 0, 12, 2);
    func->SetParameters(500, 1);

    if((i % 32) != 0 || i == 0) h_temp->Add(h);
    if(i != 0 && (i % 32) == 0) {
      TFitResultPtr overallres = h_temp->Fit(func, "QS");
      h_exp_perBoard->SetBinContent((int)(i/32), overallres->Parameter(1));
      h_exp_perBoard->SetBinError((int)(i/32), overallres->ParError(1));
      h_temp->Reset();
      h_temp->Add(h);
    }

    TF1 * func2 = new TF1("func2", poissonFunc, 0, 12, 2);
    func2->SetParameters(500, 1);

    TFitResultPtr fitres = h->Fit(func2, "QS");
    
    if(fitres->ParError(1) > fitres->Parameter(1)) continue;

    if(i < 32) {
      h_exp_112->SetBinContent(i+1, fitres->Parameter(1));
      h_exp_112->SetBinError(i+1, fitres->ParError(1));
    }
    else if(i < 64) {
      h_exp_113->SetBinContent(i+1, fitres->Parameter(1));
      h_exp_113->SetBinError(i+1, fitres->ParError(1));
    }
    else if(i < 96) {
      h_exp_115->SetBinContent(i+1, fitres->Parameter(1));
      h_exp_115->SetBinError(i+1, fitres->ParError(1));
    }
    else {
      h_exp_116->SetBinContent(i+1, fitres->Parameter(1));
      h_exp_116->SetBinError(i+1, fitres->ParError(1));
    }

  }

  TFitResultPtr overallres = h_temp->Fit(func, "QS");
  h_exp_perBoard->SetBinContent(4, overallres->Parameter(1));
  h_exp_perBoard->SetBinError(4, overallres->ParError(1));

  TCanvas * canv = new TCanvas("canv", "Plot", 10, 10, 2000, 2000);
  canv->SetLogy(false);

  h_exp_112->SetLineWidth(3);
  h_exp_112->SetLineColor(kBlack);

  h_exp_113->SetLineWidth(3);
  h_exp_113->SetLineColor(kBlue);

  h_exp_115->SetLineWidth(3);
  h_exp_115->SetLineColor(kRed);

  h_exp_116->SetLineWidth(3);
  h_exp_116->SetLineColor(kGreen+3);

  h_exp_perBoard->SetLineWidth(5);
  h_exp_perBoard->SetLineColor(kMagenta);

  TLegend * leg = new TLegend(0.60, 0.65, 0.85, 0.85, NULL, "brNDC");
  leg->SetFillColor(0);
  leg->SetTextSize(0.028);
  leg->AddEntry(h_exp_112, "Board 112", "LP");
  leg->AddEntry(h_exp_113, "Board 113", "LP");
  leg->AddEntry(h_exp_115, "Board 115", "LP");
  leg->AddEntry(h_exp_116, "Board 116", "LP");

  h_exp_112->Draw();
  h_exp_113->Draw("same");
  h_exp_115->Draw("same");
  h_exp_116->Draw("same");
  h_exp_perBoard->Draw("same");
  leg->Draw("same");

  canv->SaveAs("expectedPhotons.gif");


}


void doFits(TString input) {

  TFile *f = new TFile(input, "READ");

  char char[100];
  int boardIDs[4] = {112, 113, 115, 116};

  TH1D * h_spacing_112_01 = new TH1D("spacing_112_01", "Peak spacing 1 to 2;ADC Counts;Events", 50, 0, 50);
  TH1D * h_spacing_113_01 = new TH1D("spacing_113_01", "Peak spacing 1 to 2;ADC Counts;Events", 50, 0, 50);
  TH1D * h_spacing_115_01 = new TH1D("spacing_115_01", "Peak spacing 1 to 2;ADC Counts;Events", 50, 0, 50);
  TH1D * h_spacing_116_01 = new TH1D("spacing_116_01", "Peak spacing 1 to 2;ADC Counts;Events", 50, 0, 50);

  TH1D * h_spacing_112_12 = new TH1D("spacing_112_12", "Peak spacing 1 to 2;ADC Counts;Events", 50, 0, 50);
  TH1D * h_spacing_113_12 = new TH1D("spacing_113_12", "Peak spacing 1 to 2;ADC Counts;Events", 50, 0, 50);
  TH1D * h_spacing_115_12 = new TH1D("spacing_115_12", "Peak spacing 1 to 2;ADC Counts;Events", 50, 0, 50);
  TH1D * h_spacing_116_12 = new TH1D("spacing_116_12", "Peak spacing 1 to 2;ADC Counts;Events", 50, 0, 50);

  TH1D * h_spacing_112_23 = new TH1D("spacing_112_23", "Peak spacing 1 to 2;ADC Counts;Events", 50, 0, 50);
  TH1D * h_spacing_113_23 = new TH1D("spacing_113_23", "Peak spacing 1 to 2;ADC Counts;Events", 50, 0, 50);
  TH1D * h_spacing_115_23 = new TH1D("spacing_115_23", "Peak spacing 1 to 2;ADC Counts;Events", 50, 0, 50);
  TH1D * h_spacing_116_23 = new TH1D("spacing_116_23", "Peak spacing 1 to 2;ADC Counts;Events", 50, 0, 50);

  vector<TString> histNames;

  vector<TH1D*> peakHeights;
  peakHeights.resize(128);

  for(int i = 0; i < 4; i++) {
    for(int j = 0; j < 32; j++) {
      sprintf(char, "peakHeight_board-%d_chan-%d", boardIDs[i], j);
      histNames.push_back(TString(char));
    }
  }

  for(unsigned int ui = 0; ui < histNames.size(); ui++) peakHeights[ui] = (TH1D*)f->Get(histNames[ui]);

  vector<TH1D*> peakSpacing;
  peakSpacing.resize(128);
  for(unsigned int ui = 0; ui < 128; ui++) {
    peakSpacing[ui] = new TH1D("peakSpacing_"+TString(Form("%d", ui)), "peakSpacing_"+TString(Form("%d", ui)), 100, 0, 100);
    peakSpacing[ui]->Sumw2();
  }

  vector<TH1D*> peakIntegral;
  peakIntegral.resize(128);
  for(unsigned int ui = 0; ui < 128; ui++) {
    peakIntegral[ui] = new TH1D("peakIntegral_"+TString(Form("%d", ui)), "peakIntegral_"+TString(Form("%d", ui)), 120, 0, 12);
    peakIntegral[ui]->Sumw2();
  }

  Int_t boardID_, channel_, npeaks_;
  Float_t spacing01_, spacing12_, spacing23_;
  Float_t occupancy0_, occupancy1_, occupancy2_, occupancy3_;

  TTree * tree = new TTree("tree", "tree");
  tree->Branch("boardID", &boardID_, "boardID_/I");
  tree->Branch("channel", &channel_, "channel_/I");
  tree->Branch("npeaks", &npeaks_, "npeaks_/I");
  tree->Branch("spacing01", &spacing01_, "spacing01_/F");
  tree->Branch("spacing12", &spacing12_, "spacing12_/F");
  tree->Branch("spacing23", &spacing23_, "spacing23_/F");
  tree->Branch("occupancy0", &occupancy0_, "occupancy0_/F");
  tree->Branch("occupancy1", &occupancy1_, "occupancy1_/F");
  tree->Branch("occupancy2", &occupancy2_, "occupancy2_/F");
  tree->Branch("occupancy3", &occupancy3_, "occupancy3_/F");

  for(int i = 0; i < 128; i++) {

    TSpectrum * s = new TSpectrum();

    int npeaks = s->Search(peakHeights[i], 2, "nodraw", 0.005);
    Float_t * xpeaks = s->GetPositionX();

    vector<float> xpos;
    for(int j = 0; j < npeaks; j++) xpos.push_back(xpeaks[j]);
    sort(xpos.begin(), xpos.end());

    vector<float> endpoints;
    endpoints.push_back(0);

    for(int j = 0; j < npeaks - 2; j++) {
      endpoints.push_back(0.5 * (xpos[j+1] + xpos[j]));
    }
     
    endpoints.push_back(2000);

    for(unsigned int k = 0; k < endpoints.size() - 1; k++) {

      if(xpos[k] > 250) continue;

      TF1 * func = new TF1("func"+TString(Form("%d", k)), "gaus");
      func->SetRange(endpoints[k], endpoints[k+1]);

      TH1D * bkg = (TH1D*)s->Background(peakHeights[i], 20, "nodraw");
      TH1D * sig = (TH1D*)peakHeights[i]->Clone("sig"+TString(Form("%d", k)));
      sig->Add(bkg, -1);

      TFitResultPtr fitres = sig->Fit(func, "+QS", "", endpoints[k], endpoints[k+1]);
      Double_t norm = fitres->Parameter(0);
      Double_t normError = fitres->ParError(0);

      peakIntegral[i]->SetBinContent(peakIntegral[i]->GetXaxis()->FindBin(k), norm);
      peakIntegral[i]->SetBinError(peakIntegral[i]->GetXaxis()->FindBin(k), normError);

    }

    if(i < 32) boardID_ = 112;
    else if(i < 64) boardID_ = 113;
    else if(i < 96) boardID_ = 115;
    else boardID_ = 116;

    channel_ = (i % 32);
    npeaks_ = xpos.size();

    spacing01_ = (npeaks_ > 1) ? xpos[1] - xpos[0] : -1;
    spacing12_ = (npeaks_ > 2) ? xpos[2] - xpos[1] : -1;
    spacing23_ = (npeaks_ > 3) ? xpos[3] - xpos[2] : -1;

    occupancy0_ = peakIntegral[i]->GetBinContent(1);
    occupancy1_ = peakIntegral[i]->GetBinContent(2);
    occupancy2_ = peakIntegral[i]->GetBinContent(3);
    occupancy3_ = peakIntegral[i]->GetBinContent(4);

    tree->Fill();

    for(unsigned int k = 0; k < xpos.size() - 1; k++) {

      peakSpacing[i]->Fill(xpos[k+1] - xpos[k]);

      if(k == 0) {
	if(i < 32) h_spacing_112_01->Fill(xpos[k+1] - xpos[k]);
	else if(i < 64) h_spacing_113_01->Fill(xpos[k+1] - xpos[k]);
	else if(i < 96) h_spacing_115_01->Fill(xpos[k+1] - xpos[k]);
	else h_spacing_116_01->Fill(xpos[k+1] - xpos[k]);
      }

      if(k == 1) {
	if(i < 32) h_spacing_112_12->Fill(xpos[k+1] - xpos[k]);
	else if(i < 64) h_spacing_113_12->Fill(xpos[k+1] - xpos[k]);
	else if(i < 96) h_spacing_115_12->Fill(xpos[k+1] - xpos[k]);
	else h_spacing_116_12->Fill(xpos[k+1] - xpos[k]);
      }

      if(k == 2) {
	if(i < 32) h_spacing_112_23->Fill(xpos[k+1] - xpos[k]);
	else if(i < 64) h_spacing_113_23->Fill(xpos[k+1] - xpos[k]);
	else if(i < 96) h_spacing_115_23->Fill(xpos[k+1] - xpos[k]);
	else h_spacing_116_23->Fill(xpos[k+1] - xpos[k]);
      }

    }

  }

  TFile * out = new TFile("peakSpacings_"+input, "RECREATE");
  
  tree->Write();

  for(int i = 0; i < 128; i++) {
    peakSpacing[i]->Write();
    peakIntegral[i]->Write();
  }

  h_spacing_112_01->Write();
  h_spacing_113_01->Write();
  h_spacing_115_01->Write();
  h_spacing_116_01->Write();

  h_spacing_112_12->Write();
  h_spacing_113_12->Write();
  h_spacing_115_12->Write();
  h_spacing_116_12->Write();

  h_spacing_112_23->Write();
  h_spacing_113_23->Write();
  h_spacing_115_23->Write();
  h_spacing_116_23->Write();
  
  TCanvas * canv = new TCanvas("canv", "Plot", 10, 10, 2000, 2000);
  canv->SetLogy(false);

  TF1 * func_gaus = new TF1("func_gaus", "gaus", 0, 20);

  h_spacing_112_12->SetLineWidth(3);
  h_spacing_112_12->Fit(func_gaus);
  h_spacing_112_12->Draw();
  canv->SaveAs("spacing_112.gif");

  h_spacing_113_12->SetLineWidth(3);
  h_spacing_113_12->Draw();
  canv->SaveAs("spacing_113.gif");

  h_spacing_115_12->SetLineWidth(3);
  h_spacing_115_12->Draw();
  canv->SaveAs("spacing_115.gif");

  h_spacing_116_12->SetLineWidth(3);
  h_spacing_116_12->Draw();
  canv->SaveAs("spacing_116.gif");

  out->Write();
  out->Close();

}
