#ifndef WAVEVIEWER_H
#define WAVEVIEWER_H



#include "TGFrame.h"
#include "TRootEmbeddedCanvas.h"
#include "TGFileDialog.h"
#include "TGCanvas.h"
#include "TGStatusBar.h"
#include "TGNumberEntry.h"
#include "TGLabel.h"
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
#include "TBits.h"
#include <vector>
#include <set>

using std::vector; 



const UInt_t BoardIDs[] = { 112, 115, 116, 117 }; 
const UInt_t ChannelIDs[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31 }; 



class waveInterface : public TQObject { 

  RQ_OBJECT("waveInterface"); 
  ClassDef(waveInterface,1);

 private: 
  void loadRootFile(); 

  TString _filename; 
  TGMainFrame *_FMain; 
  TGVerticalFrame *_buttonFrame; 
  TGHorizontalFrame *_boardAndchannelFrame; 
  TGHorizontalFrame *_actionFrame; 
  TGVerticalFrame *_spectrumFrame; 
  TGCompositeFrame *_channelFrame; 
  TGVerticalFrame *_histogramFrame; 
  TGHorizontalFrame *_addFrame; 
  TGGroupFrame *_boardFrame; 



  TRootEmbeddedCanvas   *_waveformCanvas;
  TRootEmbeddedCanvas   *_histoCanvas; 

  TBEvent *_event; 
  TTree *_eventTree; 
  TBranch *_bevent; 
  TH1F *_waveform; 
  TH1F *_pulseHeight; 
  TThread *_timer; 

  UInt_t _delay; 
  UInt_t _width; 
  UInt_t _height; 
  Int_t _currentEntry; 
  UInt_t _currentChannel; 
  UInt_t _currentBoard; 
  UInt_t _minCount; 
  bool _playerStatus; 

  //Buttons 

  TGTextButton *_updateHistogramBN; 
  TGTextButton *_loadBN;
  TGTextButton *_goBN;
  TGTextButton *_stopBN;
  TGTextButton *_fitBN;
  TGTextButton *_nextenBN;
  TGTextButton *_prevenBN;

  TGTextButton *_nextchBN;
  TGTextButton *_prevchBN;
  TGTextButton *_firstchBN; 
 
  TGCheckButton *_addHistogramBN; 

  //Channel and Board Button List
  TObjArray _chList; 
  TObjArray _boardList; 
  std::set<Int_t> _chSet; 
  std::set<Int_t>::iterator _chiterator;
  

  //  TGHSlider *_slider; 
  TGNumberEntry *_delayBox; 
  TGNumberEntry *_minADC; 
  TGLabel *_adcLabel; 



  TFile *_f; 
  
  PadeChannel _padeChannel; 
  bool _showPulseFit;
  


 public: 
  waveInterface(const TString &filename, bool initialise=true); 
  waveInterface(bool initialise=true); 
  virtual ~waveInterface(); 
  void openFileDialog(); 
  void closeCleanup(); 
  void Go(); 
  void Stop(); 
  void Fit();

  bool PlayerStatus() { return _playerStatus; } 
  UInt_t Delay() { return _delay; } 


  bool nextOverthresholdCh(bool up); 
  void minADCUpdate(); 
  void waveformChUpdate(); 
  bool nextChannel(); 
  void prevChannel(); 
  bool nextEntry(); 
  void prevEntry(); 
  void firstChannel(); 
  void updateBoardSelection(Int_t brd); 
  void updateChannelSelection();
  void addHistogramPane();
  void updateHistogram(); 
  void enableChannelColumn(UInt_t col); 

  void setDelay(UInt_t delay) { _delay = delay;  }
  void delayBoxUpdate();


  void initWindow(UInt_t width=0, UInt_t height=0); 
  void makeButtons(); 

  void addFixedCheckBoxes(); 

  void connectButtons(); 
  void reset(); 
  void updateFrame(UInt_t entry, UInt_t channel); 
  
}; 

#endif 
