#ifndef RODFEdgeBuilder_h
#define RODFEdgeBuilder_h
/***************************************************************************
                          RODFEdgeBuilder.h
    A builder for edges used by the DFROUTER
                             -------------------
    project              : SUMO
    begin                : Thu, 16.03.2006
    copyright            : (C) 2006 by DLR/IVF http://ivf.dlr.de/
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
// Revision 1.2  2006/03/17 09:04:26  dkrajzew
// class-documentation added/patched
//
// Revision 1.1  2005/12/08 12:59:41  ericnicolay
// *** empty log message ***
//
// Revision 1.4  2005/10/07 11:42:28  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.3  2005/09/15 12:05:23  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.2  2005/05/04 08:57:12  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.1  2004/01/26 06:08:38  dkrajzew
// initial commit for dua-classes
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

#include <router/ROAbstractEdgeBuilder.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class ROEdge;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class RODUAEdgeBuilder
 */
class RODFEdgeBuilder : public ROAbstractEdgeBuilder {
public:
    /// Constructor
    RODFEdgeBuilder();

    /// Destructor
    ~RODFEdgeBuilder();

    /// Builds the dua-edge
    ROEdge *buildEdge(const std::string &name);

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:
