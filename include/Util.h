#ifndef UTIL_H
#define UTIL_H
// -*- C++ -*-

/**
 Description: A class of Root utilities. These functions are placed in a class 
              so that Reflex can handle overloading automatically. This is
	      just a collection of simple boilerplate code to lessen
	      clutter.
 
 Implementation:
     As simple as possible
*/
//
// Original Author:  Harrison B. Prosper
//         Created:  Fri Apr 04 2008
// $Id: root.h,v 1.1.1.1 2011/05/04 13:04:28 prosper Exp $
//
//
//-----------------------------------------------------------------------------
#include <iostream>
#include <vector>
#include <string>
//-----------------------------------------------------------------------------
#include <TGClient.h>
#include <TGWindow.h>
#include <TGListBox.h>
#include <TGFileDialog.h>
//-----------------------------------------------------------------------------

/// Simple wrapper around TGFileDialog.


struct Util
{
  ///
  static
  Pixel_t Color(std::string name);

  ///
  static
  TGLBEntry* GLBEntry(TGListBox* listbox, std::string str, int id, 
                      std::string font="helvetica-medium-r", int fontsize=14);

  bool accumulate;
  bool stealthmode;



};
#endif
