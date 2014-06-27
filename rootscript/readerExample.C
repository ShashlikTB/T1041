// ftp://root.cern.ch/root/doc/ROOTUsersGuideHTML/ch12s17.html
// Created 4/12/2014 B.Hirosky: Initial release

// Note:  This code give examples for using many of the featues if the T1041 analysis
// package.  These are not at 'production level' and may represent coarse approximations
// or simplistic calculations.  Please do not assume all data returned is applicable for
// a precise analysis, until you have verified and (ideally) improved the supplied library
// code

#include <TString.h>
#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <TString.h>
#include <iostream>
#include <vector>
#include <TH1I.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TMath.h>
#include "../include/TBReco.h"
#include "../include/WC.h"
#include "../include/TBEvent.h"
#include "../include/calConstants.h"
#include "../include/Mapper.h"
#include <TCanvas.h>
#include <TBox.h>
#include <TStyle.h>

using std::cout;
using std::endl;
using std::vector;
using TMath::Min;

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

void readerExample(TString file="latest.root"){

  TFile *f = new TFile(file);
  TTree *t1041 = (TTree*) f->Get("t1041");
 
  TString outname=file;
  outname.ReplaceAll(".root","_reco.root");
  TFile *tfo=new TFile(outname,"recreate");

  //// Use WCReco to find definition of in-time hits ////
  WCReco wcReco;
  wcReco.AddTree(t1041);
  Int_t tMean[NTDC], tLow[NTDC], tHigh[NTDC];
  wcReco.GetTDCcuts(tMean, tLow, tHigh); 
  TH1I* hTDC[NTDC];
  wcReco.GetTDChists(hTDC);    // use to fetch the TDC histograms 
  /////////////////////////////////////////

  // slopes and x-y postion of track at face of detector
  TH1F *hslopeX              = new TH1F("hslopeX","Beam slope in X when 1 hit in WC1 and WC2",50,-0.005,0.005);
  TH1F *hslopeY              = new TH1F("hslopeY","Beam slope in Y when 1 hit in WC1 and WC2",50,-0.005,0.005);
  TH1F *hslopeX_1hitWC1      = new TH1F("hslopeX_1hitWC1","Beam slope in X when 1 hit in WC1",50,-0.005,0.005);
  TH1F *hslopeY_1hitWC1      = new TH1F("hslopeY_1hitWC1","Beam slope in Y when 1 hit in WC1",50,-0.005,0.005);
  TH1F *hslopeX_1hitWC2      = new TH1F("hslopeX_1hitWC2","Beam slope in X when 1 hit in WC2",50,-0.005,0.005);
  TH1F *hslopeY_1hitWC2      = new TH1F("hslopeY_1hitWC2","Beam slope in Y when 1 hit in WC2",50,-0.005,0.005);
  TH1F *hslopeX_MoreThan1Hit = new TH1F("hslopeX_MoreThan1Hit","Beam slope in X when at least more than 1 hit in WC",50,-0.005,0.005);
  TH1F *hslopeY_MoreThan1Hit = new TH1F("hslopeY_MoreThan1Hit","Beam slope in Y when at least more than 1 hit in WC",50,-0.005,0.005);

  TH1F *htrackX=new TH1F("hTrackX","Track X",56,-28,28);  // 1mm bins
  TH1F *htrackY=new TH1F("hTrackY","Track Y",56,-28,28);

  // TDC timing difference plots
  TH1F *hDeltaXYWC1              = new TH1F("hDeltaXYWC1","X-Y TDC timing difference at WC1 when 1 hit in WC1 and WC2",100,-50,50);
  TH1F *hDeltaXYWC2              = new TH1F("hDeltaXYWC2","X-Y TDC timing difference at WC2 when 1 hit in WC1 and WC2",100,-50,50);
  TH1F *hDeltaXYWC1_1hitWC1      = new TH1F("hDeltaXYWC1_1hitWC1","X-Y TDC timing difference at WC1 when 1 hit in WC1",100,-50,50);
  TH1F *hDeltaXYWC1_1hitWC2      = new TH1F("hDeltaXYWC1_1hitWC2","X-Y TDC timing difference at WC1 when 1 hit in WC2",100,-50,50);
  TH1F *hDeltaXYWC2_1hitWC1      = new TH1F("hDeltaXYWC2_1hitWC1","X-Y TDC timing difference at WC2 when 1 hit in WC1",100,-50,50);
  TH1F *hDeltaXYWC2_1hitWC2      = new TH1F("hDeltaXYWC2_1hitWC2","X-Y TDC timing difference at WC2 when 1 hit in WC2",100,-50,50);
  TH1F *hDeltaXYWC1_MoreThan1Hit = new TH1F("hDeltaXYWC1_MoreThan1Hit","X-Y TDC timing difference at WC1 when at least more than 1 hit in WC",200,-100,100);
  TH1F *hDeltaXYWC2_MoreThan1Hit = new TH1F("hDeltaXYWC2_MoreThan1Hit","X-Y TDC timing difference at WC2 when at least more than 1 hit in WC",200,-100,100);
  
  // only one hit //
  TH2F *Scint1   = new TH2F("Scint1",  "Scintillator 1 Hits", 128, -64, 64, 128, -64, 64);
  TH2F *Scint2   = new TH2F("Scint2",  "Scintillator 2 Hits", 128, -64, 64, 128, -64, 64);
  TH2F *WC1      = new TH2F("WC1",     "WC 1 Hits",           128, -64, 64, 128, -64, 64);
  TH2F *WC2      = new TH2F("WC2",     "WC 2 Hits",           128, -64, 64, 128, -64, 64);
  TH2F *Shashlik = new TH2F("Shashlik","Shashlik Hits",       128, -64, 64, 128, -64, 64);
  //  Multiple hit //
  TH2F *MultiScint1       = new TH2F("MultiScint1",      "Scintillator 1 Multiple Hits", 128, -64, 64, 128, -64, 64);
  TH2F *MultiScint2       = new TH2F("MultiScint2",      "Scintillator 2 Multiple Hits", 128, -64, 64, 128, -64, 64);
  TH2F *MultiWC1          = new TH2F("MultiWC1",         "WC 1 Multiple Hits",           128, -64, 64, 128, -64, 64);
  TH2F *MultiWC2          = new TH2F("MultiWC2",         "WC 2 Multiple Hits",           128, -64, 64, 128, -64, 64);
  TH2F *MultiWC1_1HitWC1  = new TH2F("MultiWC1_1HitWC1", "WC 1 Hits when 1 hit in WC1 and multiple hits in WC2",128, -64, 64, 128, -64, 64);
  TH2F *MultiWC2_1HitWC1Not      = new TH2F("MultiWC2_1HitWC1Not", "WC 2 Hits when 1 hit in WC1 and multiple hits in WC2",128, -64, 64, 128, -64, 64);
  TH2F *MultiWC2_1HitWC1         = new TH2F("MultiWC2_1HitWC1", "WC 2 Hits when 1 hit in WC1 and multiple hits in WC2, adjacent hits removed",128, -64, 64, 128, -64, 64);
  TH2F *MultiWC2_1HitWC1_AdjRem  = new TH2F("MultiWC2_1HitWC1_AdjRem", "WC 2 Hits when 1 hit in WC1 and multiple hits in WC2, Adjacent Hit removed, taken only the minimum slope",128, -64, 64, 128, -64, 64);
  TH2F *MultiWC1_1HitWC2Not      = new TH2F("MultiWC1_1HitWC2Not", "WC 1 Hits when 1 hit in WC2 and multiple hits in WC1",128, -64, 64, 128, -64, 64);
  TH2F *MultiWC1_1HitWC2         = new TH2F("MultiWC1_1HitWC2", "WC 1 Hits when 1 hit in WC2 and multiple hits in WC1, adjacent hits removed",128, -64, 64, 128, -64, 64);
  TH2F *MultiWC1_1HitWC2_AdjRem  = new TH2F("MultiWC1_1HitWC2_AdjRem", "WC 1 Hits when 1 hit in WC2 and multiple hits in WC1, Adjacent Hit removed, taken only the minimum slope",128, -64, 64, 128, -64, 64);
  TH2F *MultiWC2_1HitWC2         = new TH2F("MultiWC2_1HitWC2", "WC 2 Hits when 1 hit in WC2 and multiple hits in WC1",128, -64, 64, 128, -64, 64);
  TH2F *MultiShashlik            = new TH2F("MultiShashlik","Shashlik Multiple Hits",       128, -64, 64, 128, -64, 64);

  // plot location of tracks projected onto face of the detector
  // TBD

  // histograms for cluster x,y positions, sigmas
  TH1F *hClusterX=new TH1F("hClusterX","Cluster X",56,-28,28);  // 1mm bins
  TH1F *hClusterY=new TH1F("hClusterY","Cluster Y",56,-28,28);
  TH1F *hClusterSX=new TH1F("hClusterSX","Cluster SX",112,0,28);
  TH1F *hClusterSY=new TH1F("hClusterSY","Cluster SY",112,0,28);
  TH2F *hClusterSXY=new TH2F("hClusterSXY","Cluster SXY",28,0,28,28,0,28);
  // and cluster center vs extrapolated track
  TH1F *hClusterDX=new TH1F("hClusterDX","Cluster-trk DX",56,-28,28);  // 1mm bins
  TH1F *hClusterDY=new TH1F("hClusterDY","Cluster-trk DY",56,-28,28);

  // histograms for cluster x,y positions (after channel calibrations)
  TH1F *hClusterCalibX=new TH1F("hClusterCalibX","Cluster X (calibrated)",56,-28,28); // 1mm bins
  TH1F *hClusterCalibY=new TH1F("hClusterCalibY","Cluster Y (calibrated)",56,-28,28);
  TH1F *hClusterCalibSX=new TH1F("hClusterCalibSX","Cluster SX (calibrated)",112,0,28);
  TH1F *hClusterCalibSY=new TH1F("hClusterCalibSY","Cluster SY (calibrated)",112,0,28);
  // and cluster center vs extrapolated track (after channel calibrations)
  TH1F *hClusterCalibDX=new TH1F("hClusterCalibDX","Cluster-trk DX (calibrated)",56,-28,28);  // 1mm bins
  TH1F *hClusterCalibDY=new TH1F("hClusterCalibDY","Cluster-trk DY (calibrated)",56,-28,28);


  // location of maximum fiber, Upstream or Downstream
  TH2F *hMax = new TH2F("hMax","",1,1,1,1,1,1);
  Mapper *mapper=Mapper::Instance();
  mapper->SetChannelBins(hMax);

  // cluster "energies"
  const float EMIN=5000;
  const float EMAX=20000;
  TH1F* hClusterE=new TH1F("hClusterE","Cluster E", 40, EMIN, EMAX);
  TH1F* hClusterECalib=new TH1F("hClusterECalib","Cluster E (calibrated)", 40, EMIN, EMAX);

  // create a pointer to an event object for reading the branch values.
  TBEvent *event = new TBEvent(); 
  TBranch *bevent = t1041->GetBranch("tbevent");
  bevent->SetAddress(&event);

  // loop over events
  CalCluster calCluster, calClusterCalib;
  vector<CalHit> calhits, calhitsCalib;
  
  int SingleHitEvent(0), WC1SingleHitMultiEvent(0), WC2SingleHitMultiEvent(0), MultiEvent(0);
  int SingleHit(0), WC1SingleHitMulti(0), WC2SingleHitMulti(0), Multi(0), InTime(0);

  for (Int_t i=0; i< t1041->GetEntries(); i++) {
    if (i%200==0) cout << i << "/" << t1041->GetEntries() << endl;
    t1041->GetEntry(i);
    
    // loop over PADE channels
    // cout << (dec) << "Spill number: " << event->GetSpillNumber()<<endl;

    
    vector<WCChannel> hitsX1, hitsY1, hitsX2, hitsY2;
    hitsX1=event->GetWChitsX(1,tLow,tHigh);   // fetch x,y hits in chambers 1 and 2
    hitsY1=event->GetWChitsY(1,tLow,tHigh);   // only selecting in-time hits
    hitsX2=event->GetWChitsX(2,tLow,tHigh);
    hitsY2=event->GetWChitsY(2,tLow,tHigh);
    bool haveTrack      = (hitsX1.size()==1 && hitsY1.size()==1 && hitsX2.size()==1 && hitsY2.size()==1);   // require only 2 x,y hits
    bool multipleTracks = (((hitsX1.size()>1 && hitsY1.size()==1)||(hitsX1.size()==1 && hitsY1.size()>1)||(hitsX1.size()>1 && hitsY1.size()>1)) && ((hitsX2.size()==1 && hitsY2.size()>1)||(hitsX2.size()>1 && hitsY2.size()==1)||(hitsX2.size()>1 && hitsY2.size()>1))); // require more than one hit 
    bool onehitWC1      = (hitsX1.size()==1 && hitsY1.size()==1 && ((hitsX2.size()>1 && hitsY2.size()==1)||(hitsX2.size()==1 && hitsY2.size()>1)||(hitsX2.size()>1 && hitsY2.size()>1))); // 1 hit in WC1
    bool onehitWC2      = (((hitsX1.size()>1 && hitsY1.size()==1)||(hitsX1.size()==1 && hitsY1.size()>1)||(hitsX1.size()>1 && hitsY1.size()>1)) && hitsX2.size()==1 && hitsY2.size()==1); // 1 hit in WC2
    bool totintime      = (hitsX1.size()>=1 && hitsY1.size()>=1 && hitsX2.size()>=1 && hitsY2.size()>=1); // total in time hits
    

    // just doing peak=pedestal subtraction
    //    event->GetCalHits(calhits,0,3.0);  // fetch ped-subtracted calorimter hits, no calib, 3sigma noise cut
    //    event->GetCalHits(calhitsCalib,CalConstants,3.0);  // and again w/ Shasha's inter calibration, 3sigma noise cut
    // using the pulse shape fit  [ this takes MUCH longer ]
    if (i==0) cout << "We are doing pulse fits here, so this will be SLOW" << endl;
    event->GetCalHitsFit(calhits,0,3.0);  // fetch ped-subtracted calorimter hits, no calib, 3sigma noise cut
    calhitsCalib=calhits;
    event->CalibrateCalHits(calhitsCalib,CalConstants);  // much faster than redoing fit
    //    event->GetCalHitsFit(calhitsCalib,CalConstants,3.0);  // and again w/ Shasha's inter calibration, 3sigma noise c


    // Loop over calhits to find the channel with maximum energy
    // Use CalHits here and not the loop over PadeChannels above, b/c
    // CalHits are pedestal corrected and our bad channel is remapped to the opposing SIPM
    float max=0;
    float x,y,z;
    for (unsigned k=0; k<calhits.size(); k++) { 
      if (calhits[k].Value()>max){
	max=calhits[k].Value();
	calhits[k].GetXYZ(x,y,z);
      }
    }
    if (max>0) hMax->Fill(x,y);

    // Analysis of calorimter clusters
    // plot x,y positions and "energy" (really ADC value) clusters
    // Warning clustering code is not well vetted
    calCluster.MakeCluster(calhits);  // cut requiring adcMin counts>pedistal
    if (calCluster.GetE()==0) continue;
    // calCluster.Print();
    bool isContained= TMath::Abs( calCluster.GetX()<14 ) && TMath::Abs( calCluster.GetY()<14 );

    hClusterX->Fill(calCluster.GetX());
    hClusterY->Fill(calCluster.GetY());
    hClusterSX->Fill(calCluster.GetSigX());
    hClusterSY->Fill(calCluster.GetSigY());
    hClusterSXY->Fill(calCluster.GetSigX(),calCluster.GetSigY());

    if (isContained) hClusterE->Fill( Min((double)calCluster.GetE(), EMAX-0.0001) );
   

    calClusterCalib.MakeCluster(calhitsCalib);  
    isContained= TMath::Abs( calClusterCalib.GetX()<14 ) && TMath::Abs( calClusterCalib.GetY()<14 );

    hClusterCalibX->Fill(calClusterCalib.GetX());
    hClusterCalibY->Fill(calClusterCalib.GetY());
    hClusterCalibSX->Fill(calClusterCalib.GetSigX());
    hClusterCalibSY->Fill(calClusterCalib.GetSigY());
    if (isContained) hClusterECalib->Fill( Min((double)calClusterCalib.GetE(), EMAX-0.0001) );


    /// TABLE POSITION ///
    float TableX = 20.0;
    float TableY = 20.0;
 
    if(totintime)InTime++;

    if (haveTrack){
      SingleHitEvent++;
      WCtrack track(hitsX1[0],hitsY1[0],hitsX2[0],hitsY2[0]); // fit a track
      hslopeX->Fill(track.GetSlopeX());
      hslopeY->Fill(track.GetSlopeY());
      hDeltaXYWC1->Fill(track.TimingDifferenceWC1()); 
      hDeltaXYWC2->Fill(track.TimingDifferenceWC2()); 
      // from Jordon
      float trackX, trackY;
      float offSetX, offSetY; 
      // projecting at Scintillator 2 //
      track.Project(zSC2, trackX, trackY);
      Scint2->Fill(trackX, trackY, 1);
      
      if(fabs(trackX) > 50 && fabs(trackY) > 50) continue;
      SingleHit++;
      // projecting at Scintillator 1 //
      track.Project(zSC1, trackX, trackY);
      Scint1->Fill(trackX, trackY, 1);
      // projecting at WireChamber 1 //
      track.Project(zWC1, trackX, trackY);
      WC1->Fill(trackX, trackY, 1);
      // projecting at WireChamber 2 //
      track.Project(zWC2, trackX, trackY);
      WC2->Fill(trackX, trackY, 1);
      // projecting at Shashlik //
      track.Project(zShash,trackX,trackY); 
      //Shashlik->Fill(trackX, trackY, 1);         
      track.TablePos(trackX, TableX, trackY, TableY, offSetX, offSetY); 
      Shashlik->Fill(offSetX, offSetY, 1);

      htrackX->Fill(trackX);
      htrackY->Fill(trackY); 

      htrackX->Fill(trackX);
      htrackY->Fill(trackY); 

      if ( isContained ){
	hClusterDX->Fill(calCluster.GetX()-trackX);
	hClusterDY->Fill(calCluster.GetY()-trackY);
	hClusterCalibDX->Fill(calClusterCalib.GetX()-trackX);
	hClusterCalibDY->Fill(calClusterCalib.GetY()-trackY);
      }
    }
    // multiple in time hits study //
    
    if(multipleTracks){
      MultiEvent++;
      vector<vector<float> > Slopes;
      //cout << "event: " << i << endl;
      for(unsigned h1=0; h1<hitsX1.size(); ++h1){ // loop over X1 
        for(unsigned h2=0; h2<hitsY1.size(); ++h2){ // loop over Y1
          for(unsigned h3=0; h3<hitsX2.size(); ++h3){ // loop over X2 
            for(unsigned h4=0; h4<hitsY2.size(); ++h4){ // loop over Y2
              WCtrack multiTrack(hitsX1[h1], hitsY1[h2], hitsX2[h3], hitsY2[h4]);
              hDeltaXYWC1_MoreThan1Hit->Fill(multiTrack.TimingDifferenceWC1());
              hDeltaXYWC2_MoreThan1Hit->Fill(multiTrack.TimingDifferenceWC2());
              float trackX, trackY;
              float huge = 10000.0;
              hslopeX_MoreThan1Hit->Fill(multiTrack.GetSlopeX());
              hslopeY_MoreThan1Hit->Fill(multiTrack.GetSlopeY());
              multiTrack.Project(huge, trackX, trackY);
              if(fabs(trackX)>70 && fabs(trackY)>70)continue;
              multiTrack.Project(zSC2, trackX, trackY);
              if(fabs(trackX)>50 && fabs(trackY)>50)continue;
              MultiScint2->Fill(trackX, trackY, 1);
              multiTrack.Project(zSC1, trackX, trackY);
              if(fabs(trackX)>50 && fabs(trackY)>50)continue;
              MultiScint1->Fill(trackX, trackY, 1);
              Multi++;
              float sample = 200.5;
              multiTrack.Project(zWC1, trackX, trackY);
              //cout <<"WC1: " << h1+1 <<":" << h2+1 << ":" << h3+1 << ":" << h4+1 << ":" << trackX << " and " << trackY << endl;
              MultiWC1->Fill(trackX, trackY, 1);
              multiTrack.Project(zWC2, trackX, trackY);
              //cout <<"WC2: " << h1+1 <<":" << h2+1 << ":" << h3+1 << ":" << h4+1 << ":" << trackX << " and " << trackY << endl;
              MultiWC2->Fill(trackX, trackY, 1);
            }
          }
        }
      }
    }
    if(onehitWC1){
      vector <float> XPosition, XPosition_AdjRem;
      vector <float> YPosition, YPosition_AdjRem;
      vector <float> XSlope, YSlope;
      vector <float> MinimumSlope;
      WC1SingleHitMultiEvent++;
      int flag1 = 0;
      for(unsigned h3=0; h3<hitsX2.size(); ++h3){ // loop over X2
        int flag = 0; 
        for(unsigned h4=0; h4<hitsY2.size(); ++h4){ // loop over Y2
          WCtrack oneTrackWC1(hitsX1[0], hitsY1[0], hitsX2[h3], hitsY2[h4]);
          float X_pos, Y_pos;
          oneTrackWC1.Project(zSC2, X_pos,Y_pos);
          if(fabs(X_pos)>50 && fabs(Y_pos)>50)continue;
          flag++; flag1++;
          oneTrackWC1.Project(zWC2, X_pos,Y_pos);
          if(flag == 1)XPosition.push_back(X_pos);
          YPosition.push_back(Y_pos);
          MultiWC2_1HitWC1Not->Fill(X_pos, Y_pos, 1);
          if(h4>0 && fabs(YPosition[h4]-YPosition[h4-1])==1)continue;
          if(h3>0 && fabs(XPosition[h3]-XPosition[h3-1])==1)continue;
          XPosition_AdjRem.push_back(X_pos);
          YPosition_AdjRem.push_back(Y_pos);
          XSlope.push_back(fabs(oneTrackWC1.GetSlopeX())); // taking the absolute value of slope
          YSlope.push_back(fabs(oneTrackWC1.GetSlopeY())); // taking the absolute value of the slope
          MinimumSlope.push_back(oneTrackWC1.GetSlopeX()*oneTrackWC1.GetSlopeX() + oneTrackWC1.GetSlopeY()*oneTrackWC1.GetSlopeY());
          WC1SingleHitMulti++;
          if(flag1==1){
            oneTrackWC1.Project(zWC1, X_pos,Y_pos);
            MultiWC1_1HitWC1->Fill(X_pos, Y_pos, 1);
          }
          oneTrackWC1.Project(zWC2, X_pos,Y_pos);
          MultiWC2_1HitWC1->Fill(X_pos, Y_pos, 1);
          hslopeX_1hitWC1->Fill(oneTrackWC1.GetSlopeX());
          hslopeY_1hitWC1->Fill(oneTrackWC1.GetSlopeY());
          if(h3 == 0 && h4 == 0)hDeltaXYWC1_1hitWC1->Fill(oneTrackWC1.TimingDifferenceWC1());
          hDeltaXYWC2_1hitWC1->Fill(oneTrackWC1.TimingDifferenceWC2());
        }
      }
      if(XSlope.size()>0 && YSlope.size()>0){
        float MinSlope = *std::min_element(MinimumSlope.begin(), MinimumSlope.end());
        bool check = false ; // check if we got the right one
        for(unsigned x1=0; x1 < XSlope.size(); ++x1){
          for(unsigned y1=0; y1 < YSlope.size(); ++y1){
            if(XSlope[x1]*XSlope[x1]+YSlope[y1]*YSlope[y1]==MinSlope){
              check = true;
              MultiWC2_1HitWC1_AdjRem->Fill(XPosition_AdjRem[x1],YPosition_AdjRem[y1], 1);
              break;
            }
          }
          if(check)break;
        }
      }
    }
    if(onehitWC2){
      vector <float> XPosition, XPosition_AdjRem;
      vector <float> YPosition, YPosition_AdjRem;
      vector <float> XSlope, YSlope;
      vector <float> MinimumSlope;
      int flag1 = 0;
      WC2SingleHitMultiEvent++;
      for(unsigned h1=0; h1<hitsX1.size(); ++h1){ // loop over X1 
        int flag = 0;
        for(unsigned h2=0; h2<hitsY1.size(); ++h2){ // loop over Y1
          WCtrack oneTrackWC2(hitsX1[h1], hitsY1[h2], hitsX2[0], hitsY2[0]);
          float X_pos, Y_pos;
          oneTrackWC2.Project(zSC2, X_pos,Y_pos);
          if(fabs(X_pos)>50 && fabs(Y_pos)>50)continue;
          flag++; flag1++;
          oneTrackWC2.Project(zWC1, X_pos,Y_pos);
          if(flag == 1)XPosition.push_back(X_pos);
          YPosition.push_back(Y_pos);
          MultiWC1_1HitWC2Not->Fill(X_pos, Y_pos, 1);
          if(h2>0 && fabs(YPosition[h2]-YPosition[h2-1])==1)continue;
          if(h1>0 && fabs(XPosition[h1]-XPosition[h1-1])==1)continue;
          XPosition_AdjRem.push_back(X_pos);
          YPosition_AdjRem.push_back(Y_pos);
          XSlope.push_back(fabs(oneTrackWC2.GetSlopeX())); // taking the absolute value of slope
          YSlope.push_back(fabs(oneTrackWC2.GetSlopeY())); // taking the absolute value of the slope
          MinimumSlope.push_back(oneTrackWC2.GetSlopeX()*oneTrackWC2.GetSlopeX() + oneTrackWC2.GetSlopeY()*oneTrackWC2.GetSlopeY());
          
          WC2SingleHitMulti++;
          if(flag1==1){
            oneTrackWC2.Project(zWC2, X_pos,Y_pos);
            MultiWC2_1HitWC2->Fill(X_pos, Y_pos, 1);
          }
          oneTrackWC2.Project(zWC1, X_pos,Y_pos);
          MultiWC1_1HitWC2->Fill(X_pos, Y_pos, 1);
          hslopeX_1hitWC2->Fill(oneTrackWC2.GetSlopeX());
          hslopeY_1hitWC2->Fill(oneTrackWC2.GetSlopeY());
          hDeltaXYWC1_1hitWC2->Fill(oneTrackWC2.TimingDifferenceWC1());
          if(h1 == 0 && h2 == 0)hDeltaXYWC2_1hitWC2->Fill(oneTrackWC2.TimingDifferenceWC2());
        }
      }
      if(XSlope.size()>0 && YSlope.size()>0){
        float MinSlope = *std::min_element(MinimumSlope.begin(), MinimumSlope.end());
        bool check = false ; // check if we got the right one
        for(unsigned x1=0; x1 < XSlope.size(); ++x1){
          for(unsigned y1=0; y1 < YSlope.size(); ++y1){
            if(XSlope[x1]*XSlope[x1]+YSlope[y1]*YSlope[y1]==MinSlope){
              check = true;
              MultiWC1_1HitWC2_AdjRem->Fill(XPosition_AdjRem[x1],YPosition_AdjRem[y1], 1);
              break;
            }
          }
          if(check)break;
        }
      }
    }
  }
  cout << "total in time events in both WC                       : " << InTime << endl;
  cout << "single hit events in both WC                          : " << SingleHitEvent << endl;
  cout << "single hit tracks after scintillator correction       : " << SingleHit << endl;
  cout << "multiple hit events, 1 hit in WC1                     : " << WC1SingleHitMultiEvent << endl;
  cout << "multiple hit tracks after scin corr, 1 hit in WC1     : " << WC1SingleHitMulti << endl;
  cout << "multiple hit events, 1 hit in WC 2                    : " << WC2SingleHitMultiEvent << endl;
  cout << "multiple hit tracks after scin corr, 1 hit in WC 2    : " << WC2SingleHitMulti << endl;
  cout << "multiple hit events, worst case                       : " << MultiEvent << endl;
  cout << "multiple hit tracks after scin corr, worst case       : " << Multi << endl;
  cout << "Making Canvas for Tracking Display!!" << endl;

  TCanvas *A  = new TCanvas("TDCTiming", "Timing fits display", 1200, 900);
  A->Divide(4,4);
  for(int k=1; k<=16;++k){
    A->cd(k);
    hTDC[k-1]->Draw();
  }
  A->SaveAs("TimingCanvas.gif");
  A->SaveAs("TimingCanvas.pdf");

  TCanvas *B  = new TCanvas("TrackSlopes", "Slopes of tracks display", 1200, 900);
  B->Divide(2,4);
  B->cd(1);
  //gStyle->SetOptStat("1111");
  hslopeX->Draw();
  B->cd(2);
  //gStyle->SetOptStat("111111");
  hslopeY->Draw();
  B->cd(3);
  //gStyle->SetOptStat("111111");
  hslopeX_1hitWC1->Draw();
  B->cd(4);
  //gStyle->SetOptStat("111111");
  hslopeY_1hitWC1->Draw();
  B->cd(5);
  //gStyle->SetOptStat("111111");
  hslopeX_1hitWC2->Draw();
  B->cd(6);
  //gStyle->SetOptStat("111111");
  hslopeY_1hitWC2->Draw();
  B->cd(7);
  hslopeX_MoreThan1Hit->Draw();
  B->cd(8);
  hslopeY_MoreThan1Hit->Draw();
  B->SaveAs("SlopesCanvas.gif");
  B->SaveAs("SlopesCanvas.pdf");

  TCanvas *BB  = new TCanvas("TrackTimingDiffs", "Timing difference display", 1200, 900);
  BB->Divide(2,4);
  BB->cd(1);
  hDeltaXYWC1->Draw();
  BB->cd(2);
  hDeltaXYWC2->Draw();
  BB->cd(3);
  hDeltaXYWC1_1hitWC1->Draw();
  BB->cd(4);
  hDeltaXYWC2_1hitWC1->Draw();
  BB->cd(5);
  hDeltaXYWC1_1hitWC2->Draw();
  BB->cd(6);
  hDeltaXYWC2_1hitWC2->Draw();
  BB->cd(7);
  hDeltaXYWC1_MoreThan1Hit->Draw();
  BB->cd(8);
  hDeltaXYWC2_MoreThan1Hit->Draw();
  BB->SaveAs("TimingDifferences.gif");
  BB->SaveAs("TimingDifferences.pdf");

  TCanvas *AA = new TCanvas("TrackDisp", "Tracking Event Display", 1200, 900);
  AA->Divide(2,2);
  AA->cd(1);
  gStyle->SetOptStat(""); 
  Scint2->Draw("COLZ");
  DrawScintBox();
  AA->cd(2);
  gStyle->SetOptStat(""); 
  Scint1->Draw("COLZ");
  DrawScintBox();
  AA->cd(3);
  gStyle->SetOptStat(""); 
  WC1->Draw("COLZ");
  AA->cd(4);
  gStyle->SetOptStat(""); 
  Shashlik->Draw("COLZ");
  DrawShashBox();

  AA->SaveAs("TestCanvas.gif");
  AA->SaveAs("TestCanvas.pdf");

  TCanvas *AAA = new TCanvas("MultipleTrackDisp", "Tracking Event Display for Multiple tracks", 1200, 900);
  AAA->Divide(2,2);
//   AAA->cd(1);
//   gStyle->SetOptStat(""); 
//   MultiScint2->Draw("COLZ");
//   DrawScintBox();
//   AAA->cd(2);
//   gStyle->SetOptStat(""); 
//   MultiScint1->Draw("COLZ");
//   DrawScintBox();
  AAA->cd(1);
  gStyle->SetOptStat(""); 
  WC1->Draw("COLZ");
  AAA->cd(2);
  gStyle->SetOptStat(""); 
  WC2->Draw("COLZ");
  AAA->cd(3);
  gStyle->SetOptStat(""); 
  MultiWC1->Draw("COLZ");
  AAA->cd(4);
  gStyle->SetOptStat(""); 
  MultiWC2->Draw("COLZ");

  AAA->SaveAs("MultipleTestCanvas.gif");
  AAA->SaveAs("MultipleTestCanvas.pdf");

  TCanvas *BBB = new TCanvas("MultipleAdjacentHitRemoveTrack", "Tracking Event Display for Multiple tracks after removed adjacent hits", 1200, 900);
  BBB->Divide(2,2);
  gStyle->SetOptStat("1111");
  BBB->cd(1); 
  MultiWC1_1HitWC1->Draw("COLZ");
  //BBB->cd(2);
  //MultiWC2_1HitWC1Not->Draw("COLZ");
  //BBB->cd(3); 
  //MultiWC1_1HitWC1->Draw("COLZ");
  BBB->cd(2);
  MultiWC2_1HitWC1->Draw("COLZ");
  BBB->cd(3);
  MultiWC1_1HitWC1->Draw("COLZ");
  BBB->cd(4);
  MultiWC2_1HitWC1_AdjRem->Draw("COLZ");
  BBB->SaveAs("MultipleHitsAdjacentHitRemovedWC1.gif");
  BBB->SaveAs("MultipleHitsAdjacentHitRemovedWC1.pdf"); 


  TCanvas *AB = new TCanvas("MultipleAdjacentHitRemoveTrack2", "Tracking Event Display for Multiple tracks after removed adjacent hits", 1200, 900);
  AB->Divide(2,2);
  gStyle->SetOptStat("1111");
//   AB->cd(1);
//   MultiWC1_1HitWC2Not->Draw("COLZ");
//   AB->cd(2);
//   MultiWC2_1HitWC2->Draw("COLZ");
  AB->cd(1);
  MultiWC1_1HitWC2->Draw("COLZ");
  AB->cd(2);
  MultiWC2_1HitWC2->Draw("COLZ");
  AB->cd(3);
  MultiWC1_1HitWC2_AdjRem->Draw("COLZ");
  AB->cd(4);
  MultiWC2_1HitWC2->Draw("COLZ");
  AB->SaveAs("MultipleHitsAdjacentHitRemovedWC2.gif");
  AB->SaveAs("MultipleHitsAdjacentHitRemovedWC2.pdf");

  
  

  tfo->Write();
  tfo->Close();
    
  cout << "\nCreated: " << tfo->GetName() << endl;
}

