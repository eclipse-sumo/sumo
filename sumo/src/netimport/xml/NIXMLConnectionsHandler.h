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
// Revision 1.5  2005/10/07 11:41:16  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.4  2005/09/15 12:03:36  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.3  2005/04/27 12:24:42  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.2  2003/06/18 11:17:29  dkrajzew
// new message and error processing: output to user may be a message, warning or an error now; it is reported to a Singleton (MsgHandler); this handler puts it further to output instances. changes: no verbose-parameter needed; messages are exported to singleton
//
// Revision 1.1  2003/02/07 11:16:30  dkrajzew
// names changed
//
// Revision 1.2  2002/10/29 10:34:04  dkrajzew
// log patched
//
// Revision 1.1  2002/10/17 13:28:11  dkrajzew
// initial commit of classes to import connection definitions
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <utils/sumoxml/SUMOSAXHandler.h>

/* =========================================================================
 * class declarations
 * ======================================================================= */
class NBEdge;
class NBEdgeCont;


/* =========================================================================
 * class definitions
 * ======================================================================= */
class NIXMLConnectionsHandler : public SUMOSAXHandler {
public:
    /// standard constructor
    NIXMLConnectionsHandler(NBEdgeCont &ec);

    /// destructor
    ~NIXMLConnectionsHandler();

protected:
    /// The method called by the SAX-handler to parse start tags
    void myStartElement(int element, const std::string &name,
        const Attributes &attrs);

    /// The method called by the SAX-handler to parse intermediate characters
    void myCharacters(int element, const std::string &name,
        const std::string &chars);

    /// The method called by the SAX-handler to parse closing tags
    void myEndElement(int element, const std::string &name);

private:
    // parses a connection when it describes a edge-2-edge relationship
    void parseEdgeBound(const Attributes &attrs, NBEdge *from,
        NBEdge *to);

    // parses a connection when it describes a lane-2-lane relationship
    void parseLaneBound(const Attributes &attrs,NBEdge *from,
        NBEdge *to);

private:
    NBEdgeCont &myEdgeCont;

private:
    /** invalid copy constructor */
    NIXMLConnectionsHandler(const NIXMLConnectionsHandler &s);

    /** invalid assignment operator */
    NIXMLConnectionsHandler &operator=(const NIXMLConnectionsHandler &s);

};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:
