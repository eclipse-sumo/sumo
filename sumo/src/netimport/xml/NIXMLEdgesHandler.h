#ifndef NIXMLEdgesHandler_h
#define NIXMLEdgesHandler_h
/***************************************************************************
                          NIXMLEdgesHandler.h
			  Realises the loading of the edges given in a XML-format
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
// Revision 1.9  2005/09/15 12:03:36  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.8  2005/04/27 12:24:42  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.7  2003/07/07 08:32:19  dkrajzew
// adapted the importer to the new lane geometry description
//
// Revision 1.6  2003/06/18 11:17:29  dkrajzew
// new message and error processing: output to user may be a message, warning or an error now; it is reported to a Singleton (MsgHandler); this handler puts it further to output instances. changes: no verbose-parameter needed; messages are exported to singleton
//
// Revision 1.4  2003/05/20 09:44:35  dkrajzew
// some make-up done (splitting large methods)
//
// Revision 1.3  2003/04/09 15:53:26  dkrajzew
// netconvert-changes: further work on Vissim-import, documentation added
//
// Revision 1.2  2003/02/13 15:55:16  dkrajzew
// xml-loaders now use new options
//
// Revision 1.1  2003/02/07 11:16:30  dkrajzew
// names changed
//
// Revision 1.1  2002/10/16 15:45:36  dkrajzew
// initial commit for xml-importing classes
//
// Revision 1.5  2002/06/11 16:00:40  dkrajzew
// windows eol removed; template class definition inclusion depends now on the EXTERNAL_TEMPLATE_DEFINITION-definition
//
// Revision 1.4  2002/06/07 14:58:45  dkrajzew
// Bugs on dead ends and junctions with too few outgoing roads fixed; Comments improved
//
// Revision 1.3  2002/05/14 04:42:57  dkrajzew
// new computation flow
//
// Revision 1.2  2002/04/26 10:07:12  dkrajzew
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
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <utils/geom/Position2DVector.h>
#include <utils/sumoxml/SUMOSAXHandler.h>
#include <netbuild/NBEdge.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class OptionsCont;
class NBNode;
class NBNodeCont;
class NBEdgeCont;
class NBTypeCont;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class NIXMLEdgesHandler
 * A class that parses edges from XML-files
 */
class NIXMLEdgesHandler : public SUMOSAXHandler {
public:
    /// standard constructor
    NIXMLEdgesHandler(NBNodeCont &nc, NBEdgeCont &ec,
		NBTypeCont &tc,OptionsCont &options);

    /// Destructor
    ~NIXMLEdgesHandler();

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
    /// Parses the id from the given attributes
    void setID(const Attributes &attrs);

    /// Parses the name from the given attributes
    void setName(const Attributes &attrs);

    /// Sets the type from the given attributes
    void checkType(const Attributes &attrs);

    /// Sets the speed from the given type or the given attributes
    void setGivenSpeed(const Attributes &attrs);

    /// Sets the number of lanes from the given type or the given attributes
    void setGivenLanes(const Attributes &attrs);

    /// Sets the priority from the given type or the given attributes
    void setGivenPriority(const Attributes &attrs);

    /// Sets the length of the edge, computing it in prior if necessary
    void setLength(const Attributes &attrs);

    /// tries to parse the shape definition
    Position2DVector tryGetShape(const Attributes &attrs);

    /// Tries to set information needed by the nodes
    bool setNodes(const Attributes &attrs);

    /** @brief tries to parse one of the node's positions
        Which position has to be parsed is defined by the given call variables */
    double tryGetPosition(const Attributes &attrs, int tag,
        const std::string &attrName);

    /** @brief Build the enodes in respect to the given parameters
        Returns the information whether the nodes could be build correctly */
    bool insertNodesCheckingCoherence();

    /// Parses the optional information of how to spread the lanes
    NBEdge::LaneSpreadFunction getSpreadFunction(const Attributes &attrs);


private:
    /// A reference to the program's options
    OptionsCont &_options;

    /// The current edge's id
    std::string myCurrentID;

    /// The current edge's name
    std::string myCurrentName;

    /// The current edge's maximum speed
    double myCurrentSpeed;

    /// The current edge's priority
    int myCurrentPriority;

    /// The current edge's number of lanes
    int myCurrentLaneNo;

    /// The current edge's type
    std::string myCurrentType;

    /// The ids of the begin and the end node
    std::string myCurrentBegNodeID, myCurrentEndNodeID;

    /// The positions of the nodes
    double myBegNodeXPos, myBegNodeYPos, myEndNodeXPos, myEndNodeYPos;

    /// The nodes
    NBNode *myFromNode, *myToNode;

    /// The current edge's length
    double myLength;

    /// The shape of the edge
    Position2DVector myShape;

    /// Information about how to spread the lanes
    NBEdge::LaneSpreadFunction myLanesSpread;

    NBNodeCont &myNodeCont;

	NBEdgeCont &myEdgeCont;

	NBTypeCont &myTypeCont;

private:
    /** invalid copy constructor */
    NIXMLEdgesHandler(const NIXMLEdgesHandler &s);

    /** invalid assignment operator */
    NIXMLEdgesHandler &operator=(const NIXMLEdgesHandler &s);

};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:
