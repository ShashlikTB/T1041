#ifndef CONNECTION_H
#define CONNECTION_H
/////////////////////////////////////////////////////////////////////////
// File:    Connection.h
// Purpose: Model a connection between a Signal and a Slot.
// Created: 25-Aug-2002 Harrison B. Prosper
// Updated: 05-Jun-2008 HBP Adapt to CMS 
/////////////////////////////////////////////////////////////////////////
//$Revision: 1.1.1.1 $

#include "Slot.h"
#include "TQObject.h"

// This is a hack to get around a limitation of rootcint (HBP)
extern "C" {
  typedef ssize_t Py_ssize_t;

#define _PyObject_HEAD_EXTRA

/* PyObject_HEAD defines the initial segment of every PyObject. */
#define PyObject_HEAD                   \
    _PyObject_HEAD_EXTRA                \
    Py_ssize_t ob_refcnt;               \
    struct _typeobject *ob_type;

  typedef struct _object {
    PyObject_HEAD
  } PyObject;
}

/** Model a signal/slot association.
    Many computing systems use the signal/slot model for associating a signal,
    for example a user interaction with a graphical user interface (GUI), and
    a slot, that is, a procedure or function that is executed in response to 
    the signal. This class is designed to be used with Python. 
    See TheNtupleMaker/scripts/mkntuplecfi.py for an illustration of how to use
    this class with the ROOT GUI classes.
 */
class Connection : public TQObject
{
public:
  Connection() {}

  /** Construct a signal/slott association.
      @param sender - The address of an object able to issue signals 
      (e.g., a TGMainFrame)
      @param signal - The function, typically of the sender, 
      that is to be called when some action triggers it
      (e.g., ``CloseWindow()")
      @param receiver - The address of the object that must respond to the 
      signal
      @param - The function, typically a method of the receiver, that is to
      be called upon the issuing of the associated signal.
   */
  Connection(TQObject* sender_,   std::string signal_, 
             PyObject* receiver_, std::string method_);

  /**
   */
  ~Connection();

  /// True if signal has no arguments.
  bool noArgs(std::string& signal_);

  ///
  TQObject*   sender()   const {return _sender;}

  ///
  std::string signal()   const {return _signal;}

  ///
  void*       receiver() const {return _slot->receiver();}

  ///
  std::string method()   const {return _slot->method();}


private:
  TQObject    *_sender;
  std::string  _signal;
  Slot        *_slot;
};

#endif
