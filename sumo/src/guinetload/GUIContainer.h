#ifndef GUIContainer_h
#define GUIContainer_h
//---------------------------------------------------------------------------//
//                        GUIContainer.h -
//  A loading container derived from NLContainer with additional values
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
// Revision 1.3  2003/02/07 10:38:19  dkrajzew
// updated
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <netload/NLContainer.h>
#include <microsim/MSNet.h>
#include <string>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class GUINet;
class NLEdgeControlBuilder;
class OptionsCont;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * As some artifacts used by the simulation differ between the gui- and the
 * normal version, a special builder is needed for both. Most artifacts used
 * by the gui-enabled simulation are simple inheritances from their normal
 * versions, so this container only overrides some methods, especially those
 * that use constructors. Here, instead of MS* classes, GUI* classes are
 * being build.
 */
class GUIContainer : public NLContainer
{
public:
    /// constructor
    GUIContainer(NLEdgeControlBuilder * const edgeBuilder);

    /// destructor
    ~GUIContainer();

    /// builds a GUI-net after the reading of all artifacts
    GUINet *buildGUINet(MSNet::TimeVector dumpMeanDataIntervalls,
        std::string baseNameDumpFiles, const OptionsCont &options);

    /// adds information about the source and the destination junction
    void addSrcDestInfo(const std::string &id, const std::string &from,
        const std::string &to);

    /// adds a lane
    void addLane(const std::string &id, const bool isDepartLane,
        const float maxSpeed, const float length, const float changeUrge);
};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "GUIContainer.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

