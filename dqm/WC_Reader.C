//////          Created: 4/12/2014 J.Adams, A. Santra: Initial release
//////                    B. Hirosky: allow input file name, add new branch names
//////                                setup to compile, small bug 
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
#include "../include/TBEvent.h"
#include "../include/TBReco.h"
#include "../include/WC.h"
using std::cout;
using std::endl;


// Float_t GetProjection(Float_t pos1, Float_t pos2, 
// 		      Float_t WCdist, Float_t projDist){
//   Float_t Delta = pos1 - pos2;
//   Float_t projection = pos1 + (projDist*(Delta/WCdist)); 
//   return (projection - 64);
// }
// 
// bool ScinConfirm(Float_t XPos1, Float_t XPos2, Float_t WCDist){
//   const float ProjDist = -(1231.9+4445.0);
//   Float_t CheckProjection = GetProjection(XPos1, XPos2, WCDist, ProjDist); 
//   if(fabs(CheckProjection)<=50)return true;
//   else return false;
// }

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



void WC_Reader(TString filename="outputNtuple.root"){
  /// root filename needs to be changed when not used with python wrapper code ///
  /// Read in NTuple from TBNtupleMaker2.py output, and grab the TTree ///
  
  TFile *f = new TFile(filename);
  TTree* t1041 = (TTree*)f->Get("t1041");

  TBEvent *event = new TBEvent();
  TBranch *bevent = t1041->GetBranch("tbevent");
  
  
  /// Create new output file and all relevant histograms ///
  TFile *A = new TFile("WC_Plots.root","RECREATE");

  ////////// Analyze TDC timing peaks ////////////////////////
  WCReco wcReco;
  TH1I* TDC[NTDC];
  int mean[NTDC]={0}, tLow[NTDC]={0}, tHigh[NTDC]={0};
  wcReco.AddTree(t1041);
  wcReco.GetTDCcuts(mean, tLow, tHigh); 
  wcReco.GetTDChists(TDC);  // fetch histograms
  ////////////////////////////////////////////////////////////


  TH2I *WC1_Beam      = new TH2I("WC1_Beam",      "Beam Hits in WC1",            128, -64, 64, 128, -64, 64);
  TH2I *WC2_Beam      = new TH2I("WC2_Beam",      "Beam Hits in WC2",            128, -64, 64, 128, -64, 64);
  TH2F *WC_Shashlik   = new TH2F("WC_Shashlik",   "Beam Hits in Shashlik Face",  128, -64, 64, 128, -64, 64);
  TH2F *WC_Scin1      = new TH2F("WC_Scin1",      "Beam Hits in Scintillator 1", 128, -64, 64, 128, -64, 64);
  TH2F *WC_Scin2      = new TH2F("WC_Scin2",      "Beam Hits in Scintillator 2", 128, -64, 64, 128, -64, 64);
  TH1I *WC_TimDiff_XX = new TH1I("WC_TimDiff_XX", "XX Time Difference: WC2-WC1", 80,  -40, 40);
  TH1I *WC_TimDiff_YY = new TH1I("WC_TimDiff_YY", "YY Time Difference: WC2-WC1", 80,  -40, 40);
  TH1I *WC_TimDiff_XY = new TH1I("WC_TimDiff_XY", "XY Time Difference: WC2-WC1", 80,  -40, 40);
  TH1I *WC_TimDiff_YX = new TH1I("WC_TimDiff_YX", "YX Time Difference: WC2-WC1", 80,  -40, 40);

  TH1F *slopeY = new TH1F("slopeY","slopeY",100,-0.005,0.005);

  TH2I *WC[4], *WCO[4], *WC_Timing[2];
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
    WC_Timing[d]=new TH2I(histtiming,histtimingtitle, 60,30,90,60,30,90);
  }

  bevent->SetAddress(&event);
  /// PLOT TDC TIMING DISTRIBUTIONS ///
  TCanvas *C = new TCanvas("C","TDC Timing by Channel",1200,900);
  C->Divide(4,4);
  for(Int_t moduleNo=0;moduleNo<NTDC;moduleNo++){
      C->cd(moduleNo+1);
      TDC[moduleNo]->Draw();
  }

  
  /// X-Y PLANE PLOTS ///
  for (int i = 0; i < t1041->GetEntries(); i++) {
    t1041->GetEntry(i);
    Int_t WireChamber1Hit = 0, WireChamber2Hit = 0 ;
    Int_t X_pos_WC1 = 0, X_pos_WC2 = 0;
    Int_t Y_pos_WC1 = 0, Y_pos_WC2 = 0;
    Int_t X_tim_WC1 = 0, X_tim_WC2 = 0;
    Int_t Y_tim_WC1 = 0, Y_tim_WC2 = 0;
    //+std::cout << "WCHits: " << event->GetWCHits() << std::endl;
    //Int_t AdjacentHitXFlag[600] = {0}; // giving some bogus initial value
    // Loop over X wires
    for(int j = 0; j < event->GetWCHits(); j++){
      Int_t X_channelAdjust = 0, XY_channelAdjust = 0;
      Int_t X_module         = event->GetWCChan(j).GetTDCNum(); 
      Int_t XY_channelNumber = event->GetWCChan(j).GetWire(); 
      Int_t XY_channelCount  = event->GetWCChan(j).GetCount(); 
      if(X_module%4 == 3 || X_module%4 == 0) continue; // only X wires

//       if(((j+1) < event->GetWCHits()) && (AdjacentHitXFlag[j] >= 0) && (((event->GetWCChan(j).GetTDCNum() == event->GetWCChan(j+1).GetTDCNum()) && (event->GetWCChan(j+1).GetWire() - event->GetWCChan(j).GetWire()==1) )||((event->GetWCChan(j+1).GetTDCNum()-event->GetWCChan(j).GetTDCNum()==1) && (event->GetWCChan(j).GetWire() - event->GetWCChan(j+1).GetWire()==63)))){
//         //&& (fabs(event->GetWCChan(j).GetCount() - event->GetWCChan(j+1).GetCount())==5)
//         // && (fabs(event->GetWCChan(j).GetCount() - event->GetWCChan(j+1).GetCount())==5)
//         AdjacentHitXFlag[j]   = 1;
//         AdjacentHitXFlag[j+1] = -1;
//       }
//       else if(((j+1) < event->GetWCHits()) && (AdjacentHitXFlag[j] >= 0)){
//         AdjacentHitXFlag[j]   = 1;
//         AdjacentHitXFlag[j+1] = 0;
//       }

//       if(AdjacentHitXFlag[j] > 0){
//         X_module         = event->GetWCChan(j).GetTDCNum(); 
//         XY_channelNumber = event->GetWCChan(j).GetWire();
//         XY_channelCount  = event->GetWCChan(j).GetCount(); 
//       }

      
      
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
      if(X_module%4 == 1 && XY_channelCount >  mean[X_module-1] + tdcRange) OutTimeX1++;
      if(X_module%4 == 1 && XY_channelCount <= mean[X_module-1] + tdcRange) InTimeX1++;
      if(X_module%4 == 2 && XY_channelCount >  mean[X_module-1] + tdcRange) OutTimeX2++; 
      if(X_module%4 == 2 && XY_channelCount <= mean[X_module-1] + tdcRange) InTimeX2++; 
      X_channelAdjust = XY_channelAdjust; 
      //Int_t AdjacentHitYFlag[600] = {0}; // giving some bogus initial value
        // Loop over Y wires
      for(int k = 0; k < event->GetWCHits(); ++k){
        Int_t Y_channelAdjust   = 0, XY_channelAdjust2 = 0;
        Int_t Y_module          = event->GetWCChan(k).GetTDCNum(); // giving some bogus initial value
        Int_t XY_channelNumber2 = event->GetWCChan(k).GetWire(); // giving some bogus initial value
        Int_t XY_channelCount2  = event->GetWCChan(k).GetCount(); // giving some bogus initial value
        if(Y_module%4 == 1 || Y_module%4 == 2) continue; // only Y wires

//           if(((k+1) < event->GetWCHits()) && (AdjacentHitYFlag[k] >= 0) && (((event->GetWCChan(k+1).GetTDCNum() == event->GetWCChan(k).GetTDCNum()) && (event->GetWCChan(k+1).GetWire() - event->GetWCChan(k).GetWire()==1)  )||((event->GetWCChan(k+1).GetTDCNum()-event->GetWCChan(k).GetTDCNum()==1) && (event->GetWCChan(k).GetWire() - event->GetWCChan(k+1).GetWire()==63) ))){
//             //&& (fabs(event->GetWCChan(k).GetCount() - event->GetWCChan(k+1).GetCount())==5)
//             //&& (fabs(event->GetWCChan(k).GetCount() - event->GetWCChan(k+1).GetCount())==5)
//             //std::cout << "No Problem" << k << std::endl;
//             AdjacentHitYFlag[k]   = 1;
//             AdjacentHitYFlag[k+1] = -1;
//           }
//           else if(((k+1) < event->GetWCHits()) && (AdjacentHitYFlag[k] >= 0)){
//             AdjacentHitYFlag[k]   = 1;
//             AdjacentHitYFlag[k+1] = 0;
//           }
// 
//           if(AdjacentHitYFlag[k] > 0){
//             Y_module          = event->GetWCChan(k).GetTDCNum(); 
//             XY_channelNumber2 = event->GetWCChan(k).GetWire();
//             XY_channelCount2  = event->GetWCChan(k).GetCount(); 
//           }

        Int_t ModuleNum3  = 0, ModuleNum4 = 0, ModuleNum7 = 0, ModuleNum8 = 0;
        Int_t ModuleNum11 = 0, ModuleNum12 = 0, ModuleNum15 = 0, ModuleNum16 = 0;
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
    //Float_t CutX = wcReco.GetProjection(X_pos_WC1, X_pos_WC2, dWC1toWC2, -(dWC1toScin1 + dScin1toScin2));// why was dWC1toScin1 negative to begin with?
    //Float_t CutY = wcReco.GetProjection(Y_pos_WC1, Y_pos_WC2, dWC1toWC2, -(dWC1toScin1 + dScin1toScin2));

    bool ConfirmScinX = wcReco.ScintConfirm(X_pos_WC1, X_pos_WC2, dWC1toWC2);
    bool ConfirmScinY = wcReco.ScintConfirm(Y_pos_WC1, Y_pos_WC2, dWC1toWC2); 
    //if((CutX <= 50) && (CutX >= -50) && (CutY <= 50) && (CutY >= -50)){
    if(ConfirmScinX && ConfirmScinY){
      // If we have exactly one in-time hit in WC1 and WC2
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
	Float_t ShashX = wcReco.GetProjection(X_pos_WC1, X_pos_WC2, dWC1toWC2, (dWC1toWC2 + dWC2toShash));
	Float_t ShashY = wcReco.GetProjection(Y_pos_WC1, Y_pos_WC2, dWC1toWC2, (dWC1toWC2 + dWC2toShash));
	slopeY->Fill((Y_pos_WC1-Y_pos_WC2)/dWC1toWC2);
	WC_Shashlik->Fill(ShashX, ShashY, 1); 
	// Backtracking to Scintillator 1 and Scintillator 2
	Float_t Scint1X = wcReco.GetProjection(X_pos_WC1, X_pos_WC2, dWC1toWC2, dWC1toScin1);
	Float_t Scint1Y = wcReco.GetProjection(Y_pos_WC1, Y_pos_WC2, dWC1toWC2, dWC1toScin1);
	WC_Scin1->Fill(Scint1X, Scint1Y, 1);
	Float_t Scint2X = wcReco.GetProjection(X_pos_WC1, X_pos_WC2, dWC1toWC2, -(dWC1toScin1 + dScin1toScin2));
	Float_t Scint2Y = wcReco.GetProjection(Y_pos_WC1, Y_pos_WC2, dWC1toWC2, -(dWC1toScin1 + dScin1toScin2));
	WC_Scin2->Fill(Scint2X, Scint2Y, 1);
      }// only one in-time hit in wirechamber 

      // If we have more than one in-time hits in WC1 and WC2
      if(WireChamber1Hit > 1 && WireChamber2Hit > 1){
        

      }// more than one in-time hits in wirechamber
    }// confirming hit in scintillator 1 and scintillator 2 
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
