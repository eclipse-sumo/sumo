#ifndef ROJPEdgeBuilder_h
#define ROJPEdgeBuilder_h
//---------------------------------------------------------------------------//
//                        ROJPEdgeBuilder.h -
//      The builder for jp-edges
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Tue, 20 Jan 2004
//  copyright            : (C) 2004 by Daniel Krajzewicz
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
// Revision 1.1  2004/01/26 06:09:11  dkrajzew
// initial commit for jp-classes
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <router/ROAbstractEdgeBuilder.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class ROEdge;
class RONet;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class ROJPEdgeBuilder
 * This class builds edges that may be used by the junction-percantage
 *  router.
 */
class ROJPEdgeBuilder : public ROAbstractEdgeBuilder {
public:
    /// Constructor
    ROJPEdgeBuilder();

    /// Destructor
    ~ROJPEdgeBuilder();

    /** @brief Builds a jp-edge */
    ROEdge *buildEdge(const std::string &name);

    /** Post process the edges */
    void setTurningDefinitions(RONet &net,
        const std::vector<float> &turn_defs);

private:
    /// The turn definitions
    std::vector<std::string> myNames;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

