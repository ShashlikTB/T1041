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
#include "TBReco.h"
#include "WC.h"
#include "TBEvent.h"
#include "calConstants.h"
#include "Mapper.h"

using std::cout;
using std::endl;
using std::vector;
using TMath::Min;

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
  TH1F *hslopeX=new TH1F("hslopeX","Beam slope in X",50,-0.005,0.005);
  TH1F *hslopeY=new TH1F("hslopeY","Beam slope in Y",50,-0.005,0.005);
  TH1F *htrackX=new TH1F("hTrackX","Track X",56,-28,28);  // 1mm bins
  TH1F *htrackY=new TH1F("hTrackY","Track Y",56,-28,28);

  // plot location of tracks projected onto face of the detector
  // TBD

  // histograms of peak times
  TH1I *htime112=new TH1I("time112","Timing board 112",120,0,120);
  TH1I *htime113=new TH1I("time113","Timing board 113",120,0,120);
  TH1I *htime115=new TH1I("time115","Timing board 115",120,0,120);
  TH1I *htime116=new TH1I("time116","Timing board 116",120,0,120);

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
  TBSpill *spill = new TBSpill();
  TBranch *bevent = t1041->GetBranch("tbevent");
  TBranch *bspill = t1041->GetBranch("tbspill");
  bevent->SetAddress(&event);
  bspill->SetAddress(&spill);

  // loop over events
  CalCluster calCluster, calClusterCalib;
  vector<CalHit> calhits, calhitsCalib;

  for (Int_t i=0; i< t1041->GetEntries(); i++) {

    if (i%200==0) cout << i << "/" << t1041->GetEntries() << endl;
    t1041->GetEntry(i);

    if (i==0){
      cout << "Table position (x,y) : (" 
	   << spill->GetTableX() << "," << spill->GetTableY() << ")" << endl;
    }
    
    // loop over PADE channels
    // cout << (dec) << "Spill number: " << event->GetSpillNumber()<<endl;
    
    for (Int_t j=0; j<event->NPadeChan(); j++){
      PadeChannel pc=event->GetPadeChan(j);
      int board=pc.GetBoardID();
      if (board==112) htime112->Fill(pc.GetPeak());
      if (board==113) htime113->Fill(pc.GetPeak());
      if (board==115) htime115->Fill(pc.GetPeak());
      if (board==116) htime116->Fill(pc.GetPeak());
    }

    
    vector<WCChannel> hitsX1, hitsY1, hitsX2, hitsY2;
    hitsX1=event->GetWChitsX(1,tLow,tHigh);   // fetch x,y hits in chambers 1 and 2
    hitsY1=event->GetWChitsY(1,tLow,tHigh);   // only selecting in-time hits
    hitsX2=event->GetWChitsX(2,tLow,tHigh);
    hitsY2=event->GetWChitsY(2,tLow,tHigh);
    bool haveTrack= (hitsX1.size()==1 && hitsY1.size()==1 && 
		     hitsX2.size()==1 && hitsY2.size()==1);   // require only 2 x,y hits

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
    double x,y,z;
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


 
    if (haveTrack){
      WCtrack track(hitsX1[0],hitsY1[0],hitsX2[0],hitsY2[0]); // fit a track
      hslopeX->Fill(track.GetSlopeX());
      hslopeY->Fill(track.GetSlopeY());  

      float trackX, trackY;
      track.Project(zShash,trackX,trackY);

      htrackX->Fill(trackX);
      htrackY->Fill(trackY); 

      if ( isContained ){
	hClusterDX->Fill(calCluster.GetX()-trackX);
	hClusterDY->Fill(calCluster.GetY()-trackY);
	hClusterCalibDX->Fill(calClusterCalib.GetX()-trackX);
	hClusterCalibDY->Fill(calClusterCalib.GetY()-trackY);
      }
    }    
  }

  tfo->Write();
  tfo->Close();
    
  cout << "\nCreated: " << tfo->GetName() << endl;
}

