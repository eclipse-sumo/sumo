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
// Revision 1.3  2002/06/11 13:43:37  dkrajzew
// Windows eol removed
//
// Revision 1.2  2002/06/10 08:33:23  dkrajzew
// Parsing of strings into other data formats generelized; Options now recognize false numeric values; documentation added
//
// Revision 1.4  2002/06/10 06:54:30  dkrajzew
// Conversion of strings (XML and c-strings) to numerical values generalized; options now recognize false numerical input
//
// Revision 1.3  2002/05/14 04:45:50  dkrajzew
// Bresenham added; some minor changes; windows eol removed
//
// Revision 1.2  2002/04/26 10:08:39  dkrajzew
// Windows eol removed
//
// Revision 1.1.1.1  2002/04/09 14:18:27  dkrajzew
// new version-free project name (try2)
//
// Revision 1.1.1.1  2002/04/09 13:22:01  dkrajzew
// new version-free project name
//
// Revision 1.1.1.1  2002/02/19 15:33:04  traffic
// Initial import as a separate application.
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
 * An exception that is thrown when an (yet) unsupported class or
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

/**
 * EmptyData
 * Thrown when data required by a method is missing
 */
class EmptyData : public std::exception {
public:
    /** constructor */
    EmptyData() { }
    /** destructor */
    ~EmptyData() { }
};

/**
 * NumberFormatException
 * Thrown when the string that shall be converted into a
 * numerical representation has any other chracters then
 * digits and a dot
 */
class NumberFormatException : public std::exception {
public:
    /** constructor */
    NumberFormatException() { }
    /** destructor */
    ~NumberFormatException() { }
};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "UtilExceptions.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

