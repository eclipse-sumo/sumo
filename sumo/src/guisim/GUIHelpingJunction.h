/****************************************************************************/
/// @file    GUIHelpingJunction.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 29.04.2005
/// @version $Id$
///
//	»missingDescription« 
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GUIHelpingJunction_h
#define GUIHelpingJunction_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <microsim/MSJunction.h>

// ===========================================================================
// class declarations
// ===========================================================================
class GUIJunctionWrapper;
class GUIGlObjectStorage;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIHelpingJunction
 */
class GUIHelpingJunction
            : public MSJunction
{
public:
    static void fill(std::vector<GUIJunctionWrapper*> &list,
                     GUIGlObjectStorage &idStorage);

private:
    /// Default constructor.
    GUIHelpingJunction();

    /// Copy constructor.
    GUIHelpingJunction(const GUIHelpingJunction&);

    /// Assignment operator.
    GUIHelpingJunction& operator=(const GUIHelpingJunction&);

    /// Destructor.
    ~GUIHelpingJunction();

};


#endif

/****************************************************************************/

