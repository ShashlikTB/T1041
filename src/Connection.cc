//-----------------------------------------------------------------------------
// File:    Connection.cc
// Purpose: Model a connection between a Signal and a Slot
// Created: Summer-2002 Harrison B. Prosper
// Updated: 05-Jun-2008 HBP Adapt to CMS
//          09-May-2014 HBP Adapt to CM testbeam 2014
//-----------------------------------------------------------------------------
#include <iostream>
#include <iomanip>
#include "Connection.h"

using namespace std;

Connection::Connection(TQObject* sender_,   std::string signal_, 
                       PyObject* receiver_, std::string method_)
  : _sender  (sender_),
    _signal  (signal_)
{
  _slot = new Slot(receiver_, method_);

  if ( noArgs(_signal) )
    TQObject::Connect(_sender, _signal.c_str(), 
		      "Slot", _slot, "handleSignal()"); 
  
  else
   TQObject::Connect(_sender, _signal.c_str(), 
		     "Slot", _slot, "handleSignal(Int_t)");
}

Connection::~Connection() 
{
  if ( noArgs(_signal) )
    TQObject::Disconnect(_sender, _signal.c_str(), 
			 _slot,"handleSignal()"); 
  else
    TQObject::Disconnect(_sender, _signal.c_str(),
			 _slot, "handleSignal(Int_t)");
  delete _slot;
  _slot = 0;
}

Bool_t Connection::noArgs(string& signal_)
{ return signal_.find("()") < signal_.length(); }
