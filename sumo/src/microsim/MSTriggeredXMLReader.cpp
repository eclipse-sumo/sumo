//---------------------------------------------------------------------------//
//                        MSTriggeredXMLReader.cpp -
//  The basic class for classes that read XML-triggers
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
// Revision 1.2  2003/02/07 10:41:50  dkrajzew
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
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <utils/common/SErrorHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/sumoxml/SUMOSAXHandler.h>
#include "MSTriggeredReader.h"
#include "MSTriggeredXMLReader.h"

using namespace std;

MSTriggeredXMLReader::MSTriggeredXMLReader(MSNet &net,
                                           const std::string &filename)
    : MSTriggeredReader(net),
    SUMOSAXHandler("sumo-trigger values", true, true, filename) // !!! (options)
{
}


MSTriggeredXMLReader::~MSTriggeredXMLReader()
{
}


void
MSTriggeredXMLReader::init(MSNet &net)
{
    myParser = XMLReaderFactory::createXMLReader();
    myParser->setFeature(
        XMLString::transcode(
            "http://xml.org/sax/features/namespaces" ), false );
    myParser->setFeature(
        XMLString::transcode(
            "http://apache.org/xml/features/validation/schema" ), false );
    myParser->setFeature(
        XMLString::transcode(
            "http://apache.org/xml/features/validation/schema-full-checking"),
        false );
    myParser->setFeature(
        XMLString::transcode(
            "http://xml.org/sax/features/validation"), false );
    myParser->setFeature(
        XMLString::transcode(
            "http://apache.org/xml/features/validation/dynamic" ), false );
    if(!myParser->parseFirst(_file.c_str(), myToken)) {
        SErrorHandler::add(
            string("Can not read XML-file '") + _file + string("'."));
        throw ProcessError();
    }
    while(!readNextTriggered());
}


bool
MSTriggeredXMLReader::readNextTriggered()
{
    _nextRead = false;
    while(myParser->parseNext(myToken)) {
        if(_nextRead) {
            return true;
        }
    }
    return false;
}



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "MSTriggeredXMLReader.icc"
//#endif

// Local Variables:
// mode:C++
// End:


