/****************************************************************************/
/// @file    RONode.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// Base class for nodes used by the router
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
#ifndef RONode_h
#define RONode_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <utils/common/Named.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class RONode
 * @brief Base class for nodes used by the router
 */
class RONode : public Named
{
public:
    /// @brief Constructor
    RONode(const std::string &id) throw();

    /// @brief Destructor
    ~RONode() throw();


private:
    /// @brief Invalidated copy constructor
    RONode(const RONode &src);

    /// @brief Invalidated assignment operator
    RONode &operator=(const RONode &src);

};


#endif

/****************************************************************************/

