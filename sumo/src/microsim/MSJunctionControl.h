/****************************************************************************/
/// @file    MSJunctionControl.h
/// @author  Christian Roessel
/// @date    Mon, 12 Mar 2001
/// @version $Id$
///
// Junction-operations.
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
#ifndef MSJunctionControl_h
#define MSJunctionControl_h
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <map>
#include <string>


// ===========================================================================
// class declarations
// ===========================================================================
class MSJunction;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 */
class MSJunctionControl
{
public:
    /// Container for Junctions.
    typedef std::vector< MSJunction* > JunctionCont;

    /// Use this constructor only.
    MSJunctionControl(std::string id, JunctionCont* j);

    /// Destructor.
    ~MSJunctionControl();

    /** resets the requests for all lanes */
    void resetRequests();

    /// Sets the responds
    void setAllowed();

    /** @brief Inserts MSJunctionControl into the static dictionary
        Returns true if the key id isn't already in the dictionary.
        Otherwise returns false (the control is not inserted then). */
    static bool dictionary(std::string id, MSJunctionControl* junControl);

    /** @brief Returns the MSEdgeControl associated to the key id if exists
        otherwise returns 0. */
    static MSJunctionControl* dictionary(std::string id);

    /** Clears the dictionary */
    static void clear();

protected:

private:
    /// Unique ID.
    std::string myID;

    /// The net's junctions.
    JunctionCont* myJunctions;

    /// Static dictionary to associate string-ids with objects.
    typedef std::map< std::string, MSJunctionControl* > DictType;
    static DictType myDict;

    /// Default constructor.
    MSJunctionControl();

    /// Copy constructor.
    MSJunctionControl(const MSJunctionControl&);

    /// Assignment operator.
    MSJunctionControl& operator=(const MSJunctionControl&);
};


#endif

/****************************************************************************/

