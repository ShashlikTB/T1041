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

void DrawShashBox(){
  vector<TBox*> bx;
  for (int iy=-28;iy<=14;iy+=14){
    for (int ix=-28; ix<=14;ix+=14){
      bx.push_back(new TBox(ix,iy,ix+14,iy+14));
      bx.back()->SetFillStyle(0);
      bx.back()->Draw("l");
    }
  }
  TBox *RR = new TBox(-42,-42, 42, 42); 
  RR->SetFillStyle(0);
  RR->Draw("l");
}

void DrawScintBox(){
  TBox *SS = new TBox(-50,-50, 50, 50);
  SS->SetFillStyle(0);
  SS->Draw("l");
}

void SetHistoPreferences(THStack* stack){
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
  WC1=new TH2I("WC1 Hits, In Time","WC1 Hits, In Time",32,-64,64,32,-64,64);
  WC2=new TH2I("WC2 Hits, In Time","WC2 Hits, In Time",32,-64,64,32,-64,64);
  WCO1=new TH2I("WC1 Hits, Out of Time","WC1 Hits, Out of Time",32,-64,64,32,-64,64);
  WCO2=new TH2I("WC2 Hits, Out of Time","WC2 Hits, Out of Time",32,-64,64,32,-64,64);

  
  WC1_Beam->SetFillColor(2);
  WC2_Beam->SetFillColor(2);
  WC1->SetFillColor(13);
  WC2->SetFillColor(13);
  WCO1->SetFillColor(17);
  WCO2->SetFillColor(17);


    
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

void WCPlanes::GetWCMeans(string meanfile, int *tLow, int *mean, int *tHigh){
  string line;
  ifstream myfile (meanfile.c_str());
  if (myfile.is_open())
    {
      int i = 0;
      while ( getline (myfile,line) )
	{
	  std::stringstream linestream(line);
	  int tdcnum;
	  linestream >> tdcnum>> tLow[i] >> mean[i] >> tHigh[i];
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
    WC1->Reset();
    WC2->Reset();
    WCO1->Reset();
    WCO2->Reset();
  }
    
  GetWCMeans("meanfile.txt", tLow, mean, tHigh);

  WireChamber1Hit = 0, WireChamber2Hit = 0 ;
  X_pos_WC1 = X_pos_WC2 = 0;
  Y_pos_WC1 = Y_pos_WC2 = 0;
  X_tim_WC1 = X_tim_WC2 = 0;
  Y_tim_WC1 = Y_tim_WC2 = 0;

  for(int j = 0; j < event->GetWCHits(); j++){
    Int_t X_channelAdjust = 0, XY_channelAdjust = 0;
    Int_t X_module         = event->GetWCChan(j).GetTDCNum(); 
    Int_t XY_channelNumber = event->GetWCChan(j).GetWire(); 
    Int_t XY_channelCount  = event->GetWCChan(j).GetCount(); 
    if(X_module%4 == 3 || X_module%4 == 0) continue; // only X wires

    Int_t ModuleNum1  = 0;
    Int_t ModuleNum2  = 0;
    Int_t ModuleNum5  = 0;
    Int_t ModuleNum6  = 0;
    if(X_module%4 == 1){
      XY_channelAdjust = (XY_channelNumber + 1);
      if(X_module == 1)  ModuleNum1  = X_module;
      if(X_module == 5)  ModuleNum5  = X_module;
    }
    else if(X_module%4 == 2){
      XY_channelAdjust = (XY_channelNumber + 65);
      if(X_module == 2)  ModuleNum2  = X_module;
      if(X_module == 6)  ModuleNum6  = X_module;
    }
    Int_t InTimeX1  = 0, InTimeX2  = 0;
    Int_t OutTimeX1 = 0, OutTimeX2 = 0;
    if(X_module%4 == 1 && XY_channelCount >  mean[X_module-1] + tdcRange) OutTimeX1++;
    if(X_module%4 == 1 && XY_channelCount <= mean[X_module-1] + tdcRange) InTimeX1++;
    if(X_module%4 == 2 && XY_channelCount >  mean[X_module-1] + tdcRange) OutTimeX2++; 
    if(X_module%4 == 2 && XY_channelCount <= mean[X_module-1] + tdcRange) InTimeX2++; 
    X_channelAdjust = XY_channelAdjust; 


    for(int k = 0; k < event->GetWCHits(); ++k){
      Int_t Y_channelAdjust   = 0, XY_channelAdjust2 = 0;
      Int_t Y_module          = event->GetWCChan(k).GetTDCNum(); // giving some bogus initial value
      Int_t XY_channelNumber2 = event->GetWCChan(k).GetWire(); // giving some bogus initial value
      Int_t XY_channelCount2  = event->GetWCChan(k).GetCount(); // giving some bogus initial value
      if(Y_module%4 == 1 || Y_module%4 == 2) continue; // only Y wires

      Int_t ModuleNum3  = 0, ModuleNum4 = 0, ModuleNum7 = 0, ModuleNum8 = 0;
      if(Y_module%4 == 3){
	XY_channelAdjust2 = (129 - XY_channelNumber2);
	if(Y_module == 3)  ModuleNum3  = Y_module;
	if(Y_module == 7)  ModuleNum7  = Y_module;
      }
      else if(Y_module%4 == 0){
	XY_channelAdjust2 = (65 - XY_channelNumber2); 
	if(Y_module == 4)  ModuleNum4  = Y_module;
	if(Y_module == 8)  ModuleNum8  = Y_module;
      }
      Int_t InTimeY1  = 0, InTimeY2  = 0;
      Int_t OutTimeY1 = 0, OutTimeY2 = 0;
      if(Y_module%4 == 3 && XY_channelCount2 >  mean[Y_module-1] + tdcRange) OutTimeY1++; 
      if(Y_module%4 == 3 && XY_channelCount2 <= mean[Y_module-1] + tdcRange) InTimeY1++; 
      if(Y_module%4 == 0 && XY_channelCount2 >  mean[Y_module-1] + tdcRange) OutTimeY2++;
      if(Y_module%4 == 0 && XY_channelCount2 <= mean[Y_module-1] + tdcRange) InTimeY2++; 
      Y_channelAdjust = XY_channelAdjust2;
      // Fill the scatter plots
      if(X_channelAdjust != 0 && Y_channelAdjust != 0){
	// WC 1 //
	if(((InTimeX1==1 && ModuleNum1==1) || (InTimeX2==1 && ModuleNum2==2)) && ((InTimeY1==1 && ModuleNum3==3) || (InTimeY2==1 && ModuleNum4==4))){
	  WireChamber1Hit++;
	  X_pos_WC1 = X_channelAdjust;
	  Y_pos_WC1 = Y_channelAdjust;
	  X_tim_WC1 = XY_channelCount;
	  Y_tim_WC1 = XY_channelCount2;
	  WC1->Fill((X_pos_WC1 - 64), (Y_pos_WC1 - 64), 1); 

	}
	if(((OutTimeX1==1 && ModuleNum1==1) || (OutTimeX2==1 && ModuleNum2==2)) && ((OutTimeY1==1 && ModuleNum3==3) || (OutTimeY2==1 && ModuleNum4==4)))WCO1->Fill(X_channelAdjust-64, Y_channelAdjust-64, 1); 
	// WC 2 //
	if(((InTimeX1==1 && ModuleNum5==5) || (InTimeX2==1 && ModuleNum6==6)) && ((InTimeY1==1 && ModuleNum7==7) || (InTimeY2==1 && ModuleNum8==8))){
	  WireChamber2Hit++;
	  X_pos_WC2 = X_channelAdjust;
	  Y_pos_WC2 = Y_channelAdjust;
	  X_tim_WC2 = XY_channelCount;
	  Y_tim_WC2 = XY_channelCount2;
	  WC2->Fill((X_pos_WC2 - 64), (Y_pos_WC2 - 64), 1);
	} 
	if(((OutTimeX1==1 && ModuleNum5==5) || (OutTimeX2==1 && ModuleNum6==6)) && ((OutTimeY1==1 && ModuleNum7==7) || (OutTimeY2==1 && ModuleNum8==8)))WCO2->Fill(X_channelAdjust-64, Y_channelAdjust-64, 1);
      }
    }
  }// X loop ends 


  bool ConfirmScinX = wcReco.ScintConfirm(X_pos_WC1, X_pos_WC2, dWC1toWC2);
  bool ConfirmScinY = wcReco.ScintConfirm(Y_pos_WC1, Y_pos_WC2, dWC1toWC2); 
  //if((CutX <= 50) && (CutX >= -50) && (CutY <= 50) && (CutY >= -50)){
  if(ConfirmScinX && ConfirmScinY){
    // If we have exactly one in-time hit in WC1 and WC2
    if(WireChamber1Hit == 1 && WireChamber2Hit == 1){
      // Beam hits in wire chamber 1
      WC1_Beam->Fill(X_pos_WC1 - 64, Y_pos_WC1 - 64, 1);
      WC2_Beam->Fill(X_pos_WC2 - 64, Y_pos_WC2 - 64, 1);
      // Extrapolating to the shashlik face
      Float_t ShashX = 
	wcReco.GetProjection(X_pos_WC1, X_pos_WC2, dWC1toWC2, (dWC1toWC2 + dWC2toShash));
      Float_t ShashY = 
	wcReco.GetProjection(Y_pos_WC1, Y_pos_WC2, dWC1toWC2, (dWC1toWC2 + dWC2toShash));
      WC_Shashlik->Fill(ShashX, ShashY, 1); 
    }// only one in-time hit in wirechamber 
    // If we have more than one in-time hits in WC1 and WC2
    if(WireChamber1Hit > 1 && WireChamber2Hit > 1){
    }// more than one in-time hits in wirechamber
  }// confirming hit in scintillator 1 and scintillator 2 



  hEmpty  = new TH2I("Empty","Empty",32,-64,64,32,-64,64); 
  if(hStack1!=0) delete hStack1;
  TString name1; name1.Form("Upstream WC - evt %d", util.eventNumber);
  TString name2; name2.Form("Downstream WC - evt %d", util.eventNumber);
  hStack1 = new THStack(name1,name1);
  hStack2 = new THStack(name2,name2);
  c1->cd(1); 
  gPad->SetFrameFillColor(0);
  if(util.WC_showOThits) hStack1->Add(WCO1);
  if(util.WC_showIThits) hStack1->Add(WC1);
  if(util.WC_showQhits)hStack1->Add(WC1_Beam);
  if(!(util.WC_showOThits||util.WC_showIThits||util.WC_showQhits)) 
    hStack1->Add(hEmpty);
  if(! util.stealthmode)
    {
    hStack1->Draw();
    SetHistoPreferences(hStack1);
    gPad->BuildLegend(0, .77, 0.5, 0.93);
    }


  c1->cd(2);
  if(util.WC_showOThits) hStack2->Add(WCO2);
  if(util.WC_showIThits) hStack2->Add(WC2);
  if(util.WC_showQhits) hStack2->Add(WC2_Beam);
  if(!(util.WC_showOThits||util.WC_showIThits||util.WC_showQhits))
    hStack2->Add(hEmpty);
  if(! util.stealthmode)
    {
      hStack2->Draw();
      SetHistoPreferences(hStack2);
      gPad->BuildLegend(0, .77, 0.5, 0.93);     
      c1->Update();
    }
}
