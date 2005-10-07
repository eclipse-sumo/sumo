#ifndef ROWeightsHandler_h
#define ROWeightsHandler_h
//---------------------------------------------------------------------------//
//                        ROWeightsHandler.h -
//  A SAX-handler for loading SUMO-weights (aggregated dumps)
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
// $Log$
// Revision 1.11  2005/10/07 11:42:15  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.10  2005/09/23 06:04:36  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.9  2005/09/15 12:05:11  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.8  2005/07/12 12:39:02  dkrajzew
// edge-based mean data implemented; previous lane-based is now optional
//
// Revision 1.7  2005/05/04 08:55:13  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.6  2004/07/02 09:39:41  dkrajzew
// debugging while working on INVENT; preparation of classes to be derived
//  for an online-routing
//
// Revision 1.5  2004/01/26 08:01:21  dkrajzew
// loaders and route-def types are now renamed in an senseful way;
//  further changes in order to make both new routers work;
//  documentation added
//
// Revision 1.4  2003/06/19 11:00:26  dkrajzew
// usage of false tag-enums patched
//
// Revision 1.3  2003/02/07 10:45:07  dkrajzew
// updated
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

#include <string>
#include <utils/sumoxml/SUMOSAXHandler.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class OptionsCont;
class RONet;
class ROEdge;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class ROWeightsHandler
 * A XML-handler for network weights a simulation generates and saves using
 * the aggregated-output.
 */
class ROWeightsHandler : public SUMOSAXHandler {
public:
    /// Constructor
    ROWeightsHandler(OptionsCont &oc, RONet &net,
        const std::string &file, bool useLanes);

    /// Destructor
    ~ROWeightsHandler();

protected:
    /** the user-impemlented handler method for an opening tag */
    void myStartElement(int element, const std::string &name,
        const Attributes &attrs);

    /** the user-implemented handler method for characters */
    void myCharacters(int element, const std::string &name,
        const std::string &chars);

    /** the user-implemented handler method for a closing tag */
    void myEndElement(int element, const std::string &name);

private:
    /// Parses the information for which time steps the following values are valid
    void parseTimeStep(const Attributes &attrs);

    /// Parses the efforts of an edge for the previously read times
    void parseEdge(const Attributes &attrs);

    /// Parses the efforts of a lane for the previously read times
    void parseLane(const Attributes &attrs);

private:
    /// the programm settings (options/configuration)
    OptionsCont &_options;

    /// the previously build net to store the weights in
    RONet &_net;

    /// the name of the value to extract; called scheme for further extensions
    std::string _scheme;

    /// the begin of the time period that is currently being processed
    SUMOTime _currentTimeBeg;
    /// the end of the time period that is currently being processed
    SUMOTime _currentTimeEnd;

    /// the edge the is currently being processed
    ROEdge *_currentEdge;

    /** table of relationships between the attributes in their written
        representation and their enumeration */
    static Tag  _tags[3];

    /// aggregated value over the lanes read within the current edge
    SUMOReal myAggValue;

    /// The number of lanes read for the current edge
    size_t myNoLanes;

    /// Whether the values shall be read from lanes
    bool myUseLanes;


private:
    /// we made the copy constructor invalid
    ROWeightsHandler(const ROWeightsHandler &src);

    /// we made the assignment operator invalid
    ROWeightsHandler &operator=(const ROWeightsHandler &src);

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

