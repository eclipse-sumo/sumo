#ifndef MSRouteHandler_h
#define MSRouteHandler_h
/***************************************************************************
                          MSRouteHandler.h
              Parser and container for routes during their loading
                             -------------------
    project              : SUMO
    begin                : Mon, 9 Jul 2001
    copyright            : (C) 2001 by DLR/IVF http://ivf.dlr.de/
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
// Revision 1.13  2006/01/09 11:57:05  dkrajzew
// bus stops implemented
//
// Revision 1.12  2005/12/01 07:37:35  dkrajzew
// introducing bus stops: eased building vehicles; vehicles may now have nested elements
//
// Revision 1.11  2005/10/07 11:37:45  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.10  2005/09/22 13:45:51  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.9  2005/09/15 11:10:46  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.8  2005/05/04 08:32:05  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.7  2005/02/01 10:10:42  dkrajzew
// got rid of MSNet::Time
//
// Revision 1.6  2004/07/02 09:26:24  dkrajzew
// classes prepared to be derived
//
// Revision 1.5  2003/11/20 13:28:38  dkrajzew
// loading and using of a predefined vehicle color added
//
// Revision 1.4  2003/11/20 13:27:42  dkrajzew
// loading and using of a predefined vehicle color added
//
// Revision 1.3  2003/06/18 11:12:51  dkrajzew
// new message and error processing: output to user may be a message,
//  warning or an error now; it is reported to a Singleton (MsgHandler);
//  this handler puts it further to output instances.
//  changes: no verbose-parameter needed; messages are exported to singleton
//
// Revision 1.2  2003/03/20 16:21:12  dkrajzew
// windows eol removed; multiple vehicle emission added
//
// Revision 1.1  2003/02/07 10:41:51  dkrajzew
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
#include "MSRoute.h"
#include "MSEmitControl.h"
#include "MSVehicle.h"
#include <utils/sumoxml/SUMOSAXHandler.h>
#include <utils/sumoxml/SUMOBaseRouteHandler.h>
#include <utils/common/SUMOTime.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class MSEdge;
class MSVehicle;
class RGBColor;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * MSRouteHandler
 * MSRouteHandler is the container for routes while they are build until
 * their transfering to the MSNet::RouteDict
 * The result of the operations are single MSNet::Route-instances
 */
class MSRouteHandler : public SUMOSAXHandler, public SUMOBaseRouteHandler
{
public:
    /// standard constructor
    MSRouteHandler(const std::string &file, bool addVehiclesDirectly,
        bool wantsVehicleColor);

    /// standard destructor
    virtual ~MSRouteHandler();

    /// opens a route for the addition of edges
    void openRoute(const std::string &id, bool multiReferenced);

    /// adds an edge to the route
    void addEdge(MSEdge *edge);

    /** closes (ends) the building of a route.
        Afterwards no edges may be added to it;
        this method may throw exceptions when
        a) the route is empty or
        b) another route with the same id already exists */
    virtual void closeRoute();

    SUMOTime getLastDepart() const;

    MSVehicle *retrieveLastReadVehicle();

protected:
    /** implementation of the GenericSAXHandler-myStartElement - interface */
    virtual void myStartElement(int element, const std::string &name,
        const Attributes &attrs);

    /** implementation of the GenericSAXHandler-myCharacters - interface */
    virtual void myCharacters(int element, const std::string &name,
        const std::string &chars);

    /** implementation of the GenericSAXHandler-myEndElement - interface */
    virtual void myEndElement(int element, const std::string &name);


    /** parses an occured vehicle type definition */
    virtual void addVehicleType(const Attributes &attrs);

    /** adds the parsed vehicle type */
    virtual void addParsedVehicleType(const std::string &id,
        const SUMOReal length, const SUMOReal maxspeed, const SUMOReal bmax,
        const SUMOReal dmax, const SUMOReal sigma);


    /* parses an occured vehicle definition */
    //void openVehicle(const Attributes &attrs);

    /** adds the parsed vehicle /
    MSVehicle *addParsedVehicle(const std::string &id,
        const std::string &vtypeid, const std::string &routeid,
        const long &depart, int repNumber, int repOffset, RGBColor &c);
*/

    /** opens a route for reading */
    virtual void openRoute(const Attributes &attrs);

    /** reads the route elements */
    void addRouteElements(const std::string &name,
        const std::string &chars);

    void closeVehicle();


protected:
    /// the emission time of the vehicle read last
    SUMOTime myLastDepart;

    /// the last vehicle read
    MSVehicle *myLastReadVehicle;

    /// the current route
    MSEdgeVector myActiveRoute;

    /// the id of the current route
    std::string myActiveRouteID;

    /** information wheter the route shall be kept after being passed
        (otherwise it will be deleted) */
    bool m_IsMultiReferenced;

    /** information whether vehicles shall be directly added to the network
        or kept within the buffer */
    bool myAddVehiclesDirectly;

    /** information whether colors of vehicles shall be loaded */
    bool myWantVehicleColor;

    /// The vehicle's explicite route
    MSRoute *myCurrentEmbeddedRoute;

    /// List of the stops the vehicle will make
    std::vector<MSVehicle::Stop> myVehicleStops;

private:
    /** invalid copy constructor */
    MSRouteHandler(const MSRouteHandler &s);

    /** invalid assignment operator */
    MSRouteHandler &operator=(const MSRouteHandler &s);

};


/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:
