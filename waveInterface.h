#ifndef WAVEVIEWER_H
#define WAVEVIEWER_H



#include "TGFrame.h"
#include "TRootEmbeddedCanvas.h"
#include "TGFileDialog.h"
#include "TGCanvas.h"
#include "TGStatusBar.h"
#include "TGNumberEntry.h"
#include "TArrow.h"
#include "TPad.h"
#include "TH1F.h"
#include "TPaveLabel.h"
#include "TPaveText.h"
#include "TFile.h"
#include "TString.h"
#include "TTree.h"
#include "RQ_OBJECT.h"
#include "TThread.h"
#include "TGSlider.h"
#include "TBEvent.h"



class waveInterface : public TQObject { 
  RQ_OBJECT("waveInterface"); 
  ClassDef(waveInterface,1);

 private: 
  void loadRootFile(); 

  TString _filename; 
  TGMainFrame *_FMain; 
  TGVerticalFrame *_buttonFrame; 
  TGVerticalFrame *_spectrumFrame; 

  TRootEmbeddedCanvas   *_waveformCanvas;

  TBEvent *_event; 
  TTree *_eventTree; 
  TBranch *_bevent; 
  TH1F *_waveform; 
  TThread *_timer; 

  UInt_t _delay; 
  UInt_t _width; 
  UInt_t _height; 
  Int_t _currentEntry; 
  Int_t _currentChannel; 
  bool _playerStatus; 

  //Buttons 
  TGTextButton *_loadBN;
  TGTextButton *_goBN;
  TGTextButton *_stopBN;
  TGTextButton *_nextenBN;
  TGTextButton *_prevenBN;

  TGTextButton *_nextchBN;
  TGTextButton *_prevchBN;
  TGTextButton *_firstchBN;
  
  //  TGHSlider *_slider; 
  TGNumberEntry *_delayBox; 

  


  TFile *_f; 
  
  PadeChannel _padeChannel; 
  


 public: 
  waveInterface(const TString &filename, bool initialise=true); 
  waveInterface(bool initialise=true); 
  virtual ~waveInterface(); 
  void openFileDialog(); 
  void closeCleanup(); 
  void Go(); 
  void Stop(); 

  bool PlayerStatus() { return _playerStatus; } 
  UInt_t Delay() { return _delay; } 
  
  bool nextChannel(); 
  void prevChannel(); 
  bool nextEntry(); 
  void prevEntry(); 
  void firstChannel(); 

  void setDelay(UInt_t delay) { _delay = delay;  }
  void delayBoxUpdate();

  void initWindow(UInt_t width=0, UInt_t height=0); 
  void makeButtons(); 
  void connectButtons(); 
  void reset(); 
  void updateFrame(UInt_t entry, UInt_t channel); 
  
}; 

#endif 
