//////          Authors: S. Bein, H. Prosper 4/12/2014:
//////                    Modified code from J.Adams and A.Santra, added stack
/////                     display features and formatted to sync with TB Gui 
//////         4/20/2014 BH: Update to start using tools in TBReco library  */
#include <TH2.h>
#include <TH1F.h>
#include <TStyle.h>
#include <TROOT.h>
#include <TFile.h>
#include <TSpectrum.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TBox.h>
#include <TTree.h>
#include <TSystem.h>
#include <TString.h>
#include <TClassTable.h>
#include <vector>
#include <iostream>
#include "THStack.h"
#include "TLegend.h"
#include "TBEvent.h"
#include "TBReco.h"
#include "WC.h"
#include "WCPlanes.h"

#include <iostream>
#include <fstream>
#include <sstream>
using std::cout;
using std::endl;


void SetHistoPrefs(THStack* stack){
  double LabelSize = 0.045;
  stack->GetXaxis()->SetTitle("x-position (mm)");
  stack->GetYaxis()->SetTitle("y-position (mm)");
  stack->GetXaxis()->SetTitleSize(LabelSize);
  stack->GetYaxis()->SetTitleSize(LabelSize);
  stack->GetXaxis()->SetLabelSize(LabelSize);
  stack->GetYaxis()->SetLabelSize(LabelSize);
  stack->GetXaxis()->SetTitleOffset(1.3);
  stack->GetYaxis()->SetTitleOffset(1.5);
}


WCPlanes::WCPlanes()
  : c1(0), wcReco(WCReco()), isFirstEvent(true) {}

WCPlanes::WCPlanes(TCanvas* canvas)
  : c1(canvas), wcReco(WCReco()), isFirstEvent(true)
{
 
  WC1_Beam      = new TH2I("WC1_Beam", "WC1 Hits, Quality",            32,-64,64,32,-64,64);
  WC2_Beam      = new TH2I("WC2_Beam", "WC2 Hits, Quality",            32,-64,64,32,-64,64);
  WC_Shashlik   = new TH2F("WC_Shashlik", "Beam Hits in Shashlik Face",  32,-64,64,32,-64,64); 
  WC1_hits = new TH2I("WC1 Hits, In Time","WC1 Hits, In Time",32,-64,64,32,-64,64);
  WC2_hits = new TH2I("WC2 Hits, In Time","WC2 Hits, In Time",32,-64,64,32,-64,64);
  Scint1   = new TH2F("Scint1",  "Scintillator 1 Hits", 128, -64, 64, 128, -64, 64);
  Scint2   = new TH2F("Scint2",  "Scintillator 2 Hits", 128, -64, 64, 128, -64, 64);
  Shashlik = new TH2F("Shashlik","Shashlik Hits",       128, -64, 64, 128, -64, 64);

  
  WC1_Beam->SetFillColor(2);
  WC2_Beam->SetFillColor(2);
  WC1_hits->SetFillColor(13);
  WC2_hits->SetFillColor(13);


  TableX = 20.0;
  TableY = 20.0;
    
  hStack1 = 0;
  hStack2 = 0;
  
  
}
WCPlanes::~WCPlanes(){}


void WCPlanes::CacheWCMeans(string meanfile, string rootfilename){

  TFile *f = new TFile(rootfilename.c_str());
  TTree* t1041 = (TTree*)f->Get("t1041");
  wcReco.AddTree(t1041);

  wcReco.GetTDCcuts(mean, tLow, tHigh); 
  ofstream myfile;
  myfile.open (meanfile.c_str());
  for(int i=0; i<NTDC;i++) myfile << i+1 << "\t" << tLow[i] <<"\t"<<mean[i] <<"\t"<<tHigh[i] << endl;
  myfile.close();

  wcReco.GetTDChists(TDC);
  TFile* tdcFile = new TFile("tdc_dists.root","RECREATE");
  gROOT->SetBatch(true);  
  for(int i = 0; i<NTDC; i++) TDC[i]->Write();
  tdcFile->Close(); 
  gROOT->SetBatch(false);
  //for (int i = 0; i<NTDC; i++)TDC[i]->Reset();
  
}

void WCPlanes::GetWCMeans(string meanfile, int *tLow_, int *mean_, int *tHigh_){
  string line;
  ifstream myfile (meanfile.c_str());
  if (myfile.is_open())
    {
      int i = 0;
      while ( getline (myfile,line) )
	{
	  std::stringstream linestream(line);
	  int tdcnum;
	  linestream >> tdcnum>> tLow_[i] >> mean_[i] >> tHigh_[i];
	  i++;
	}
      myfile.close();
    }
  else cout << "Unable to open file"; 

}

