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
// Revision 1.7  2004/02/06 08:45:40  dkrajzew
// debugged some bugs showing up when compiling under linux
//
// Revision 1.6  2004/01/27 10:30:50  dkrajzew
// debugged the usage of a temporary value returned by reference
//
// Revision 1.5  2003/12/04 13:11:42  dkrajzew
// message retrival added
//
// Revision 1.4  2003/06/05 11:54:48  dkrajzew
// class templates applied; documentation added
//
// Revision 1.3  2003/04/04 15:42:42  roessel
// Added throw() to several declarations/definitons.
//
// Revision 1.2  2003/02/07 10:47:17  dkrajzew
// updated
//
// Revision 1.1  2002/10/16 15:09:09  dkrajzew
// initial commit for some utility classes common to most propgrams of the sumo-package
//
// Revision 1.4  2002/06/11 15:58:26  dkrajzew
// windows eol removed
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
// Revision 1.3  2002/04/09 12:20:38  dkrajzew
// Windows-Memoryleak detection changed
//
// Revision 1.2  2002/03/22 10:59:38  dkrajzew
// Memory leak tracing added; ostrstreams replaces by ostringstreams
//
// Revision 1.1.1.1  2002/02/19 15:33:04  traffic
// Initial import as a separate application.
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <exception>
#include "UtilExceptions.h"


/* =========================================================================
 * debugging definitions (MSVC++ only)
 * ======================================================================= */
#ifdef _DEBUG
   #define _CRTDBG_MAP_ALLOC // include Microsoft memory leak detection procedures
   #define _INC_MALLOC	     // exclude standard memory alloc procedures
#endif


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
/* -------------------------------------------------------------------------
 * InvalidArgument - static variables
 * ----------------------------------------------------------------------- */
std::string UtilException::myDefaultMessage = "<no description available>";


/* -------------------------------------------------------------------------
 * InvalidArgument - methods
 * ----------------------------------------------------------------------- */
InvalidArgument::InvalidArgument(const string &msg)
    : _msg(msg)
{
}


InvalidArgument::~InvalidArgument() throw()
{
}


const string &InvalidArgument::msg()
{
    return _msg;
}



/* -------------------------------------------------------------------------
 * ProcessError - methods
 * ----------------------------------------------------------------------- */
ProcessError::ProcessError()
{
}


ProcessError::~ProcessError() throw()
{
}



/* -------------------------------------------------------------------------
 * UnsupportedFeature - methods
 * ----------------------------------------------------------------------- */
UnsupportedFeature::UnsupportedFeature(const string &message)
    : _message(message)
{
}


UnsupportedFeature::~UnsupportedFeature() throw()
{
}


const string &
UnsupportedFeature::msg()
{
    return _message;
}



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "UtilExceptions.icc"
//#endif

// Local Variables:
// mode:C++
// End:

