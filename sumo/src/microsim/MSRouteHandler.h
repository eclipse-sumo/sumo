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
// Revision 1.3  2003/06/18 11:12:51  dkrajzew
// new message and error processing: output to user may be a message, warning or an error now; it is reported to a Singleton (MsgHandler); this handler puts it further to output instances. changes: no verbose-parameter needed; messages are exported to singleton
//
// Revision 1.2  2003/03/20 16:21:12  dkrajzew
// windows eol removed; multiple vehicle emission added
//
// Revision 1.1  2003/02/07 10:41:51  dkrajzew
// updated
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include "MSRoute.h"
#include "MSNet.h"
#include "MSEmitControl.h"
#include <utils/sumoxml/SUMOSAXHandler.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class MSEdge;
class MSVehicle;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * MSRouteHandler
 * MSRouteHandler is the container for routes while they are build until
 * their transfering to the MSNet::RouteDict
 * The result of the operations are single MSNet::Route-instances
 */
class MSRouteHandler : public SUMOSAXHandler
{
public:
    /// standard constructor
    MSRouteHandler(const std::string &file, bool addVehiclesDirectly);

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
    void closeRoute();

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
    void addVehicleType(const Attributes &attrs);

    /** adds the parsed vehicle type */
    void addParsedVehicleType(const std::string &id, const float length,
        const float maxspeed, const float bmax,
        const float dmax, const float sigma);


    /** parses an occured vehicle definition */
    void addVehicle(const Attributes &attrs);

    /** adds the parsed vehicle */
    MSVehicle *addParsedVehicle(const std::string &id,
        const std::string &vtypeid, const std::string &routeid,
        const long &depart, int repNumber, int repOffset);


    /** opens a route for reading */
    void openRoute(const Attributes &attrs);

    /** reads the route elements */
    void addRouteElements(const std::string &name,
        const std::string &chars);

protected:
    /// the emission time of the vehicle read last
    MSNet::Time myLastDepart;

    /// the last vehicle read
    MSVehicle *myLastReadVehicle;

private:
    /// the current route
    MSEdgeVector    *m_pActiveRoute;

    /// the id of the current route
    std::string     m_ActiveId;

    /** information wheter the route shall be kept after being passed
        (otherwise it will be deleted) */
    bool m_IsMultiReferenced;

    /** information whether vehicles shall be directly added to the network
        or kept within the buffer */
    bool myAddVehiclesDirectly;

private:
    /** invalid copy constructor */
    MSRouteHandler(const MSRouteHandler &s);

    /** invalid assignment operator */
    MSRouteHandler &operator=(const MSRouteHandler &s);
};


/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "MSRouteHandler.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:
