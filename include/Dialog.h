#ifndef FILEDIALOG_H
#define FILEDIALOG_H
//-----------------------------------------------------------------------------
// Original Author:  Harrison B. Prosper 
//-----------------------------------------------------------------------------
#include <iostream>
#include <vector>
#include <string>
//-----------------------------------------------------------------------------
#include "TGWindow.h"
#include "TGFileDialog.h"
//-----------------------------------------------------------------------------

/// Simple wrapper around some TG dialogs
class Dialog
{
public:
  
  Dialog(const TGWindow* window, const TGWindow* main);
          
  
  ~Dialog();

  ///
  std::string SelectFile(EFileDialogMode dlg_type=kFDOpen, 
                         std::string inidir=".",
                         std::string inifilename="");

  ///
  
  void SetText(std::string title, 
               std::string text, 
               UInt_t w=600, 
               UInt_t h=300);
  
  ///
  std::string IniDir();

  ///
  std::string GetInput(std::string prompt, std::string defstr="");

private:
  const TGWindow* window_; 
  const TGWindow* main_;
  std::string _filename;
  std::string _inidir;
};
#endif
