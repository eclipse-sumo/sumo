/****************************************************************************/
/// @file    UtilExceptions.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 17 Dec 2001
/// @version $Id$
///
// Exceptions for used by some utility classes
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef UtilExceptions_h
#define UtilExceptions_h
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <exception>


// ===========================================================================
// class definitions
// ===========================================================================
class UtilException : public std::exception
{
public:
    virtual const std::string &msg() const
    {
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
class InvalidArgument : public UtilException
{
public:
    /** constructor */
    InvalidArgument(const std::string &msg);

    /** destructor */
    ~InvalidArgument() throw();

    /** returns the error description */
    const std::string &msg() const;

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
class ProcessError : public UtilException
{
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
class UnsupportedFeature : public UtilException
{
public:
    /** constructor */
    UnsupportedFeature(const std::string &message);

    /** destructor */
    ~UnsupportedFeature() throw();

    /** returns the message */
    const std::string &msg() const;

private:
    /** the message of the exception */
    std::string _message;

};





/**
 * EmptyData
 * Thrown when data required by a method is missing
 */
class EmptyData : public UtilException
{
public:
    /** constructor */
    EmptyData()
    { }

    /** destructor */
    ~EmptyData() throw()
    { }

};





/**
 * NumberFormatException
 * Thrown when the string that shall be converted into a
 * numerical representation has any other chracters then
 * digits and a dot
 */
class NumberFormatException : UtilException
{
public:
    /** constructor */
    NumberFormatException()
    { }

    /** destructor */
    ~NumberFormatException() throw()
    { }

};






/**
 * OutOfBoundsException
 * Thrown when an array element out of the array's
 * bounderies is accessed
 */
class OutOfBoundsException : UtilException
{
public:
    /** constructor */
    OutOfBoundsException()
    { }

    /** destructor */
    ~OutOfBoundsException() throw()
    { }

};





/**
 * UnknownElement
 * Thrown when a named element is tried to be accesed
 * which is not known to the container
 */
class UnknownElement : UtilException
{
public:
    /** constructor */
    UnknownElement()
    { }

    /** destructor */
    ~UnknownElement() throw()
    { }

};



/**
 * SignalException
 */
class SignalException : UtilException
{
public:
    /** constructor */
    SignalException(int signal) : mySignal(signal)
    { }

    /** destructor */
    ~SignalException() throw()
    { }

    int getSignal()
    {
        return mySignal;
    }

protected:
    int mySignal;

};



class FileBuildError : public UtilException
{
public:
    /** constructor */
    FileBuildError(const std::string &message);

    /** destructor */
    ~FileBuildError() throw();

    /** returns the message */
    const std::string &msg() const;

private:
    /** the message of the exception */
    std::string myMessage;

};



class NetworkError : public UtilException
{
public:
    // constructor
    NetworkError(const std::string &message);

    // destructor
    ~NetworkError() throw();

    // return the exceptions message
    const std::string &msg() const;

private:
    // the exceptions message
    std::string myMessage;
};


#endif

/****************************************************************************/

