#ifndef NIXMLConnectionsHandler_h
#define NIXMLConnectionsHandler_h
/***************************************************************************
                          NIXMLConnectionsHandler.h
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
// Revision 1.1  2003/02/07 11:16:30  dkrajzew
// names changed
//
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

/* =========================================================================
 * class declarations
 * ======================================================================= */
class NBEdge;

/* =========================================================================
 * class definitions
 * ======================================================================= */
class NIXMLConnectionsHandler : public SUMOSAXHandler {
public:
    /// standard constructor
    NIXMLConnectionsHandler(bool warn, bool verbose);
    /// destructor
    ~NIXMLConnectionsHandler();
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
    NIXMLConnectionsHandler(const NIXMLConnectionsHandler &s);
    /** invalid assignment operator */
    NIXMLConnectionsHandler &operator=(const NIXMLConnectionsHandler &s);
};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "NIXMLConnectionsHandler.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:
