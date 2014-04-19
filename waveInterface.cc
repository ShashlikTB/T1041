// ROOT Interface for viewing waveform's generated from the test shashlik module
// Initial Version 0.00000001 
// Thomas Anderson

#include <iostream> 
#include <vector>
using std::vector; 


//Root Includes
#include <TSystem.h>
#include <TObject.h>
#include <TInterpreter.h>
#include <TApplication.h> 
#include "TString.h"
#include "TSpectrum.h"
#include "TList.h"
#include "TPolyMarker.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TFitResultPtr.h"
#include "TF1.h"
#include <TNtuple.h>
#include "TTree.h"
#include "TStyle.h"
#include "TLatex.h"
#include "TQObject.h"
#include <TGButton.h>
#include <TGClient.h>
#include <TGLabel.h>
#include "TGFileBrowser.h"
#include "TROOT.h"
#include "TMath.h"
#include "TRegexp.h"
#include "TArrayF.h"
#include "TArrayC.h"
#include <TGSlider.h>


#include "waveInterface.h"


// Button Frame Constants
const UInt_t mainFrameWidth = 1000; 
const UInt_t mainFrameHeight = 750; 
const UInt_t buttonFrameWidth = 500; 
const UInt_t buttonFrameHeight = 250; 
const UInt_t spectrumFrameWidth = 1000; 
const UInt_t spectrumFrameHeight = 500; 



void *timer(void *ptr) { 
  waveInterface *interface = (waveInterface *) ptr; 

  while(1) { 
    if (!interface->PlayerStatus())
      break; 

    interface->nextChannel(); 
    gSystem->Sleep(interface->Delay());  
  }
  return 0; 
}


waveInterface::waveInterface(bool initialise) { 
  _f = NULL; 
  _filename = "" ;
  _width = 1000; 
  _height = 500; 
  _playerStatus = false; 
  //By default start the interface
  if (initialise)
    initWindow(); 
}

waveInterface::waveInterface(const TString &filename, bool initialise) { 
  _f = NULL; 
  _filename = filename; 
  _playerStatus = false; 


  std::cout << "Just starting up" << std::endl; 
  _width = 1000; 
  _height = 500; 
  if (initialise) {
    initWindow(); 
  }
  
}

waveInterface::~waveInterface() { 
  if (_FMain) {
    delete _FMain; 
  }

}



void waveInterface::initWindow(UInt_t width, UInt_t height) { 
  if (width != 0 && height != 0) {
    _width = width; 
    _height = height; 
  }


  _FMain = new TGMainFrame(gClient->GetRoot(), _width, _height, kVerticalFrame); 
  _FMain->SetWindowName("Test Beam Waveform Viewer v.00000001"); 
  _buttonFrame = new TGVerticalFrame(_FMain, buttonFrameWidth, buttonFrameHeight, kLHintsRight | kFixedWidth | kFixedHeight); 
  _spectrumFrame = new TGVerticalFrame(_FMain, spectrumFrameWidth, spectrumFrameHeight, kLHintsLeft | kFixedWidth); 
  _waveformCanvas = new TRootEmbeddedCanvas("waveform", _spectrumFrame, spectrumFrameWidth, spectrumFrameHeight, kSunkenFrame); 
  _spectrumFrame->AddFrame(_waveformCanvas, new TGLayoutHints(kLHintsExpandY, 10,10,10,1)); 

   makeButtons(); 
   connectButtons(); 
   if (_filename != "") { 
     loadRootFile(); 
   }
  _FMain->MapSubwindows(); 
  _FMain->Resize(_FMain->GetDefaultSize()); 
  _FMain->MapWindow(); 

}


void waveInterface::makeButtons()  {
  std::cout << "Adding buttons to display." << std::endl; 
  ULong_t red;
  ULong_t green;
  ULong_t blue;
  ULong_t gray;
  ULong_t white;
  gClient->GetColorByName("red", red);
  gClient->GetColorByName("green", green);
  gClient->GetColorByName("blue", blue);
  //  gClient->GetColorByName("gray", gray);
  gClient->GetColorByName("#888888",gray);
  gClient->GetColorByName("white", white);

  

  _loadBN = new TGTextButton(_buttonFrame, "&Load File");
  _loadBN->SetTextColor(blue);
  _loadBN->Connect("Clicked()", "waveInterface", this, "openFileDialog()");
  _loadBN->SetToolTipText("Select input ROOT file", 2000);
  _buttonFrame->AddFrame(_loadBN); 


  _firstchBN = new TGTextButton(_buttonFrame, "&First Channel");

  _buttonFrame->AddFrame(_firstchBN);

  _prevchBN = new TGTextButton(_buttonFrame, "&Prev Channel");

  _buttonFrame->AddFrame(_prevchBN);

  _nextchBN = new TGTextButton(_buttonFrame, "&Next Channel");

  _buttonFrame->AddFrame(_nextchBN);


  _prevenBN = new TGTextButton(_buttonFrame, "&Prev Entry");

  _buttonFrame->AddFrame(_prevenBN);

  _nextenBN = new TGTextButton(_buttonFrame, "&Next Entry");
 
  _buttonFrame->AddFrame(_nextenBN);



  _goBN = new TGTextButton(_buttonFrame, "&Go");
  _buttonFrame->AddFrame(_goBN);

  _stopBN = new TGTextButton(_buttonFrame, "&Stop");
  _buttonFrame->AddFrame(_stopBN); 


  _delayBox = new TGNumberEntry(_buttonFrame, 1000,9,999, TGNumberFormat::kNESInteger,
				TGNumberFormat::kNEANonNegative,TGNumberFormat::kNELLimitMinMax,100,10000);
  _buttonFrame->AddFrame(_delayBox); 

  //  _slider = new TGHSlider(_buttonFrame, 0, 200, kSlider1 | kScaleBoth, 100); 
  //  _slider->SetRange(0, 31); 
  //  _buttonFrame->AddFrame(_slider); 


  _FMain->AddFrame(_spectrumFrame, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY)); 
  _FMain->AddFrame(_buttonFrame, new TGLayoutHints(kLHintsExpandX)); 



}


void waveInterface::connectButtons()  {
  
  std::cout << "Connecting buttons" << std::endl; 

  _goBN->Connect("Clicked()", "waveInterface", this, "Go()"); 
  _stopBN->Connect("Clicked()", "waveInterface", this, "Stop()"); 
  _nextenBN->Connect("Clicked()", "waveInterface", this, "nextEntry()"); 
  _prevenBN->Connect("Clicked()", "waveInterface", this, "prevEntry()");

  _nextchBN->Connect("Clicked()", "waveInterface", this, "nextChannel()"); 
  _prevchBN->Connect("Clicked()", "waveInterface", this, "prevChannel()"); 
  _firstchBN->Connect("Clicked()", "waveInterface", this, "firstChannel()"); 
  _delayBox->Connect("ReturnPressed()", "waveInterface", this, "delayBoxUpdate()"); 
  _delayBox->Connect("ValueSet(Long_t)", "waveInterface", this, "delayBoxUpdate()"); 


  _FMain->Connect("CloseWindow()", "waveInterface", this, "closeCleanup()"); 

}

void waveInterface::delayBoxUpdate() { 

  _delay = _delayBox->GetIntNumber(); 

}

void waveInterface::closeCleanup() { 

  std::cout << "Got window close or Alt-F4, quitting!" << std::endl; 
  exit(0); 

}

void waveInterface::reset() { 
  _waveformCanvas->GetCanvas()->Clear(); 
  delete _waveform; 
  _waveform = NULL; 
  _f->Close(); 
  _event = NULL;
  _eventTree = NULL; 
  _waveform = NULL; 

  

}

void waveInterface::openFileDialog() { 

  std::cout << "Open something will ya!" << std::endl; 
  TGFileInfo *fileInfo = new TGFileInfo; 
  TGFileDialog *fileDialog = new TGFileDialog(gClient->GetRoot(), _FMain, kFDOpen, fileInfo); 
  std::cout << "Selected file: " << fileInfo->fFilename << std::endl; 
  

  if (fileInfo->fFilename == NULL) {
    std::cout << "File selection cancelled." << std::endl; 
    return; 
  }

  _filename = fileInfo->fFilename; 
  loadRootFile(); 

}

