#ifndef NIXMLNodesHandler_h
#define NIXMLNodesHandler_h
/***************************************************************************
                          NIXMLNodesHandler.h
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
// $Log$
// Revision 1.6  2004/08/02 12:44:28  dkrajzew
// using Position2D instead of two doubles
//
// Revision 1.5  2003/07/07 08:33:15  dkrajzew
// further attribute added: 1:N-definition between node and tl; adapted the importer to the new node type description
//
// Revision 1.4  2003/06/18 11:17:29  dkrajzew
// new message and error processing: output to user may be a message, warning or an error now; it is reported to a Singleton (MsgHandler); this handler puts it further to output instances. changes: no verbose-parameter needed; messages are exported to singleton
//
// Revision 1.3  2003/04/09 15:53:27  dkrajzew
// netconvert-changes: further work on Vissim-import, documentation added
//
// Revision 1.2  2003/02/13 15:55:17  dkrajzew
// xml-loaders now use new options
//
// Revision 1.1  2003/02/07 11:16:30  dkrajzew
// names changed
//
// Revision 1.1  2002/10/16 15:45:36  dkrajzew
// initial commit for xml-importing classes
//
// Revision 1.4  2002/06/11 16:00:41  dkrajzew
// windows eol removed; template class definition inclusion depends now on the EXTERNAL_TEMPLATE_DEFINITION-definition
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
// Revision 1.1.1.1  2002/02/19 15:33:04  traffic
// Initial import as a separate application.
//
// Revision 1.1  2001/12/06 13:37:59  traffic
// files for the netbuilder
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <utils/sumoxml/SUMOSAXHandler.h>
#include <utils/geom/Position2D.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class OptionsCont;
class NBNode;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class NIXMLNodesHandler
 * A class that parses nodes from XML-files
 */
class NIXMLNodesHandler
    : public SUMOSAXHandler {

public:
    /// standard constructor
    NIXMLNodesHandler(OptionsCont &options);

    /// Destructor
    ~NIXMLNodesHandler();

protected:
    /// The method called by the SAX-handler to parse start tags
    void myStartElement(int element, const std::string &name,
        const Attributes &attrs);

    /// The method called by the SAX-handler to parse characters
    void myCharacters(int element, const std::string &name,
        const std::string &chars);

    /// The method called by the SAX-handler to parse end tags
    void myEndElement(int element, const std::string &name);

private:
    /** @brief Sets the position of the node
        Returns false when the information about the node's position was not valid */
    bool setPosition(const Attributes &attrs);

    /** @brief Builds the defined traffic light or adds a node to it */
    void processTrafficLightDefinitions(const Attributes &attrs,
        NBNode *currentNode);

private:
    /// A reference to the program's options
    OptionsCont &_options;

    /// The id of the currently parsed node
    std::string myID;

    /// The position of the currently parsed node
    Position2D myPosition;

    /// The (optional) type of the node currently parsed
    std::string myType;

private:
    /** invalid copy constructor */
    NIXMLNodesHandler(const NIXMLNodesHandler &s);

    /** invalid assignment operator */
    NIXMLNodesHandler &operator=(const NIXMLNodesHandler &s);

};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:
