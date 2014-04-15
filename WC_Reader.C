// Created: 4/12/2014 J.Adams, A. Santra: Initial release
//                    B. Hirosky: allow input file name, add new branch names
//                                setup to compile, small bug fixes

// usage for now:
// root[0] gSystem->Load("TBEvent_cc.so")
// root[1] .L WC_Reader.C+
// root[2] WC_Reader("file.root")

#include <TH2.h>
#include <TStyle.h>
#include <TROOT.h>
#include <TFile.h>
#include <TSpectrum.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TBox.h>
#include <TTree.h>
#include <TSystem.h>
#include <TClassTable.h>
#include "TBEvent.h"
#include <vector>

Float_t GetProjection(Float_t pos1, Float_t pos2, 
		      Float_t WCdist, Float_t projDist){
  Float_t Delta = pos1 - pos2;
  Float_t projection = pos1 + (projDist*(Delta/WCdist)); 
  return (projection - 64);
}

void DrawShashBox(){
  TBox *AA = new TBox(-28,-28,-14,-14);
  TBox *BB = new TBox(-14,-28,  0,-14);
  TBox *CC = new TBox(0,  -28, 14,-14);
  TBox *DD = new TBox(14, -28, 28,-14);
  TBox *EE = new TBox(-28,-14,-14,  0);
  TBox *FF = new TBox(-14,-14,  0,  0);
  TBox *GG = new TBox(0,  -14, 14,  0);
  TBox *HH = new TBox(14, -14, 28,  0);
  TBox *II = new TBox(-28,  0,-14, 14);
  TBox *JJ = new TBox(-14,  0,  0, 14);
  TBox *KK = new TBox(0,    0, 14, 14);
  TBox *LL = new TBox(14,   0, 28, 14);
  TBox *MM = new TBox(-28, 14,-14, 28);
  TBox *NN = new TBox(-14, 14,  0, 28);
  TBox *OO = new TBox(0,   14, 14, 28);
  TBox *PP = new TBox(14,  14, 28, 28);
  TBox *RR = new TBox(-42,-42, 42, 42); 
  AA->SetFillStyle(0);
  BB->SetFillStyle(0);
  CC->SetFillStyle(0);
  DD->SetFillStyle(0);
  EE->SetFillStyle(0);
  FF->SetFillStyle(0);
  GG->SetFillStyle(0);
  HH->SetFillStyle(0);
  II->SetFillStyle(0);
  JJ->SetFillStyle(0);
  KK->SetFillStyle(0);
  LL->SetFillStyle(0);
  MM->SetFillStyle(0);
  NN->SetFillStyle(0);
  OO->SetFillStyle(0);
  PP->SetFillStyle(0);
  RR->SetFillStyle(0);
  AA->Draw("l"); 
  BB->Draw("l");
  CC->Draw("l");
  DD->Draw("l");
  EE->Draw("l"); 
  FF->Draw("l");
  GG->Draw("l");
  HH->Draw("l");
  II->Draw("l"); 
  JJ->Draw("l");
  KK->Draw("l");
  LL->Draw("l");
  MM->Draw("l"); 
  NN->Draw("l");
  OO->Draw("l");
  PP->Draw("l");
  RR->Draw("l");
}

void DrawScintBox(){
  TBox *SS = new TBox(-50,-50, 50, 50);
  SS->SetFillStyle(0);
  SS->Draw("l");
}

