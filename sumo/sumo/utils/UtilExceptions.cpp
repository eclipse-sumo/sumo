/***************************************************************************
                          UtilExceptions.cpp
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
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.3  2002/06/11 14:38:23  dkrajzew
// windows eol removed
//
// Revision 1.2  2002/06/11 13:43:36  dkrajzew
// Windows eol removed
//
// Revision 1.1.1.1  2002/04/08 07:21:25  traffic
// new project name
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
#include <exception>
#include "UtilExceptions.h"

/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;

/* =========================================================================
 * method definitions
 * ======================================================================= */
/* -------------------------------------------------------------------------
 * InvalidArgument - methods
 * ----------------------------------------------------------------------- */
InvalidArgument::InvalidArgument(string msg) : exception(), _msg(msg) {
}

InvalidArgument::~InvalidArgument() {
}

string InvalidArgument::msg() {
    return _msg;
}



/* -------------------------------------------------------------------------
 * ProcessError - methods
 * ----------------------------------------------------------------------- */
ProcessError::ProcessError() : exception()
{
}

ProcessError::~ProcessError()
{
}



/* -------------------------------------------------------------------------
 * UnsupportedFeature - methods
 * ----------------------------------------------------------------------- */
UnsupportedFeature::UnsupportedFeature(string message) :
  exception(), _message(message)
{
}

UnsupportedFeature::~UnsupportedFeature()
{
}

string UnsupportedFeature::message() {
  return _message;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "UtilExceptions.icc"
//#endif

// Local Variables:
// mode:C++
// End:

