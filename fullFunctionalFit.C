// m; m0, sigma, alpha, n, scale
Double_t crystalBall(Double_t * x, Double_t * par) {

  Double_t t = (x[0] - par[6]) / par[7];
  if(par[8] < 0) t = -t;

  Double_t absAlpha = fabs((Double_t)par[8]);

  if(t >= -absAlpha) {
    return par[10] * exp(-0.5 * t * t);
  }
  else {
    Double_t a = TMath::Power(par[9] / absAlpha, par[9]) * exp(-0.5 * absAlpha * absAlpha);
    Double_t b = par[9] / absAlpha - absAlpha;

    return par[10] * a / TMath::Power(b - t, par[9]);
  }

}

// 
Double_t fitf(Double_t * x, Double_t * par) {
  
  Double_t val = crystalBall(x, par);
  //Double_t val = par[6] * TMath::Poisson(x[0], par[7]);
  for(int i = 0; i < 4; i++) {
    // x; scale, mean nphotons, gaus mean, gaus sigma, peak spacing, pedastal
    val += par[0] * TMath::Poisson(i, par[1]) * TMath::Gaus(x[0] - par[4]*i - par[5], par[2], par[3]);
  }

  return val;
}

TH1D * fakeHistogram() {

  TRandom3 * rng = new TRandom3(0);

  TH1D * fake = new TH1D("fake", "fake", 800, 0, 800);
  for(int i = 0; i < 10000; i++) {
    double energy = 2 * rng->Gaus(12 * rng->Poisson(2), 3);
    double pedastal = .5 * rng->Gaus(0, 5);

    fake->Fill(energy + pedastal);
  }

  return fake;
}

void fullFunctionalFit() {

  TFile * f = new TFile("hist_20ns_LED_optimizedBias.root", "READ");
  
  TH1D * h = (TH1D*)f->Get("peakHeight_board-116_chan-8");

  TF1 * func = new TF1("fitf", fitf, 0, 800, 11);
  //TF1 * func = new TF1("fitf", fitf, 0, 800, 8);
  // x; scale, mean nphotons, gaus mean, gaus sigma, peak spacing, pedastal, m0, sigma, alpha, n
  Double_t par[12] = {1000, 1, 3, .5, 10, 100, 100, 20, -.9, 1, 1000, 1};
  //Double_t par[8] = {1000, 1, 3, .5, 10, 100, 100, 1};
  TString names[11] = {"Scale", "Mean Nphotons", "Gaus Mean", "Gaus sigma", "Peak spacing", "Pedastal",
		       "cb m0", "cb sigma", "cb alpha", "cb n", "cb scale"};

  func->SetParameters(par);
  func->SetParLimits(7, 0, 20);
  //func->SetParNames(names);
  //func->SetParName(11, "Gaus sigma scaling");
  func->SetNpx(1000);

  h->Fit(func);

  h->Draw();
}
