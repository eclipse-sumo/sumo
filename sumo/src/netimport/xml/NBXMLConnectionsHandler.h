#ifndef NBXMLConnectionsHandler_h
#define NBXMLConnectionsHandler_h
/***************************************************************************
                          NBXMLConnectionsHandler.h
			  Used to parse the XML-descriptions of types given in a XML-format
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
// $Log$
// Revision 1.2  2002/10/29 10:34:04  dkrajzew
// log patched
//
// Revision 1.1  2002/10/17 13:28:11  dkrajzew
// initial commit of classes to import connection definitions
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <utils/sumoxml/SUMOSAXHandler.h>

class NBEdge;

/* =========================================================================
 * class definitions
 * ======================================================================= */
class NBXMLConnectionsHandler : public SUMOSAXHandler {
public:
    /// standard constructor
    NBXMLConnectionsHandler(bool warn, bool verbose);
    /// destructor
    ~NBXMLConnectionsHandler();
protected:
    void myStartElement(int element, const std::string &name,
        const Attributes &attrs);
    void myCharacters(int element, const std::string &name,
        const std::string &chars);
    void myEndElement(int element, const std::string &name);
private:
    void parseEdgeBound(const Attributes &attrs, NBEdge *from,
        NBEdge *to);
    void parseLaneBound(const Attributes &attrs,NBEdge *from,
        NBEdge *to);

private:
    /** invalid copy constructor */
    NBXMLConnectionsHandler(const NBXMLConnectionsHandler &s);
    /** invalid assignment operator */
    NBXMLConnectionsHandler &operator=(const NBXMLConnectionsHandler &s);
};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "NBXMLConnectionsHandler.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