void WCPlanes::Draw(TBEvent* event, Util& util)
{
  if (isFirstEvent){
    c1->Divide(2,1);
    isFirstEvent = false;
  }

  if (! util.accumulate){
    WC1_Beam->Reset();
    WC2_Beam->Reset();
    WC1_hits->Reset();
    WC2_hits->Reset();
    Scint1->Reset();
    Scint2->Reset();
  }
    
  GetWCMeans("meanfile.txt", tLow, mean, tHigh);

  hitsX1=event->GetWChitsX(1,tLow,tHigh);   // fetch x,y hits in chambers 1 and 2
  hitsY1=event->GetWChitsY(1,tLow,tHigh);   // only selecting in-time hits
  hitsX2=event->GetWChitsX(2,tLow,tHigh);
  hitsY2=event->GetWChitsY(2,tLow,tHigh);
  

  util.x1hit = 64;
  util.y1hit = 64;
  util.x2hit = 64;
  util.y2hit = 64;

  bool foundgood = false;
  for(unsigned h1=0; h1<hitsX1.size(); ++h1)
    { // loop over X1 
      for(unsigned h2=0; h2<hitsY1.size(); ++h2){ // loop over Y1
	for(unsigned h3=0; h3<hitsX2.size(); ++h3){ // loop over X2 
	  for(unsigned h4=0; h4<hitsY2.size(); ++h4){ // loop over Y2
	    WCtrack multiTrack(hitsX1[h1], hitsY1[h2], hitsX2[h3], hitsY2[h4]);
	    float trackX, trackY;
	    multiTrack.Project(zSC1, trackX, trackY);
	    if(foundgood) continue;
	    if(fabs(trackX)>50 || fabs(trackY)>50)continue;
	    Scint1->Fill(trackX, trackY, 1);
	    multiTrack.Project(zSC2, trackX, trackY);
	    if(fabs(trackX)>50 || fabs(trackY)>50)continue;
	    Scint2->Fill(trackX, trackY, 1);
	    multiTrack.Project(zWC1, trackX, trackY);
	    WC1_Beam->Fill(trackX, trackY, 1);
	    util.x1hit = trackX;
	    util.y1hit = trackY;
	    multiTrack.Project(zWC2, trackX, trackY);
	    WC2_Beam->Fill(trackX, trackY, 1);
	    util.x2hit = trackX;
	    util.y2hit = trackY;
	      
	    foundgood = true;
	  }
	}
      }
    }

  util.WC1Xallhits.clear();
  util.WC2Xallhits.clear();
  util.WC1Yallhits.clear();
  util.WC2Yallhits.clear();
  
  
  for(unsigned h1=0; h1<hitsX1.size(); h1++)
    {
      for(unsigned h2=0; h2<hitsY1.size(); h2++){
	WC1_hits->Fill(hitsX1[h1].GetX(), hitsY1[h2].GetY(), 1);
	if(! util.stealthmode){cout<<"===\n";hitsX1[h1].Dump();hitsY1[h2].Dump();cout<<"===\n"<<endl;}
	util.WC1Xallhits.push_back(hitsX1[h1].GetX());  util.WC1Yallhits.push_back(hitsY1[h2].GetY());
      }
    }
  
  
  for(unsigned h3=0; h3<hitsX2.size(); h3++)
    {
      for(unsigned h4=0; h4<hitsY2.size(); h4++){
	WC2_hits->Fill(hitsX2[h3].GetX(), hitsY2[h4].GetY(), 1);
	if(! util.stealthmode){cout<<"===\n";hitsX2[h3].Dump();hitsY2[h4].Dump();cout<<"===\n"<<endl;}
	util.WC2Xallhits.push_back(hitsX2[h3].GetX()); util.WC2Yallhits.push_back(hitsY2[h4].GetY());
      }
    }


  hEmpty  = new TH2I("Empty","Empty",32,-64,64,32,-64,64); 
  if(hStack1!=0) delete hStack1;
  TString name1; name1.Form("Upstream WC - evt %d", util.eventNumber);
  TString name2; name2.Form("Downstream WC - evt %d", util.eventNumber);
  hStack1 = new THStack(name1,name1);
  hStack2 = new THStack(name2,name2);
  c1->cd(1); 
  gPad->SetFrameFillColor(0);
  if(util.WC_showIThits)hStack1->Add(WC1_hits);
  if(util.WC_showQhits)hStack1->Add(WC1_Beam);
  if(!(util.WC_showQhits||util.WC_showIThits)) 
    hStack1->Add(hEmpty);
  if(! util.stealthmode)
    {
      hStack1->Draw();
      SetHistoPrefs(hStack1);
      gPad->BuildLegend(0, .77, 0.5, 0.93);
    }

  c1->cd(2);
  if(util.WC_showIThits)hStack2->Add(WC2_hits);
  if(util.WC_showQhits) hStack2->Add(WC2_Beam);
  if(!(util.WC_showQhits||util.WC_showIThits))//||util.WC_showQhits))
    hStack2->Add(hEmpty);
  if(! util.stealthmode)
    {
      hStack2->Draw();
      SetHistoPrefs(hStack2);
      gPad->BuildLegend(0, .77, 0.5, 0.93);     
      c1->Update();
    }
}
