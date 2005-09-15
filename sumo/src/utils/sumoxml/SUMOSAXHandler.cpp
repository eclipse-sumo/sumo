//---------------------------------------------------------------------------//
//                        SUMOSAXHandler.cpp -
//  The basic SAX-handler for SUMO-files
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.7  2005/09/15 12:22:16  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.6  2005/04/28 09:02:50  dkrajzew
// level3 warnings removed
//
// Revision 1.5  2003/06/19 11:02:48  dkrajzew
// usage of false tag-enums patched
//
// Revision 1.4  2003/06/18 11:25:12  dkrajzew
// new message and error processing: output to user may be a message, warning or an error now; it is reported to a Singleton (MsgHandler); this handler puts it further to output instances. changes: no verbose-parameter needed; messages are exported to singleton
//
// Revision 1.3  2003/04/01 15:29:30  dkrajzew
// errors are now reported to SErrorHandler
//
// Revision 1.2  2003/02/07 10:53:23  dkrajzew
// updated
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
#include <iostream>
#include <utils/xml/AttributesReadingGenericSAX2Handler.h>
#include <utils/xml/XMLBuildingExceptions.h>
#include <utils/common/MsgHandler.h>
#include <utils/convert/TplConvert.h>
#include <utils/convert/ToString.h>
#include <utils/common/FileErrorReporter.h>
#include "SUMOXMLDefinitions.h"
#include "SUMOSAXHandler.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
SUMOSAXHandler::SUMOSAXHandler(const std::string &filetype,
                               const std::string &file)
    : FileErrorReporter(filetype, file),
        AttributesReadingGenericSAX2Handler(sumotags, noSumoTags,
                                            sumoattrs, noSumoAttrs)
{
}


SUMOSAXHandler::~SUMOSAXHandler()
{
}


void
SUMOSAXHandler::warning(const SAXParseException& exception)
{
    MsgHandler::getErrorInstance()->inform(
        string("Warning: ")
        + TplConvert<XMLCh>::_2str(exception.getMessage()));
    MsgHandler::getErrorInstance()->inform(
        string(" (At line/column ")
        + toString<int>(exception.getLineNumber()+1)
        + string("/")
        + toString<int>(exception.getColumnNumber())
        + string(")."));
    throw XMLBuildingException();
}


void
SUMOSAXHandler::error(const SAXParseException& exception)
{
    MsgHandler::getErrorInstance()->inform(
        TplConvert<XMLCh>::_2str(exception.getMessage()));
    MsgHandler::getErrorInstance()->inform(
        string(" (At line/column ")
        + toString<int>(exception.getLineNumber()+1)
        + string("/")
        + toString<int>(exception.getColumnNumber())
        + string(")."));
    throw XMLBuildingException();
}


void
SUMOSAXHandler::fatalError(const SAXParseException& exception)
{
    MsgHandler::getErrorInstance()->inform(
        TplConvert<XMLCh>::_2str(exception.getMessage()));
    MsgHandler::getErrorInstance()->inform(
        string(" (At line/column ")
        + toString<int>(exception.getLineNumber()+1)
        + string("/")
        + toString<int>(exception.getColumnNumber())
        + string(")."));
    throw XMLBuildingException();
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