void waveInterface::Go() { 

  std::cout << "Starting channel playback..." << std::endl; 
  _playerStatus = true; 
  _timer = new TThread("timer", timer, this); 
  _timer->Run(); 

}

void waveInterface::Stop() { 
  std::cout << "Stopping channel playback..." << std::endl; 
  _playerStatus = false; 

}

void waveInterface::updateFrame(UInt_t entry, UInt_t channel) { 
  //Quick sanity check
  if (_f == NULL or _eventTree == NULL)
    return;

  _eventTree->GetEntry(entry); 
  _padeChannel = _event->GetPadeChan(channel); 
  _padeChannel.GetHist(_waveform); 

  _waveformCanvas->GetCanvas()->cd(); 
  std::cout << "Drawing Sample" << std::endl; 
  _waveform->Draw(); 

  _waveformCanvas->GetCanvas()->Modified(); 
  _waveformCanvas->GetCanvas()->Update(); 
}




    

void waveInterface::loadRootFile() { 

  if (_f != NULL && _f->IsZombie()) { 
    std::cout << "Closing stale root file" << std::endl; 
    delete _f; 
    _f = NULL; 
  }

  if (_f) { 
    std::cout << "Cleaning old data" << std::endl; 
    reset(); 

  }


  if (_filename == "")
    return; 


  std::cout << "Loading Root File" << std::endl; 
  _f = new TFile(_filename); 

  std::cout << "Getting events from Tree" << std::endl; 
  _event = new TBEvent(); 
  _eventTree = (TTree*)_f->Get("t1041"); 
  _bevent = _eventTree->GetBranch("tbevent"); 
  _bevent->SetAddress(&_event); 
  
  std::cout << "Building waveform histogram" << std::endl; 
  _waveform = new TH1F("hw", "waveform", 120, 0, 120);
  

  std::cout << "Getting test sample" << std::endl; 
  _eventTree->GetEntry(0); 

  _currentEntry = 0; 
  _currentChannel = 0; 

  _padeChannel = _event->GetPadeChan(0); 
  _padeChannel.GetHist(_waveform); 



  _waveformCanvas->GetCanvas()->cd(); 
  std::cout << "Drawing Sample" << std::endl; 
  _waveform->Draw(); 

  _waveformCanvas->GetCanvas()->Modified(); 
  _waveformCanvas->GetCanvas()->Update(); 


}


void waveInterface::firstChannel() { 
  _currentChannel = 0; 
  updateFrame(_currentEntry, _currentChannel); 
}

bool waveInterface::nextChannel() { 
  bool finished = true; 
  Int_t maxchans = _event->NPadeChan(); 
  if (_currentChannel < maxchans) {
    _currentChannel++; 
    finished = false; 
  }

  updateFrame(_currentEntry, _currentChannel); 
  return finished; 
}

void waveInterface::prevChannel() { 
  if (_currentChannel == 0) 
    return; 

  _currentChannel--; 
  updateFrame(_currentEntry, _currentChannel); 

}

bool waveInterface::nextEntry() { 
  bool finished = true; 
  Int_t maxentries = _eventTree->GetEntries(); 
  std::cout << "Current Entry:" << _currentEntry << " of: " << maxentries << std::endl; 

  if (_currentEntry < maxentries) {
    _currentEntry++; 
    finished = false; 
  }

  updateFrame(_currentEntry, _currentChannel); 
  return finished; 
}

void waveInterface::prevEntry() { 
  Int_t maxentries = _eventTree->GetEntries(); 
  std::cout << "Current Entry:" << _currentEntry << " of: " << maxentries << std::endl; 

  if (_currentEntry == 0)
    return; 
  _currentEntry--; 
  updateFrame(_currentEntry, _currentChannel); 
  

}
