//-----------------------------------------------------------------------------
/** Dialogs.cc

 Description: A collection of simple Root utilities. Functions are placed in 
              a class so that Reflex can handle overloading automatically. 
 
 Implementation:
     As simple as possible
*/
// Created: Summer-2008 Harrison B. Prosper
//-----------------------------------------------------------------------------
#include <algorithm>
#include "Util.h"
#include "TRootHelpDialog.h"
#include "TGFont.h"
#include "TGListBox.h"
#include "TGResourcePool.h"


using namespace std;

Pixel_t 
Util::Color(std::string name)
{
  Pixel_t pixel;
  gClient->GetColorByName(name.c_str(), pixel);
  return pixel;
}

TGLBEntry*
Util::GLBEntry(TGListBox* listbox, 
               std::string str, int id, 
               std::string font, int fontsize)
{
  // Create font object

  char fontstr[256];
  sprintf(fontstr,"-adobe-%s-*-*-%d-*-*-*-*-*-iso8859-1", 
          font.c_str(), fontsize);

  const TGFont* ufont = gClient->GetFont(fontstr);
   if (!ufont)
     ufont = gClient->GetResourcePool()->GetDefaultFont();

   // Create graphics context object

   GCValues_t val;
   val.fMask = kGCFont;
   val.fFont = ufont->GetFontHandle();
   TGGC* uGC = gClient->GetGC(&val, kTRUE);

   TGTextLBEntry* entry = new TGTextLBEntry(listbox->GetContainer(), 
                                            new TGString(str.c_str()), 
                                            id, 
                                            uGC->GetGC(), 
                                            ufont->GetFontStruct());
   return (TGLBEntry*)entry;
}
