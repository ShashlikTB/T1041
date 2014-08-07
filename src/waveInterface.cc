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
const UInt_t buttonFrameHeight = 300; 
const UInt_t spectrumFrameWidth = 1000; 
const UInt_t spectrumFrameHeight = 500; 




void *timer(void *ptr) { 
  waveInterface *interface = (waveInterface *) ptr; 

  while(1) { 
    if (!interface->PlayerStatus())
      break; 

    if (interface->nextChannel()) {
      interface->nextEntry(); 
      interface->firstChannel(); 
    }
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
  _delay=1000;
  _currentBoard = -1; 
  //By default start the interface
  if (initialise)
    initWindow(); 
}

waveInterface::waveInterface(const TString &filename, bool initialise) { 

  _f = NULL; 
  _filename = filename; 
  _playerStatus = false; 
  _currentBoard = -1; 

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
  _buttonFrame = new TGVerticalFrame(_FMain, buttonFrameWidth, buttonFrameHeight, kFixedWidth | kFixedHeight); 
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

void waveInterface::enableChannelColumn(UInt_t col) { 
  std::cout << "Enabling column" << col << std::endl; 
  UInt_t choffset = col*8; 
  std::cout << "Channel Offset:" << choffset << std::endl; 

  _chSet.clear(); 
  TGCheckButton *chosen; 
  bool enabled = true; 
  for (unsigned i = choffset; i < choffset+8; i++) { 
    chosen = (TGCheckButton *) _chList[i]; 
    enabled = enabled && chosen->IsOn(); 
  }

  EButtonState state = enabled == true ? kButtonUp : kButtonDown; 
  

  for (unsigned i = choffset; i < choffset+8; i++) { 
    chosen = (TGCheckButton *) _chList[i]; 
    chosen->SetState(state); 
    if (chosen->IsOn()) { 
      Int_t channel = chosen->GetString().Atoi(); 
      _chSet.insert(channel); 
    }
  }

  for (Int_t i = 0; i < _chList.GetEntries(); i++) { 
    chosen = (TGCheckButton *) _chList[i]; 
    if (chosen->IsOn()) { 
      Int_t channel = chosen->GetString().Atoi(); 
      _chSet.insert(channel); 
    }
  }


}


void waveInterface::addFixedCheckBoxes() { 
  // This will add a fixed set of Board and Channel Checkboxes rather than building 
  // the checkboxes from the data 

  std::cout << " Adding Board and channel buttons." << std::endl; 

  TGRadioButton *brd = NULL; 
  TGCheckButton *ch = NULL; 
  TString converter; 

  //Build board frame
  _boardAndchannelFrame = new TGHorizontalFrame(_buttonFrame); 

  const TString boardUpdateStr = "updateBoardSelection(=s)"; 
  TString token = "s"; 
  _boardFrame = new TGGroupFrame(_boardAndchannelFrame, "Boards", kVerticalFrame); 
  for (UInt_t i = 0; i < (sizeof(BoardIDs)/sizeof(UInt_t)); i++) { 
    TString updateStr = boardUpdateStr; 
    TString boardN = converter.Itoa(BoardIDs[i], 10); 
    brd = new TGRadioButton(_boardFrame, TGHotString(boardN)); 
    brd->Connect("Clicked()", "waveInterface", this, updateStr.ReplaceAll(token, boardN)); 

    std::cout << boardN << " " << updateStr.ReplaceAll(token, boardN) << std::endl; 
    _boardFrame->AddFrame(brd, new TGLayoutHints(kLHintsTop,5,5,5,1)); 
    _boardList.Add(brd); 

  }


  _channelFrame = new TGCompositeFrame(_boardAndchannelFrame); 
  _channelFrame->SetLayoutManager(new TGMatrixLayout(_channelFrame, 9, 0, 1)); 

  TGTextButton *all; 
  UInt_t col = 0; 
  TString allCall = "enableChannelColumn(=s)"; 
  for (UInt_t i = 0; i < (sizeof(ChannelIDs)/sizeof(UInt_t)); i++) { 
    if ((i%8) == 0) { 
      all= new TGTextButton(_channelFrame, "All"); 
      TString allStr = allCall; 
      all->Connect("Clicked()", "waveInterface", this, allStr.ReplaceAll("s",converter.Itoa(col, 10))); 
      _channelFrame->AddFrame(all, new TGLayoutHints(kLHintsTop,5,5,5,1)); 
      col++; 
    }

    ch = new TGCheckButton(_channelFrame, TGHotString(converter.Itoa(i, 10))); 
    ch->Connect("Clicked()", "waveInterface", this, "updateChannelSelection()");     
    _channelFrame->AddFrame(ch); 
    _chList.Add(ch); 
  }


  _boardAndchannelFrame->AddFrame(_boardFrame, new TGLayoutHints(kLHintsLeft | kLHintsBottom | kFixedWidth, 5,5,5,1)); 

  _minADC = new TGNumberEntry(_boardAndchannelFrame, 100, 9, 4600, TGNumberFormat::kNESInteger, TGNumberFormat::kNEANonNegative, TGNumberFormat::kNELLimitMinMax, 0, 4500); 

  _minADC->Connect("ReturnPressed()", "waveInterface", this, "minADCUpdate()"); 
  _minADC->Connect("ValueSet(Long_t)", "waveInterface", this, "minADCUpdate()"); 

  _boardAndchannelFrame->AddFrame(_minADC, new TGLayoutHints(kLHintsLeft,5,5,5,1)); 
  _adcLabel = new TGLabel(_boardAndchannelFrame, TGString("Minimum ADC")); 
  _boardAndchannelFrame->AddFrame(_adcLabel, new TGLayoutHints(kLHintsLeft,5,5,5,1)); 

  _boardAndchannelFrame->AddFrame(_channelFrame, new TGLayoutHints(kLHintsRight,5,5,5,1)); 

  _buttonFrame->AddFrame(_boardAndchannelFrame, new TGLayoutHints(kLHintsExpandX, 5,5,5,1)); 

  _addFrame = new TGHorizontalFrame(_buttonFrame); 
  _addHistogramBN = new TGCheckButton(_addFrame, TGHotString("Add Histogram Window")); 
  _addHistogramBN->Connect("Clicked()", "waveInterface", this, "addHistogramPane()"); 
  _addFrame->AddFrame(_addHistogramBN); 
  _updateHistogramBN = new TGTextButton(_addFrame, "Refresh Histogram"); 
  _updateHistogramBN->Connect("Clicked()", "waveInterface", this, "updateHistogram()"); 
  _addFrame->AddFrame(_updateHistogramBN, new TGLayoutHints(kLHintsLeft,5,5,5,1)); 
  

  _buttonFrame->AddFrame(_addFrame, new TGLayoutHints(kLHintsRight | kLHintsBottom | kLHintsExpandX, 5,5,5,1)); 
  


  _buttonFrame->MapSubwindows(); 
  _buttonFrame->Resize(); 


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

  _actionFrame = new TGHorizontalFrame(_buttonFrame); 


  

  _loadBN = new TGTextButton(_actionFrame, "&Load File");
  _loadBN->SetTextColor(blue);
  _loadBN->Connect("Clicked()", "waveInterface", this, "openFileDialog()");
  _loadBN->SetToolTipText("Select input ROOT file", 2000);
  _actionFrame->AddFrame(_loadBN, new TGLayoutHints(kLHintsTop,5,5,5,1)); 


  _firstchBN = new TGTextButton(_actionFrame, "&First Channel");

  _actionFrame->AddFrame(_firstchBN, new TGLayoutHints(kLHintsTop,5,5,5,1));

  _prevchBN = new TGTextButton(_actionFrame, "&Prev Channel");

  _actionFrame->AddFrame(_prevchBN, new TGLayoutHints(kLHintsTop,5,5,5,1));

  _nextchBN = new TGTextButton(_actionFrame, "&Next Channel");

  _actionFrame->AddFrame(_nextchBN, new TGLayoutHints(kLHintsTop,5,5,5,1));


  _prevenBN = new TGTextButton(_actionFrame, "&Prev Entry");

  _actionFrame->AddFrame(_prevenBN, new TGLayoutHints(kLHintsTop,5,5,5,1));

  _nextenBN = new TGTextButton(_actionFrame, "&Next Entry");
 
  _actionFrame->AddFrame(_nextenBN, new TGLayoutHints(kLHintsTop,5,5,5,1));



  _goBN = new TGTextButton(_actionFrame, "&Go");
  _actionFrame->AddFrame(_goBN, new TGLayoutHints(kLHintsTop,5,5,5,1));

  _stopBN = new TGTextButton(_actionFrame, "&Stop");
  _actionFrame->AddFrame(_stopBN, new TGLayoutHints(kLHintsTop,5,5,5,1)); 


  _delayBox = new TGNumberEntry(_actionFrame, 1000,9,999, TGNumberFormat::kNESInteger,
				TGNumberFormat::kNEANonNegative,TGNumberFormat::kNELLimitMinMax,100,10000);
  _actionFrame->AddFrame(_delayBox, new TGLayoutHints(kLHintsTop,5,5,5,1)); 

  //  _slider = new TGHSlider(_actionFrame, 0, 200, kSlider1 | kScaleBoth, 100); 
  //  _slider->SetRange(0, 31); 
  //  _actionFrame->AddFrame(_slider); 


  _buttonFrame->AddFrame(_actionFrame, new TGLayoutHints(kLHintsExpandX | kLHintsTop,5,5,5,1)); 
  _FMain->AddFrame(_spectrumFrame, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY)); 
  _FMain->AddFrame(_buttonFrame, new TGLayoutHints(kLHintsExpandX,10,10,10,1)); 



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


void waveInterface::addHistogramPane() { 

  if (!_addHistogramBN->IsOn()) { 
    _FMain->RemoveFrame(_histogramFrame); 
    _FMain->MapSubwindows(); 
    _FMain->Resize(); 
    delete _histogramFrame; 

    
  }
  else { 
    _histogramFrame = new TGVerticalFrame(_FMain, spectrumFrameWidth, spectrumFrameHeight, kLHintsLeft | kFixedWidth); 
    _histoCanvas = new TRootEmbeddedCanvas("pulse height histogram", _histogramFrame, spectrumFrameWidth, spectrumFrameHeight, kSunkenFrame); 
    _histogramFrame->AddFrame(_histoCanvas, new TGLayoutHints(kLHintsExpandY, 10,10,10,1)); 
    if (!_pulseHeight)
      _pulseHeight = new TH1F("pulse height", "pulse height", 4096, 0, 4096); 
    
    updateHistogram(); 
    _FMain->AddFrame(_histogramFrame, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY)); 
    _FMain->MapSubwindows(); 
    _FMain->Resize(); 
  }
  



}


void waveInterface::updateChannelSelection() { 


  _chSet.clear(); 

  std::cout << "Updating channel set" << std::endl; 
  TGCheckButton *chosen; 
  for (Int_t i = 0; i < _chList.GetEntries(); i++) { 
    chosen = (TGCheckButton *) _chList[i]; 
    if (chosen->IsOn()) { 
      Int_t channel = chosen->GetString().Atoi(); 
      _chSet.insert(channel); 
    }
  }


  std::cout << "Current channel set contents:" << std::endl; 
  std::set<Int_t>::iterator it;
  for (it = _chSet.begin(); it != _chSet.end(); ++it) { 

    std::cout << *it << " "; 
  }
  std::cout << std::endl; 


      
  

} 

void waveInterface::updateBoardSelection(Int_t brd) {

  std::cout << "Updating board selection" << std::endl; 
  std::cout << "Chose Board: " << brd << std::endl; 

  TGRadioButton *btn; 
  for (Int_t i = 0; i < _boardList.GetEntries(); i++) { 
    btn = (TGRadioButton *) _boardList[i]; 
    if (btn->IsOn()) { 
      Int_t boardN = btn->GetString().Atoi(); 
      if (boardN != brd)
	btn->SetState(kButtonUp); 
      else { 
	_currentBoard = boardN; 
	firstChannel(); 
      }
    }
  }

}

void waveInterface::delayBoxUpdate() { 

  _delay = _delayBox->GetIntNumber(); 

}

void waveInterface::minADCUpdate() { 

  _minCount = _minADC->GetIntNumber(); 
  std::cout << "Minimum ADC Count set to:" << _minCount << std::endl; 

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

void waveInterface::updateHistogram() { 

  if (!_addHistogramBN->IsOn()) { 
    std::cout << "No Histogram" << std::endl; 
    return; 
  }
  _histoCanvas->GetCanvas()->cd(); 
  



  vector<Int_t> channels; 
  if (_pulseHeight)
    _pulseHeight->Reset(); 
  std::cout << "Updating histogram" << std::endl; 

  for (Int_t i = 0; i < _event->NPadeChan(); i++) { 
    _padeChannel = _event->GetPadeChan(i); 
      if (_padeChannel.GetBoardID() == _currentBoard && _chSet.count(_padeChannel.GetChannelNum()) == 1) { 
	channels.push_back(i); 
      }
  }

  
  for (Int_t j = 0; j < _eventTree->GetEntries(); j++) { 
    _eventTree->GetEntry(j); 
    for (std::vector<Int_t>::iterator it = channels.begin(); it != channels.end(); ++it) 
      {
	_padeChannel = _event->GetPadeChan(*it); 
	_pulseHeight->Fill(_padeChannel.GetMax()); 
      }
  }

    _eventTree->GetEntry(_currentEntry); 
    _pulseHeight->SetStats(0); 
    _pulseHeight->Draw(); 
    _histoCanvas->GetCanvas()->Modified(); 
    _histoCanvas->GetCanvas()->Update(); 

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
  _waveform->SetStats(0); 
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


  
  _currentBoard = -1; 
  

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
  addFixedCheckBoxes(); 
  _waveform->SetStats(0); 
  _waveform->Draw(); 

  _waveformCanvas->GetCanvas()->Modified(); 
  _waveformCanvas->GetCanvas()->Update(); 
  _currentBoard = _padeChannel.GetBoardID(); 
  ((TGRadioButton *) _boardList[0])->SetState(kButtonDown); 
  ((TGCheckButton *) _chList[0])->SetState(kButtonDown); 
  firstChannel(); 


}


bool waveInterface::nextOverthresholdCh(bool up) { 
  std::cout << "Overthresh starting chit:" << *_chiterator << std::endl; 
  if (up)  { 
    for (Int_t i = 0; i < _event->NPadeChan(); i++) { 
      _padeChannel = _event->GetPadeChan(i);
      if (_padeChannel.GetBoardID() == _currentBoard && _padeChannel.GetChannelNum() == *_chiterator && _padeChannel.GetMax() < _minCount) { 
	_chiterator++; 

	if (_chiterator == _chSet.end()) {
	  _chiterator--; 
	  break; 
	}
      }
      if (_padeChannel.GetBoardID() == _currentBoard && _padeChannel.GetChannelNum() == *_chiterator && _padeChannel.GetMax() >= _minCount) { 
	std::cout << "Channel " << *_chiterator << " Over threshold." << std::endl; 
	return true; 
      }
    }
    return false; 
  }
  else { 
    Int_t i = 0; 
    while (i < _event->NPadeChan()) { 
      _padeChannel = _event->GetPadeChan(i); 
      if (_padeChannel.GetBoardID() == _currentBoard && _padeChannel.GetChannelNum() == *_chiterator)
	break; 
      i++; 
    }

    for (; i >= 0; i--) { 

      _padeChannel = _event->GetPadeChan(i); 
    if (_padeChannel.GetBoardID() == _currentBoard && _padeChannel.GetChannelNum() == *_chiterator && _padeChannel.GetMax() < _minCount) { 

	if (_chiterator != _chSet.begin())
	  _chiterator--; 
	else 
	  return false; 
    }
    if (_padeChannel.GetBoardID() == _currentBoard && _padeChannel.GetChannelNum() == *_chiterator && _padeChannel.GetMax() >= _minCount)
	return true; 
    }

  }
  return false; 
}

void waveInterface::waveformChUpdate() { 
  for (Int_t i = 0; i < _event->NPadeChan(); i++) { 
      _padeChannel = _event->GetPadeChan(i); 
      if (_padeChannel.GetBoardID() == _currentBoard && _padeChannel.GetChannelNum() == *_chiterator && _padeChannel.GetMax() >= _minCount ) {
	std::cout << _padeChannel.GetBoardID() << " " << _padeChannel.GetChannelNum() << std::endl; 
	updateFrame(_currentEntry, i); 
	return; 
      }
  }
  _waveformCanvas->GetCanvas()->Clear(); 
  _waveformCanvas->GetCanvas()->Modified(); 
  _waveformCanvas->GetCanvas()->Update(); 
}

void waveInterface::firstChannel() { 
  _chiterator = _chSet.begin(); 
  waveformChUpdate(); 
}

bool waveInterface::nextChannel() { 
  std::cout << "Next Channel" << std::endl; 
  _chiterator++; 
  if (_chiterator == _chSet.end()) { 
    _chiterator--; 
    return true; 
  }
  if (nextOverthresholdCh(true)) { 
    waveformChUpdate(); 
    return false; 
  }
  else {
    //    std::cout << "No next overthreshold" << std::endl;
    waveformChUpdate(); 
    return true; 
  }
}


void waveInterface::prevChannel() { 
  std::cout << "Prev Channel" << std::endl; 
  if (_chiterator != _chSet.begin())
    _chiterator--; 
  nextOverthresholdCh(false); 
  waveformChUpdate(); 

}



bool waveInterface::nextEntry() { 
  bool finished = true; 
  Int_t maxentries = _eventTree->GetEntries(); 
  std::cout << "Current Entry:" << _currentEntry << " of: " << maxentries << std::endl; 

  if (_currentEntry < maxentries) {
    _currentEntry++; 
    finished = false; 
    firstChannel(); 
  }


  return finished; 
}

void waveInterface::prevEntry() { 
  Int_t maxentries = _eventTree->GetEntries(); 
  std::cout << "Current Entry:" << _currentEntry << " of: " << maxentries << std::endl; 

  if (_currentEntry == 0)
    return; 
  _currentEntry--; 
  firstChannel(); 
  

}
