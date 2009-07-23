/****************************************************************************/
/// @file    GUIHandler.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// The XML-Handler for building networks within the gui-version
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GUIHandler_h
#define GUIHandler_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <netload/NLHandler.h>
#include <utils/common/RGBColor.h>


// ===========================================================================
// class declarations
// ===========================================================================
class AttributeList;
class NLContainer;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIHandler
 * @brief The XML-Handler for building networks within the gui-version
 *
 * This SAX-Handler overrides some methods of the NLHandler, the original
 * network description handler in order to be used together with a GUIContainer
 * and allow instantiations of gui versions of the microsimulation artifacts
 */
class GUIHandler : public NLHandler {
public:
    /// standard constructor
    GUIHandler(const std::string &file,
               MSNet &net,
               NLDetectorBuilder &detBuilder, NLTriggerBuilder &triggerBuilder,
               NLEdgeControlBuilder &edgeBuilder,
               NLJunctionControlBuilder &junctionBuilder,
               NLGeomShapeBuilder &shapeBuilder) throw();

    /// standard destructor
    ~GUIHandler() throw();

protected:
    /// Builds the route by calling the constructor
    MSRoute* buildRoute() throw();

    /// Starts route processing
    void openRoute(const SUMOSAXAttributes &attrs);

private:
    /// The currently parsed route's color
    RGBColor myColor;

private:
    /** invalid copy constructor */
    GUIHandler(const GUIHandler &s);

    /** invalid assignment operator */
    GUIHandler &operator=(const GUIHandler &s);

};


#endif

/****************************************************************************/

