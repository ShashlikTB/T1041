Double_t fitf(Double_t * x, Double_t * par) {
  return par[0] * TMath::Poisson(x[0], par[1]);
}

void hurr() {

  TFile * f = new TFile("peakSpacings.root", "READ");

  TH1D * h = (TH1D*)f->Get("peakIntegral_127");

  TF1 * func = new TF1("func", fitf, 0, 12, 2);
  func->SetParameters(500, 1);

  h->Fit(func);

  h->Draw();
}
