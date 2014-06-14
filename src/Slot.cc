//-----------------------------------------------------------------------------
// File:    Slot.cc
// Purpose: Models a slot to which a signal can be connected.
// Created: Summer-2002 Harrison B. Prosper
// Updated: 05-Jun-2008 HBP Adapt to CMS
//          14-Apr-2011 HBP use unsigned long
//-----------------------------------------------------------------------------
#include "Python.h"
#include <iostream>
#include <iomanip>
#include "Slot.h"

using namespace std;

// Make this class known to Root
ClassImp(Slot)

// RootCint requires a default constructor

Slot::Slot() {}

Slot::Slot(void* object_, std::string method_)  	   
  : _object(object_),
    _method(method_)
{}

Slot::~Slot() 
{}

// SLOTS

void Slot::handleSignal(int id)
{
  char ip[4] = {"(i)"};

  PyObject* result = PyObject_CallMethod((PyObject*)_object, 
                                         (char*)_method.c_str(), ip, id);
  if ( PyErr_Occurred() ) PyErr_Clear();

  // Decrement reference count. Use XDECREF to ignore NULL references
  
  Py_XDECREF(result);
}

void Slot::handleSignal()
{
  PyObject* result = PyObject_CallMethod((PyObject*)_object, 
                                         (char*)_method.c_str(), NULL);
  if ( PyErr_Occurred() ) PyErr_Clear();

  // Decrement reference count. Use XDECREF to ignore NULL references
  
  Py_XDECREF(result);
}


