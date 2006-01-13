#ifndef DFDetectorHandler_h
#define DFDetectorHandler_h
//---------------------------------------------------------------------------//
//                        DFDetectorHandler.h -
//  The handler for SUMO-Networks
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
// Revision 1.1  2006/01/13 16:40:56  ericnicolay
// base classes for the reading of the detectordefs
//
// Revision 1.7  2005/10/07 11:42:15  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.6  2005/09/15 12:05:11  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.5  2004/07/02 09:39:41  dkrajzew
// debugging while working on INVENT; preparation of classes to be derived for an online-routing
//
// Revision 1.4  2004/01/26 08:01:10  dkrajzew
// loaders and route-def types are now renamed in an senseful way; further changes in order to make both new routers work; documentation added
//
// Revision 1.3  2003/09/05 15:22:44  dkrajzew
// handling of internal lanes added
//
// Revision 1.2  2003/02/07 10:45:07  dkrajzew
// updated
//
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
#include <utils/options/OptionsCont.h>
#include "dfdetector.h"


/* =========================================================================
 * class declarations
 * ======================================================================= */
//class RONet;
//class OptionsCont;
//class ROEdge;
//class ROAbstractEdgeBuilder;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class DFDetectorHandler
 * SAX2-Handler for SUMO-network loading. As this class is used for both
 *  the dua- and the jp-router, a reference to the edge builder is given.
 */
class DFDetectorHandler : public SUMOSAXHandler {
public:
    /// Constructor
    DFDetectorHandler(OptionsCont &oc, DFDetectorCon &con);

    /// Destrcutor
    virtual ~DFDetectorHandler();

protected:
    /** the user-impemlented handler method for an opening tag */
    void myStartElement(int element, const std::string &name,
        const Attributes &attrs);

    /** the user-implemented handler method for characters */
    void myCharacters(int element, const std::string &name,
        const std::string &chars);

    /** the user-implemented handler method for a closing tag */
    void myEndElement(int element, const std::string &name);

protected:
    /// Parses an edge
    void parseDetector(const Attributes &attrs);

    /// Preallocates edges
    void preallocateEdges(const std::string &chars);

protected:
    /// the options (program settings)
    OptionsCont &_options;

	DFDetectorCon &_con;

    /// the net to store the information into
//    RONet &_net;

    /// the name of the edge/node that is currently processed
    std::string _currentName;

    /// the currently build edge
    //ROEdge *_currentEdge;

    /** @brief An indicator whether the next edge shall be read
        Internal edges are not read by now */
    bool _process;

 //   /// The object used to build of edges of the desired type
   // ROAbstractEdgeBuilder &myEdgeBuilder;

private:
    /// invalidated copy constructor
    DFDetectorHandler(const DFDetectorHandler &src);

    /// invalidated assignment operator
    DFDetectorHandler &operator=(const DFDetectorHandler &src);
};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

