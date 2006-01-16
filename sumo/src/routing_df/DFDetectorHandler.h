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
// Revision 1.4  2006/01/16 13:21:28  dkrajzew
// computation of detector types validated for the 'messstrecke'-scenario
//
// Revision 1.3  2006/01/16 10:46:24  dkrajzew
// some initial work on  the dfrouter
//
// Revision 1.2  2006/01/16 08:09:46  dkrajzew
// set proper types; false cvs log removed
//
// Revision 1.1  2006/01/13 16:40:56  ericnicolay
// base classes for the reading of the detectordefs
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
    /// the options (program settings)
    OptionsCont &myOptions;

	DFDetectorCon &myContainer;

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

