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
// Revision 1.2  2003/02/07 10:53:23  dkrajzew
// updated
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <string>
#include <iostream>
#include <utils/xml/AttributesReadingGenericSAX2Handler.h>
#include <utils/convert/TplConvert.h>
#include <utils/common/FileErrorReporter.h>
#include "SUMOXMLDefinitions.h"
#include "SUMOSAXHandler.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
SUMOSAXHandler::SUMOSAXHandler(const std::string &filetype,
                               bool warn, bool verbose,
                               const std::string &file)
    : FileErrorReporter(filetype, file),
        AttributesReadingGenericSAX2Handler(sumotags, noSumoTags,
                                            sumoattrs, noSumoAttrs),
    _warn(warn), _verbose(verbose)
{
}


SUMOSAXHandler::~SUMOSAXHandler()
{
}


void
SUMOSAXHandler::warning(const SAXParseException& exception)
{
    cout << "Warning: "
        << TplConvert<XMLCh>::_2str(exception.getMessage()) << endl;
    cout << " (At line/column " << exception.getLineNumber()+1
        << '/' << exception.getColumnNumber() << ")." << endl;
    _errorOccured = true;
}


void
SUMOSAXHandler::error(const SAXParseException& exception)
{
    cout << "Error: "
        << TplConvert<XMLCh>::_2str(exception.getMessage()) << endl;
    cout << " (At line/column " << exception.getLineNumber()+1
        << '/' << exception.getColumnNumber() << ")." << endl;
    _errorOccured = true;
}


void
SUMOSAXHandler::fatalError(const SAXParseException& exception)
{
    cout << "Error: "
        << TplConvert<XMLCh>::_2str(exception.getMessage()) << endl;
    cout << " (At line/column " << exception.getLineNumber()+1
        << '/' << exception.getColumnNumber() << ")." << endl;
    _errorOccured = true;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "SUMOSAXHandler.icc"
//#endif

// Local Variables:
// mode:C++
// End:


