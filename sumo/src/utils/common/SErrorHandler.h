#ifndef SErrorHandler_h
#define SErrorHandler_h
/***************************************************************************
                          SWrrorHandler.h  -  container for errors that
			  shall be presented to the user
                             -------------------
    project              : SUMO
    begin                : Mon, 9 Jul 2001
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
// Revision 1.3  2003/03/19 08:09:16  dkrajzew
// addition of const char*-typed errors added
//
// Revision 1.2  2003/02/07 10:47:17  dkrajzew
// updated
//
// Revision 1.1  2002/10/16 15:09:09  dkrajzew
// initial commit for some utility classes common to most propgrams of the sumo-package
//
// Revision 1.1  2002/07/25 08:55:43  dkrajzew
// support for Visum7.5 & Cell import added
//
// Revision 1.5  2002/06/11 14:39:26  dkrajzew
// windows eol removed
//
// Revision 1.4  2002/06/11 13:44:35  dkrajzew
// Windows eol removed
//
// Revision 1.3  2002/06/07 14:39:59  dkrajzew
// errors occured while building larger nets and adaption of new netconverting methods debugged
//
// Revision 1.2  2002/04/24 10:38:45  dkrajzew
// Strings are now passed as references
//
// Revision 1.1.1.1  2002/04/08 07:21:25  traffic
// new project name
//
// Revision 2.0  2002/02/14 14:43:26  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.5  2002/02/14 14:37:22  croessel
// GPL-notice added.
//
// Revision 1.4  2002/02/13 15:40:47  croessel
// Merge between SourgeForgeRelease and tesseraCVS.
//
// Revision 1.1  2001/12/06 13:36:13  traffic
// moved from netbuild
//
// Revision 1.4  2001/08/16 12:53:59  traffic
// further exception handling (now validated) and new comments
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include <vector>

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * SErrorHandler
 * The SErrorHandler holds all error messages that shall be presented to the
 * user.
 * The method which adds new error messages to the list is declared static
 * to allow calling it from methods which did not get the reference to the
 * SErrorHandler and so avoid the passing of an ErrorHandler to all
 * error-generating methods.
 */
class SErrorHandler {
public:
    /// standard constructor
    SErrorHandler();

    /// destructor
    ~SErrorHandler();

    /// adds a new error to the list
    static void add(const std::string &error, bool report=true);

    /// adds a new error to the list
    static void add(char *error, bool report=true);

    /// adds a new error to the list
    static void add(const char *error, bool report=true);

    /// lists all errors
    static void print();

    /// sets the fatal-information to true
    static void setFatal();

    /// returns the information if a fatal error occured
    static bool wasFatal();

    /// returns the information if any error occured
    static bool errorOccured();

    /// Clear information whether previous error exist
    static void clearErrorInformation();

private:
    /// definition for the container of saved errors
    typedef std::vector<std::string> Errors;

    /// the list of errors which shall be presented to the user
    static Errors    m_Errors;

    /// information if an error was fatal
    static bool      m_WasFatal;

    /// information wehther an error occured at all
    static bool      m_ErrorOccured;

private:
    /** invalid copy constructor */
    SErrorHandler(const SErrorHandler &s);

    /** invalid assignment operator */
    SErrorHandler &operator=(const SErrorHandler &s);
};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "SErrorHandler.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

