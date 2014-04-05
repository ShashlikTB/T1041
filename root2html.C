#include "TString.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TObject.h"
#include "TKey.h"
#include "TH1.h"
#include <iostream>

void root2html(TString fname, TString outDir){
  TFile* fin = new TFile(fname,"READ");
  TIter nextkey(gDirectory->GetListOfKeys()); 
  TCanvas *c1=new TCanvas("c1","histo");
  while (TKey *key = (TKey*)nextkey() ){
    TObject *obj = key->ReadObj(); 
     if (obj->IsA()->InheritsFrom("TH1")) {
      TH1* h = (TH1*)obj; 
      TString name=h->GetName();
      name.ReplaceAll("(","_");  // simple name cleaning for file creation
      name.ReplaceAll(")","_");
      name.ReplaceAll(" ","_");
      //      cout << name << endl;
      h->Draw();
      c1->Print(outDir+"/"+name+".png");
    }
  }
  fin->Close();
}

