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
// Revision 1.4  2003/09/22 14:56:07  dkrajzew
// base debugging
//
// Revision 1.3  2003/06/18 11:12:51  dkrajzew
// new message and error processing: output to user may be a message, warning or an error now; it is reported to a Singleton (MsgHandler); this handler puts it further to output instances. changes: no verbose-parameter needed; messages are exported to singleton
//
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
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/sumoxml/SUMOSAXHandler.h>
#include "MSEventControl.h"
#include "MSTriggeredReader.h"
#include "MSTriggeredXMLReader.h"

using namespace std;

MSTriggeredXMLReader::MSTriggeredXMLReader(MSNet &net,
                                           const std::string &filename)
    : MSTriggeredReader(net),
    SUMOSAXHandler("sumo-trigger values", filename)
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
    myParser->setContentHandler( this );
    myParser->setErrorHandler( this );
    if(!myParser->parseFirst(_file.c_str(), myToken)) {
        MsgHandler::getErrorInstance()->inform(
            string("Can not read XML-file '") + _file + string("'."));
        throw ProcessError();
    }
    if(readNextTriggered()) {
        MSEventControl::getBeginOfTimestepEvents()->addEvent(
            new MSTriggerCommand(*this), _offset, MSEventControl::ADAPT_AFTER_EXECUTION);
    }
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


