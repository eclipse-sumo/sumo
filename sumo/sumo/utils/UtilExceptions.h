#ifndef UtilExceptions_h
#define UtilExceptions_h
/***************************************************************************
                          UtilExceptions.h
			  Exceptions for used by some utility classes
			  (Option...)
                             -------------------
    project              : SUMO
    begin                : Mon, 17 Dec 2001
    copyright            : (C) 2001 by DLR/IVF http://ivf.dlr.de/
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
// $Log$
// Revision 1.1  2002/04/08 07:21:25  traffic
// Initial revision
//
// Revision 2.0  2002/02/14 14:43:29  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.1  2002/02/13 15:48:20  croessel
// Merge between SourgeForgeRelease and tesseraCVS.
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include <exception>

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * InvalidArgument
 * Thrown when an argument was not proper in the current context
 * A message will be supplied
 */
class InvalidArgument : public std::exception {
 private:
    /** the meaning of the argument within the context */
    std::string   _msg;
 public:
    /** constructor */
   InvalidArgument(std::string msg);
    /** destructor */
    ~InvalidArgument();
    /** returns the error description */
    std::string msg();
};

/**
 * ProcessError
 * A general exception type that may be thrown when the process is no
 * longer able to proceed due to any reason. The reason itself is mostly
 * reported before throwing the exception
 */
class ProcessError : public std::exception {
public:
  /** constructor */
   ProcessError();
  /** destructor */
  ~ProcessError();
};

/**
 * UnsupportedFeature
 * An exception taht is thrown when an (yet) unsupported class or 
 * feature shall be instantiated
 */
class UnsupportedFeature : public std::exception {
private:
  /** the message of the exception */
  std::string _message;
public:
  /** constructor */
  UnsupportedFeature(std::string message);
  /** destructor */
  ~UnsupportedFeature();
  /** returns the message */
  std::string message();
};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "UtilExceptions.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

