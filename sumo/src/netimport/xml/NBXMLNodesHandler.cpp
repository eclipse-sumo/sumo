/***************************************************************************
                          NBXMLNodesHandler.h
			  Used to load the XML-description of the nodes given in a
           XML-format
                             -------------------
    project              : SUMO
    subproject           : netbuilder / netconverter
    begin                : Tue, 20 Nov 2001
    copyright            : (C) 2001 by DLR http://ivf.dlr.de/
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
// Revision 1.1  2002/10/16 15:45:36  dkrajzew
// initial commit for xml-importing classes
//
// Revision 1.7  2002/06/21 10:13:28  dkrajzew
// inclusion of .cpp-files in .cpp files removed
//
// Revision 1.6  2002/06/17 15:19:30  dkrajzew
// unreferenced variable declarations removed
//
// Revision 1.5  2002/06/11 16:00:42  dkrajzew
// windows eol removed; template class definition inclusion depends now on the EXTERNAL_TEMPLATE_DEFINITION-definition
//
// Revision 1.4  2002/06/10 06:56:14  dkrajzew
// Conversion of strings (XML and c-strings) to numerical values generalized; options now recognize false numerical input
//
// Revision 1.3  2002/05/14 04:42:57  dkrajzew
// new computation flow
//
// Revision 1.2  2002/04/26 10:07:13  dkrajzew
// Windows eol removed; minor double to int conversions removed;
//
// Revision 1.1.1.1  2002/04/09 14:18:27  dkrajzew
// new version-free project name (try2)
//
// Revision 1.1.1.1  2002/04/09 13:22:00  dkrajzew
// new version-free project name
//
// Revision 1.3  2002/04/09 12:21:25  dkrajzew
// Windows-Memoryleak detection changed
//
// Revision 1.2  2002/03/22 10:50:04  dkrajzew
// Memory leaks debugging added (MSVC++)
//
// Revision 1.1.1.1  2002/02/19 15:33:04  traffic
// Initial import as a separate application.
//
// Revision 1.1  2001/12/06 13:37:59  traffic
// files for the netbuilder
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include <iostream>
#include <sax/HandlerBase.hpp>
#include <sax/AttributeList.hpp>
#include <sax/SAXParseException.hpp>
#include <sax/SAXException.hpp>
#include "NBXMLNodesHandler.h"
#include <utils/sumoxml/SUMOSAXHandler.h>
#include <netbuild/NBNodeCont.h>
#include <utils/sumoxml/SUMOXMLDefinitions.h>
#include <utils/convert/TplConvert.h>
#include <utils/xml/XMLBuildingExceptions.h>

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
NBXMLNodesHandler::NBXMLNodesHandler(bool warn, bool verbose)
    : SUMOSAXHandler(warn, verbose)
{
}


NBXMLNodesHandler::~NBXMLNodesHandler()
{
}



void
NBXMLNodesHandler::myStartElement(int element, const std::string &tag,
                                  const Attributes &attrs)
{
    string id;
    if(tag=="node") {
        try {
            // retrieve the id of the node
            id = getString(attrs, SUMO_ATTR_ID);
            string name = id;
            // retrieve the name of the node
            try {
                name = getString(attrs, SUMO_ATTR_NAME);
            } catch (EmptyData) {
            }

            // retrieve the position of the node
            double x, y;
            x = y = -1.0;
                // retrieve the x-position
            try {
                x = getFloatSecure(attrs, SUMO_ATTR_X, -1);
            } catch (NumberFormatException) {
                addError("xml-nodes - file",
                    string("Not numeric value for X (at tag ID='") + id
                    + string("')."));
            }
                // retrieve the y-position
            try {
                y = getFloatSecure(attrs, SUMO_ATTR_Y, -1);
            } catch (NumberFormatException) {
                addError("xml-nodes - file",
                    string("Not numeric value for Y (at tag ID='") + id
                    + string("')."));
            }
            // check whether the positions are valid
            if(x==-1||y==-1) {
                _errorOccured = true;
                return;
            }
            // insert the node
            if(!NBNodeCont::insert(id, x, y)) {
                addError("xml-nodes - file",
                    string("Duplicate node occured. ID='") + id
                    + string("'"));
            }
        } catch (EmptyData) {
            if(_verbose) {
	            cout << "No id given... Skipping." << endl;
            }
        }
    }
}

void
NBXMLNodesHandler::myCharacters(int element, const std::string &name,
                                const std::string &chars)
{
    myCharactersDump(element, name, chars);
}


void
NBXMLNodesHandler::myEndElement(int element, const std::string &name)
{
    myEndElementDump(element, name);
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NBXMLNodesHandler.icc"
//#endif

// Local Variables:
// mode:C++
// End:

