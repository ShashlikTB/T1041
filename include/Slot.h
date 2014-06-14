#ifndef SLOT_H
#define SLOT_H
//----------------------------------------------------------------------------
// File:    Slot.h
// Purpose: Model a Slot for use in Signal/Slot communication.
// Created: Summer-2002 Harrison B. Prosper
// Updated: 05-Jun-2008 HBP Adapt to CMS
//          14-Apr-2011 HBP changed unsigned long
//----------------------------------------------------------------------------
#include <string>
#include <vector>
#include <string>
#include "TQObject.h"

/**
 */
class Slot : public TQObject
{
private:
  void*  _object;
  std::string _mstr;
  std::string _method;

public:

  /** RootCint requires a default constructor      
   */

  Slot();

  /**
   */
  Slot(void* object, std::string method_);

  /**
   */
  ~Slot();

  /**
  */
  void handleSignal(int id);

  /**
   */
  void handleSignal();

  void*  receiver() const {return _object;}

  const char*    method()   const {return _mstr.c_str();}

  // Make this class known to Root

  ClassDef(Slot,0)
};

#endif