void WC_Reader(TString filename="outputNtuple.root"){
  
  /// Z - AXIS DISTANCES ///
  TSpectrum tspectrum;	                 // To find peaks
  int verbose              = 0;          // Not sure what this does...
  const float fitRange     = 13.0;       // Originally it was 8.0
  const float WC1toWC2     = 3476.625;   // Distance between WC1 and WC2         (mm)
  const float WC2toShash   = 533.4;      // Distance between WC2 and Shashlik    (mm)
  const float WC1toScin1   = 1231.9;     // Distance between WC1 and Scint 1     (mm)
  const float Scin1toScin2 = 4445.0;     // Distance between Scint 1 and Scint 2 (mm)

  if (!TClassTable::GetDict("TBEvent")) {
    gSystem->Load("TBEvent_cc.so");
  }

  /// Read in NTuple from TBNtupleMaker2.py output, and grab the TTree ///
  
  TFile *f = new TFile(filename);
  TTree* BeamData = (TTree*)f->Get("t1041");

  TBEvent *event = new TBEvent();

  TBranch *bevent = BeamData->GetBranch("tbevent");
  bevent->SetAddress(&event);
  
  TF1* fGaussian = new TF1("fGaussian","gaus",0,100);
  fGaussian->SetLineColor(2);
  
  /// Create new output file and all relevant histograms ///

  TFile *A = new TFile("WC_Plots.root","RECREATE");

  TH1I* TDC[16];
  char *histname = new char[10];
  char *histtitle= new char[20];
  for (Int_t c=0;c<16; c++) {
    sprintf(histname, "TDC%d",c+1);
    sprintf(histtitle,"Counts for TDC%d",c+1);
    TDC[c]=new TH1I(histname, histtitle, 300, 0.0, 300.0);
  }

  TH2I *WC1_Beam      = new TH2I("WC1_Beam",      "Beam Hits in WC1",            128, -64, 64, 128, -64, 64);
  TH2I *WC2_Beam      = new TH2I("WC2_Beam",      "Beam Hits in WC2",            128, -64, 64, 128, -64, 64);
  TH2F *WC_Shashlik   = new TH2F("WC_Shashlik",   "Beam Hits in Shashlik Face",  128, -64, 64, 128, -64, 64);
  TH2F *WC_Scin1      = new TH2F("WC_Scin1",      "Beam Hits in Scintillator 1", 128, -64, 64, 128, -64, 64);
  TH2F *WC_Scin2      = new TH2F("WC_Scin2",      "Beam Hits in Scintillator 2", 128, -64, 64, 128, -64, 64);
  TH1I *WC_TimDiff_XX = new TH1I("WC_TimDiff_XX", "XX Time Difference: WC2-WC1", 80,  -40, 40);
  TH1I *WC_TimDiff_YY = new TH1I("WC_TimDiff_YY", "YY Time Difference: WC2-WC1", 80,  -40, 40);
  TH1I *WC_TimDiff_XY = new TH1I("WC_TimDiff_XY", "XY Time Difference: WC2-WC1", 80,  -40, 40);
  TH1I *WC_TimDiff_YX = new TH1I("WC_TimDiff_YX", "YX Time Difference: WC2-WC1", 80,  -40, 40);

  TH2I* WC[4], *WCO[4], *WC_Timing[2];
  char *histnameWC     = new char[15];
  char *histnameWCO    = new char[15];
  char *histtiming     = new char[25];
  char *histtitleWC    = new char[40];
  char *histtitleWCO   = new char[40];
  char *histtimingtitle= new char[80];

  for (Int_t d=0;d<4; d++) {
    sprintf(histnameWC, "WC%d",d+1);
    sprintf(histtitleWC,"Plane Hits %d, In Time",d+1);
    WC[d]=new TH2I(histnameWC,histtitleWC,128,-64,64,128,-64,64);
    sprintf(histnameWCO, "WCO%d",d+1);
    sprintf(histtitleWCO,"Plane Hits %d, Out of Time",d+1);
    WCO[d]=new TH2I(histnameWCO,histtitleWCO,128,-64,64,128,-64,64);
  }
  for (Int_t d=0;d<2; d++){
    sprintf(histtiming, "WCtiming%d",d+1);
    sprintf(histtimingtitle,"TDC counts %d, For In Time Hits;X TDC counts;Y TDC counts",d+1);
    WC_Timing[d]=new TH2I(histtiming,histtimingtitle, 40,95,135,40,95,135);
  }

  /// TDC TIMING DISTRIBUTIONS ///
  TCanvas *C = new TCanvas("C","TDC Timing by Channel",1200,900);
  C->Divide(4,4);
  for (int i = 0; i < BeamData->GetEntries(); i++) {
    BeamData->GetEntry(i);
    for(int j = 0; j < event->GetWCHits(); j++){
      Int_t module        = event->GetWCChan(j).GetTDCNum();
      //Int_t channelNumber = event->GetWCChan(j).GetWire();
      Int_t channelCount  = event->GetWCChan(j).GetCount();   
      for(Int_t moduleNo=0;moduleNo<16;moduleNo++){
        if(module == moduleNo+1){
  	  TDC[moduleNo]->Fill(channelCount);
          C->cd(moduleNo+1);
  	  TDC[moduleNo]->Draw();
        }
      }
    }
  }

  /// FIT TIMING PEAKS TO GAUSSIAN ///
  int mean[16]={0};
  for(Int_t drawG=0; drawG<16; ++drawG){  
    // finding peak using tspectrum
    tspectrum.Search(TDC[drawG],2,"goff",0.08); 
    int npeaks = tspectrum.GetNPeaks();
    Float_t *tdc_peaks = tspectrum.GetPositionX();
    Float_t early_peak = 9999.;
    for (int ipeak = 0; ipeak < npeaks; ipeak++){
      //if ( verbose ) cout << ipeak << "\t" << tdc_peaks[ipeak] << endl;
      if ( tdc_peaks[ipeak] < early_peak ){ 
  	early_peak = tdc_peaks[ipeak];
      }
    }
    // Fitting the peak with a gaussian
    int maxBin    = TDC[drawG]->FindBin(early_peak);
    mean[drawG]   = TDC[drawG]->GetBinCenter(maxBin);
    int ampl      = TDC[drawG]->GetBinContent(maxBin);
    fGaussian->SetParameters(ampl, mean[drawG], 4.5);
    fGaussian->SetRange(0, mean[drawG]+fitRange);
    if ( verbose ) TDC[drawG]->Fit(fGaussian,"R");
    else           TDC[drawG]->Fit(fGaussian,"0+QR");
    const Int_t kNotDraw = 1<<9;
    TDC[drawG]->GetFunction("fGaussian")->ResetBit(kNotDraw);
  }
  
  /// X-Y PLANE PLOTS ///
  for (int i = 0; i < BeamData->GetEntries(); i++) {
    BeamData->GetEntry(i);
    Int_t WireChamber1Hit = 0, WireChamber2Hit = 0 ;
    Int_t X_pos_WC1 = 0, X_pos_WC2 = 0;
    Int_t Y_pos_WC1 = 0, Y_pos_WC2 = 0;
    Int_t X_tim_WC1 = 0, X_tim_WC2 = 0;
    Int_t Y_tim_WC1 = 0, Y_tim_WC2 = 0;
    // Loop over X wires
    for(int j = 0; j < event->GetWCHits(); j++){
      Int_t X_channelAdjust = 0, XY_channelAdjust = 0;
      Int_t X_module         = event->GetWCChan(j).GetTDCNum(); 
      Int_t XY_channelNumber = event->GetWCChan(j).GetWire();
      Int_t XY_channelCount  = event->GetWCChan(j).GetCount(); 
      if(X_module%4 == 3 || X_module%4 == 0) continue;
      Int_t ModuleNum1 = 0, ModuleNum2  = 0, ModuleNum5  = 0, ModuleNum6  = 0;
      Int_t ModuleNum9 = 0, ModuleNum10 = 0, ModuleNum13 = 0, ModuleNum14 = 0;
      if(X_module%4 == 1){
        XY_channelAdjust = (XY_channelNumber + 1);
        if(X_module == 1)  ModuleNum1  = X_module;
        if(X_module == 5)  ModuleNum5  = X_module;
        if(X_module == 9)  ModuleNum9  = X_module;
        if(X_module == 13) ModuleNum13 = X_module;
      }
      else if(X_module%4 == 2){
        XY_channelAdjust = (XY_channelNumber + 65);
        if(X_module == 2)  ModuleNum2  = X_module;
        if(X_module == 6)  ModuleNum6  = X_module;
        if(X_module == 10) ModuleNum10 = X_module;
        if(X_module == 14) ModuleNum14 = X_module;
      }
      Int_t InTimeX1  = 0, InTimeX2  = 0;
      Int_t OutTimeX1 = 0, OutTimeX2 = 0;
      if(X_module%4 == 1 && XY_channelCount >  mean[X_module-1] + fitRange) OutTimeX1++;
      if(X_module%4 == 1 && XY_channelCount <= mean[X_module-1] + fitRange) InTimeX1++;
      if(X_module%4 == 2 && XY_channelCount >  mean[X_module-1] + fitRange) OutTimeX2++; 
      if(X_module%4 == 2 && XY_channelCount <= mean[X_module-1] + fitRange) InTimeX2++; 
      X_channelAdjust = XY_channelAdjust; 
      // Loop over Y wires
      for(int k = 0; k < event->GetWCHits(); ++k){
        Int_t Y_channelAdjust   = 0;
        Int_t XY_channelAdjust2 = 0;
        Int_t Y_module          = event->GetWCChan(k).GetTDCNum(); 
        Int_t XY_channelNumber2 = event->GetWCChan(k).GetWire();
        Int_t XY_channelCount2  = event->GetWCChan(k).GetCount(); 
        if(Y_module%4 == 1 || Y_module%4 == 2) continue;
        Int_t ModuleNum3  = 0;
        Int_t ModuleNum4  = 0;  
        Int_t ModuleNum7  = 0;
        Int_t ModuleNum8  = 0;
        Int_t ModuleNum11 = 0;
        Int_t ModuleNum12 = 0;
        Int_t ModuleNum15 = 0;
        Int_t ModuleNum16 = 0;
        if(Y_module%4 == 3){
          XY_channelAdjust2 = (129 - XY_channelNumber2);
          if(Y_module == 3)  ModuleNum3  = Y_module;
          if(Y_module == 7)  ModuleNum7  = Y_module;
          if(Y_module == 11) ModuleNum11 = Y_module;
          if(Y_module == 15) ModuleNum15 = Y_module;
        }
        else if(Y_module%4 == 0){
          XY_channelAdjust2 = (65 - XY_channelNumber2); 
          if(Y_module == 4)  ModuleNum4  = Y_module;
          if(Y_module == 8)  ModuleNum8  = Y_module;
          if(Y_module == 12) ModuleNum12 = Y_module;
          if(Y_module == 16) ModuleNum16 = Y_module;
        }
        Int_t InTimeY1  = 0;
        Int_t InTimeY2  = 0;
        Int_t OutTimeY1 = 0;
        Int_t OutTimeY2 = 0;
        if(Y_module%4 == 3 && XY_channelCount2 >  mean[Y_module-1] + fitRange) OutTimeY1++; 
        if(Y_module%4 == 3 && XY_channelCount2 <= mean[Y_module-1] + fitRange) InTimeY1++; 
        if(Y_module%4 == 0 && XY_channelCount2 >  mean[Y_module-1] + fitRange) OutTimeY2++;
        if(Y_module%4 == 0 && XY_channelCount2 <= mean[Y_module-1] + fitRange) InTimeY2++; 
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
            WC[0]->Fill((X_channelAdjust - 64), (Y_channelAdjust - 64), 1); 
          }
          if(((OutTimeX1==1 && ModuleNum1==1) || (OutTimeX2==1 && ModuleNum2==2)) && ((OutTimeY1==1 && ModuleNum3==3) || (OutTimeY2==1 && ModuleNum4==4)))WCO[0]->Fill(X_channelAdjust-64, Y_channelAdjust-64, 1); 
          // WC 2 //
          if(((InTimeX1==1 && ModuleNum5==5) || (InTimeX2==1 && ModuleNum6==6)) && ((InTimeY1==1 && ModuleNum7==7) || (InTimeY2==1 && ModuleNum8==8))){
            WireChamber2Hit++;
            X_pos_WC2 = X_channelAdjust;
            Y_pos_WC2 = Y_channelAdjust;
            X_tim_WC2 = XY_channelCount;
            Y_tim_WC2 = XY_channelCount2;
	    WC[1]->Fill((X_channelAdjust - 64), (Y_channelAdjust - 64), 1);
          } 
          if(((OutTimeX1==1 && ModuleNum5==5) || (OutTimeX2==1 && ModuleNum6==6)) && ((OutTimeY1==1 && ModuleNum7==7) || (OutTimeY2==1 && ModuleNum8==8)))WCO[1]->Fill(X_channelAdjust-64, Y_channelAdjust-64, 1);
          // WC 3 //
          if(((InTimeX1==1 && ModuleNum9==9) || (InTimeX2==1 && ModuleNum10==10)) && ((InTimeY1==1 && ModuleNum11==11) || (InTimeY2==1 && ModuleNum12==12)))WC[2]->Fill(X_channelAdjust-64, Y_channelAdjust-64, 1); 
          if(((OutTimeX1==1 && ModuleNum9==9) || (OutTimeX2==1 && ModuleNum10==10)) && ((OutTimeY1==1 && ModuleNum11==11) || (OutTimeY2==1 && ModuleNum12==12)))WCO[2]->Fill(X_channelAdjust-64, Y_channelAdjust-64, 1);
          // WC 4 //
          if(((InTimeX1==1 && ModuleNum13==13) || (InTimeX2==1 && ModuleNum14==14)) && ((InTimeY1==1 && ModuleNum15==15) || (InTimeY2==1 && ModuleNum16==16)))WC[3]->Fill(X_channelAdjust-64, Y_channelAdjust-64, 1); 
          if(((OutTimeX1==1 && ModuleNum13==13) || (OutTimeX2==1 && ModuleNum14==14)) && ((OutTimeY1==1 && ModuleNum15==15) || (OutTimeY2==1 && ModuleNum16==16)))WCO[3]->Fill(X_channelAdjust-64, Y_channelAdjust-64, 1);
        }
      } // Y loop ends
    }// X loop ends 

    if(i%1000==0)std::cout << "Beam particles processed: " << i << std:: endl;
    Float_t CutX = GetProjection(X_pos_WC1, X_pos_WC2, WC1toWC2, -(WC1toScin1 + Scin1toScin2));
    Float_t CutY = GetProjection(Y_pos_WC1, Y_pos_WC2, WC1toWC2, -(WC1toScin1 + Scin1toScin2));
    if((CutX <= 50) && (CutX >= -50) && (CutY <= 50) && (CutY >= -50)){
      
      // If we have exactly one hit in WC1 and WC2
      if(WireChamber1Hit == 1 && WireChamber2Hit == 1){
	// Timing plots
	WC_Timing[0]->Fill(X_tim_WC1, Y_tim_WC1);
	WC_Timing[1]->Fill(X_tim_WC2, Y_tim_WC2);
	// Timing difference plots
	WC_TimDiff_XX->Fill(X_tim_WC2 - X_tim_WC1);
	WC_TimDiff_YY->Fill(Y_tim_WC2 - Y_tim_WC1);
	WC_TimDiff_XY->Fill(X_tim_WC2 - Y_tim_WC1);
	WC_TimDiff_YX->Fill(Y_tim_WC2 - X_tim_WC1);
	// Beam hits in wire chamber 1
	WC1_Beam->Fill(X_pos_WC1 - 64, Y_pos_WC1 - 64, 1);
	WC2_Beam->Fill(X_pos_WC2 - 64, Y_pos_WC2 - 64, 1);
	// Extrapolating to the shashlik face
	Float_t ShashX = GetProjection(X_pos_WC1, X_pos_WC2, WC1toWC2, (WC1toWC2 + WC2toShash));
	Float_t ShashY = GetProjection(Y_pos_WC1, Y_pos_WC2, WC1toWC2, (WC1toWC2 + WC2toShash));
	WC_Shashlik->Fill(ShashX, ShashY, 1); 
	// Backtracking to Scintillator 1 and Scintillator 2
	Float_t Scint1X = GetProjection(X_pos_WC1, X_pos_WC2, WC1toWC2, -WC1toScin1);
	Float_t Scint1Y = GetProjection(Y_pos_WC1, Y_pos_WC2, WC1toWC2, -WC1toScin1);
	WC_Scin1->Fill(Scint1X, Scint1Y, 1);
	Float_t Scint2X = GetProjection(X_pos_WC1, X_pos_WC2, WC1toWC2, -(WC1toScin1 + Scin1toScin2));
	Float_t Scint2Y = GetProjection(Y_pos_WC1, Y_pos_WC2, WC1toWC2, -(WC1toScin1 + Scin1toScin2));
	WC_Scin2->Fill(Scint2X, Scint2Y, 1);
      } 
    }
  }// Event loop ends
  
  C->SaveAs("TDC_timing.gif");

  /// DRAW ALL EVENT DISPLAY PLOTS ///
  // Scatter plot for only one hit in WC1 and WC2
  TCanvas *Q = new TCanvas("Q","WC Scatter Plot for WC1 and WC2",1200,900);
  Q->Divide(2,1);
  Q->cd(1);
  WC1_Beam->SetMarkerColor(4);
  WC1_Beam->Draw();
  Q->cd(2);
  WC2_Beam->SetMarkerColor(4);
  WC2_Beam->Draw();

  Q->SaveAs("wc_scatterPlot.gif");

  // Timing plots
  TCanvas *R = new TCanvas("R","WC timing counts for WC1 and WC2",1200,900);
  R->Divide(2,1);
  for(int k=0;k<2;++k){
    R->cd(k+1);
    WC_Timing[k]->Draw("COLZ");
  }

  R->SaveAs("wc_timingCounts.gif");

  // Superposition of In Time and Out of Time
  TCanvas *J = new TCanvas("J","WC Scatter Plot - Superposition",1200,900);
  J->Divide(2,2);
  TCanvas *D = new TCanvas("D","WC Scatter Plot In Time",1200,900);
  D->Divide(2,2);
  TCanvas *G = new TCanvas("G","WC Scatter Plot Out Of Time",1200,900);
  G->Divide(2,2);
  for(int j=0;j<4;++j){
    D->cd(j+1);
    WC[j]->SetMarkerColor(4);
    WC[j]->Draw(); 
    G->cd(j+1);
    WCO[j]->SetMarkerColor(2);
    WCO[j]->Draw(); 
    J->cd(j+1); 
    WCO[j]->Draw();
    WC[j]->Draw("same");
  }

  J->SaveAs("wc_scatterPlot_superposition.gif");
  D->SaveAs("wc_scatterPlot_inTime.gif");
  G->SaveAs("wc_scatterPlot_outOfTime.gif");

  // Timing difference plots
  TCanvas *T = new TCanvas("T","WC timing difference plots",1200,900);
  T->Divide(2,2);
  T->cd(1);
  WC_TimDiff_XX->SetLineColor(4);
  WC_TimDiff_XX->Draw();
  T->cd(2);
  WC_TimDiff_YY->SetLineColor(4);
  WC_TimDiff_YY->Draw();
  T->cd(3);
  WC_TimDiff_XY->SetLineColor(4);
  WC_TimDiff_XY->Draw();
  T->cd(4);
  WC_TimDiff_YX->SetLineColor(4);
  WC_TimDiff_YX->Draw();

  T->SaveAs("wc_timingDifference.gif");

  // Shashlik face and Scinitllator plots
  TCanvas *W = new TCanvas("W","Beam Tracks",1200,900);
  W->Divide(2,2);
  W->cd(4);
  gStyle->SetOptStat(""); 
  WC_Shashlik->SetMarkerColor(4);
  WC_Shashlik->Draw("COLZ");
  DrawShashBox(); 
  W->cd(2);
  gStyle->SetOptStat("");
  WC_Scin1->SetMarkerColor(4);
  WC_Scin1->Draw("COLZ");
  DrawScintBox();
  W->cd(1);
  gStyle->SetOptStat("");
  WC_Scin2->SetMarkerColor(4);
  WC_Scin2->Draw("COLZ");
  DrawScintBox();
  W->cd(3);
  gStyle->SetOptStat("");
  WC1_Beam->SetMarkerColor(4);
  WC1_Beam->Draw("COLZ");

  W->SaveAs("beamTracks.gif");

  f->Close(); 
  A->cd();
  A->Write(); 
}
