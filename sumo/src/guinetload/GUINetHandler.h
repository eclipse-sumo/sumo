#ifndef GUINetHandler_h
#define GUINetHandler_h
//---------------------------------------------------------------------------//
//                        GUINetHandler.h -
//  The XML-Handler for building networks within the gui-version derived
//      from NLHandlerBuilder
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
// Revision 1.13  2005/09/15 11:06:03  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.12  2005/05/04 07:55:28  dkrajzew
// added the possibility to load lane geometries into the non-gui simulation; simulation speedup due to avoiding multiplication with 1;
//
// Revision 1.11  2004/12/16 12:23:37  dkrajzew
// first steps towards a better parametrisation of traffic lights
//
// Revision 1.10  2004/11/23 10:12:27  dkrajzew
// new detectors usage applied
//
// Revision 1.9  2004/07/02 08:38:51  dkrajzew
// changes needed to implement the online-router (class derivation)
//
// Revision 1.8  2004/04/02 11:14:36  dkrajzew
// extended traffic lights are no longer template classes
//
// Revision 1.7  2004/01/26 06:49:06  dkrajzew
// work on detectors: e3-detectors loading and visualisation;
//  variable offsets and lengths for lsa-detectors;
//  coupling of detectors to tl-logics
//
// Revision 1.6  2004/01/12 14:44:30  dkrajzew
// handling of e2-detectors within the gui added
//
// Revision 1.5  2003/09/24 09:54:11  dkrajzew
// bug on building induct loops of an actuated tls within the gui patched
//
// Revision 1.4  2003/07/22 14:58:33  dkrajzew
// changes due to new detector handling
//
// Revision 1.3  2003/07/07 08:13:15  dkrajzew
// first steps towards the usage of a real lane and junction geometry
//  implemented
//
// Revision 1.2  2003/06/18 11:08:05  dkrajzew
// new message and error processing: output to user may be a message, warning
//  or an error now; it is reported to a Singleton (MsgHandler);
//  this handler puts it further to output instances.
//  changes: no verbose-parameter needed; messages are exported to singleton
//
// Revision 1.1  2003/02/07 10:38:19  dkrajzew
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
#include <config.h>
#endif // HAVE_CONFIG_H

#include <netload/NLNetHandler.h>
#include <utils/gfx/RGBColor.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class AttributeList;
class NLContainer;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * This SAX-Handler overrides some methods of the NLNetHandler, the original
 * network description handler in order to be used together with a GUIContainer
 * and allow instantiations of gui versions of the microsimulation artifacts
 */
class GUINetHandler : public NLNetHandler {
public:
    /// standard constructor
    GUINetHandler(const std::string &file,
        MSNet &net,
        NLDetectorBuilder &detBuilder, NLTriggerBuilder &triggerBuilder,
        NLEdgeControlBuilder &edgeBuilder,
        NLJunctionControlBuilder &junctionBuilder,
        NLGeomShapeBuilder &shapeBuilder);

    /// standard destructor
    ~GUINetHandler();

protected:
    /** called on the occurence of the beginning of a tag;
        this method */
    void myStartElement(int element, const std::string &name,
        const Attributes &attrs);

    /** called when simple characters occure; this method */
    void myCharacters(int element, const std::string &name,
        const std::string &chars);

    /** parses an occured vehicle type definition */
    virtual void addVehicleType(const Attributes &attrs);

    /** adds the parsed vehicle type */
    virtual void addParsedVehicleType(const std::string &id,
        const float length, const float maxspeed, const float bmax,
        const float dmax, const float sigma, const RGBColor &c);

    void closeRoute();
    void openRoute(const Attributes &attrs);

private:
    void addJunctionShape(const std::string &chars);


private:
    RGBColor myColor;


private:
    /** invalid copy constructor */
    GUINetHandler(const GUINetHandler &s);

    /** invalid assignment operator */
    GUINetHandler &operator=(const GUINetHandler &s);

};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:

