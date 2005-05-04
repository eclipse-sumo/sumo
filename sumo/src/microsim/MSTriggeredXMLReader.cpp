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
// Revision 1.8  2005/05/04 08:32:05  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.7  2004/07/02 09:56:40  dkrajzew
// debugging while implementing the vss visualisation
//
// Revision 1.6  2004/01/26 07:50:43  dkrajzew
// using the xmlhelpers instead of building the parser by the object itself
//
// Revision 1.5  2003/09/23 14:18:15  dkrajzew
// hierarchy refactored; user-friendly implementation
//
// Revision 1.4  2003/09/22 14:56:07  dkrajzew
// base debugging
//
// Revision 1.3  2003/06/18 11:12:51  dkrajzew
// new message and error processing: output to user may be a message,
//  warning or an error now; it is reported to a Singleton (MsgHandler);
//  this handler puts it further to output instances.
//  changes: no verbose-parameter needed; messages are exported to singleton
//
// Revision 1.2  2003/02/07 10:41:50  dkrajzew
// updated
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <string>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <sax/SAXException.hpp>
#include <sax/SAXParseException.hpp>
#include <utils/convert/TplConvert.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/sumoxml/SUMOSAXHandler.h>
#include <utils/common/XMLHelpers.h>
#include "MSEventControl.h"
#include "MSTriggeredReader.h"
#include "MSTriggeredXMLReader.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
MSTriggeredXMLReader::MSTriggeredXMLReader(MSNet &net,
                                           const std::string &filename)
    : MSTriggeredReader(net),
    SUMOSAXHandler("sumo-trigger values", filename),
    myHaveMore(true)
{
}


MSTriggeredXMLReader::~MSTriggeredXMLReader()
{
}


void
MSTriggeredXMLReader::init(MSNet &net)
{
    try {
        myParser = XMLHelpers::getSAXReader(*this);
        if(!myParser->parseFirst(_file.c_str(), myToken)) {
            MsgHandler::getErrorInstance()->inform(
                string("Can not read XML-file '") + _file + string("'."));
            throw ProcessError();
        }
    } catch (SAXException &e) {
        MsgHandler::getErrorInstance()->inform(
            TplConvert<XMLCh>::_2str(e.getMessage()));
        throw ProcessError();
    } catch (XMLException &e) {
        MsgHandler::getErrorInstance()->inform(
            TplConvert<XMLCh>::_2str(e.getMessage()));
        throw ProcessError();
    }

    if(readNextTriggered()) {
        if(_offset<MSNet::getInstance()->getCurrentTimeStep()) {
            _offset = MSNet::getInstance()->getCurrentTimeStep() + 1;
            // !!! Warning?
        }
        MSEventControl::getBeginOfTimestepEvents()->addEvent(
            new MSTriggerCommand(*this), _offset, MSEventControl::ADAPT_AFTER_EXECUTION);
    } else {
        myHaveMore = false;
    }
}


bool
MSTriggeredXMLReader::readNextTriggered()
{
    while(myHaveMore&&myParser->parseNext(myToken)) {
        if(nextRead()) {
            return true;
        }
    }
    myHaveMore = false;
    return false;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


