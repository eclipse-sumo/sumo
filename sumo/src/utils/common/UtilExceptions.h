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
// Revision 1.10  2005/09/15 12:13:08  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.9  2005/05/04 09:10:16  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added; added the information about failure on file opening
//
// Revision 1.8  2004/11/23 10:27:45  dkrajzew
// debugging
//
// Revision 1.7  2004/01/27 10:30:50  dkrajzew
// debugged the usage of a temporary value returned by reference
//
// Revision 1.6  2004/01/26 07:28:29  dkrajzew
// something went wrong with the earlier usage of strings
//
// Revision 1.5  2003/12/04 13:11:42  dkrajzew
// message retrival added
//
// Revision 1.4  2003/06/05 11:54:49  dkrajzew
// class templates applied; documentation added
//
// Revision 1.3  2003/04/04 15:42:41  roessel
// Added throw() to several declarations/definitons.
//
// Revision 1.2  2003/02/07 10:47:17  dkrajzew
// updated
//
// Revision 1.1  2002/10/16 15:09:09  dkrajzew
// initial commit for some utility classes common to most propgrams of the sumo-package
//
// Revision 1.6  2002/07/25 08:53:49  dkrajzew
// exceptions added: OutOfBoundsException & UnknownElement
//
// Revision 1.5  2002/06/11 15:58:25  dkrajzew
// windows eol removed
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
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <string>
#include <exception>


/* =========================================================================
 * class definitions
 * ======================================================================= */
class UtilException : public std::exception {
public:
    virtual const std::string &msg() const {
        return myDefaultMessage;
    }
private:
    static std::string myDefaultMessage;
};


/**
 * InvalidArgument
 * Thrown when an argument was not proper in the current context
 * A message will be supplied
 */
class InvalidArgument : public UtilException {
public:
    /** constructor */
    InvalidArgument(const std::string &msg);

    /** destructor */
    ~InvalidArgument() throw();

    /** returns the error description */
    const std::string &msg();

private:
    /** the meaning of the argument within the context */
    std::string   _msg;

};




/**
 * ProcessError
 * A general exception type that may be thrown when the process is no
 * longer able to proceed due to any reason. The reason itself is mostly
 * reported before throwing the exception
 */
class ProcessError : public UtilException {
public:
    /** constructor */
    ProcessError();

    /** destructor */
    ~ProcessError() throw();

};





/**
 * UnsupportedFeature
 * An exception that is thrown when an (yet) unsupported class or
 * feature shall be instantiated
 */
class UnsupportedFeature : public UtilException {
public:
    /** constructor */
    UnsupportedFeature(const std::string &message);

    /** destructor */
    ~UnsupportedFeature() throw();

    /** returns the message */
    const std::string &msg();

private:
    /** the message of the exception */
    std::string _message;

};





/**
 * EmptyData
 * Thrown when data required by a method is missing
 */
class EmptyData : public UtilException {
public:
    /** constructor */
    EmptyData() { }

    /** destructor */
    ~EmptyData() throw() { }

};





/**
 * NumberFormatException
 * Thrown when the string that shall be converted into a
 * numerical representation has any other chracters then
 * digits and a dot
 */
class NumberFormatException : UtilException {
public:
    /** constructor */
    NumberFormatException() { }

    /** destructor */
    ~NumberFormatException() throw() { }

};






/**
 * OutOfBoundsException
 * Thrown when an array element out of the array's
 * bounderies is accessed
 */
class OutOfBoundsException : UtilException {
public:
    /** constructor */
    OutOfBoundsException() { }

    /** destructor */
    ~OutOfBoundsException() throw() { }

};





/**
 * UnknownElement
 * Thrown when a named element is tried to be accesed
 * which is not known to the container
 */
class UnknownElement : UtilException {
public:
    /** constructor */
    UnknownElement() { }

    /** destructor */
    ~UnknownElement() throw() { }

};



/**
 * SignalException
 */
class SignalException : UtilException {
public:
    /** constructor */
    SignalException(int signal) : mySignal(signal) { }

    /** destructor */
    ~SignalException() throw() { }

    int getSignal() { return mySignal; }

protected:
    int mySignal;

};



class FileBuildError : public UtilException {
public:
    /** constructor */
    FileBuildError(const std::string &message);

    /** destructor */
    ~FileBuildError() throw();

    /** returns the message */
    const std::string &msg();

private:
    /** the message of the exception */
    std::string myMessage;

};




/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:
