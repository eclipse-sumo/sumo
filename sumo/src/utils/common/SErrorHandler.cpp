/***************************************************************************
                          SErrorHandler.cpp
			  container for errors that shall be presented to
			  the user
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
namespace
{
     const char rcsid[] = "$Id$";
}
// $Log$
// Revision 1.2  2003/02/07 10:47:17  dkrajzew
// updated
//
// Revision 1.1  2002/10/16 15:09:09  dkrajzew
// initial commit for some utility classes common to most propgrams of the sumo-package
//
// Revision 1.1  2002/07/25 08:55:43  dkrajzew
// support for Visum7.5 & Cell import added
//
// Revision 1.5  2002/06/11 14:39:27  dkrajzew
// windows eol removed
//
// Revision 1.4  2002/06/11 13:44:33  dkrajzew
// Windows eol removed
//
// Revision 1.3  2002/05/14 04:54:25  dkrajzew
// Unexisting files are now catched independent to the Xerces-error mechanism; error report generation moved to XMLConvert
//
// Revision 1.2  2002/04/24 10:38:44  dkrajzew
// Strings are now passed as references
//
// Revision 1.1.1.1  2002/04/08 07:21:25  traffic
// new project name
//
// Revision 2.0  2002/02/14 14:43:26  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.4  2002/02/13 15:40:47  croessel
// Merge between SourgeForgeRelease and tesseraCVS.
//
// Revision 1.1  2001/12/06 13:36:12  traffic
// moved from netbuild
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <vector>
#include <string>
#include <iostream>
#include "SErrorHandler.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * static member variables definitions
 * ======================================================================= */
SErrorHandler::Errors SErrorHandler::m_Errors;
bool SErrorHandler::m_WasFatal = false;
bool SErrorHandler::m_ErrorOccured = false;

/* =========================================================================
 * method definitions
 * ======================================================================= */
SErrorHandler::SErrorHandler()
{
}

SErrorHandler::~SErrorHandler()
{
}

void
SErrorHandler::add(const string &error, bool report)
{
    m_Errors.push_back(error);
    if(report) {
    	cerr << error << endl;
    }
    m_ErrorOccured = true;
}

void
SErrorHandler::add(char *error, bool report)
{
    string str = error;
    add(str, report);
}


void
SErrorHandler::print()
{
    for(Errors::iterator i=m_Errors.begin(); i!=m_Errors.end(); i++) {
	    cout << (*i) << endl;
    }
}

void
SErrorHandler::setFatal()
{
    m_WasFatal = true;
}

bool
SErrorHandler::wasFatal()
{
    return m_WasFatal;
}

bool
SErrorHandler::errorOccured()
{
    return m_ErrorOccured;
}


void
SErrorHandler::clearErrorInformation()
{
    m_ErrorOccured = false;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "SErrorHandler.icc"
//#endif

// Local Variables:
// mode:C++
// End:


