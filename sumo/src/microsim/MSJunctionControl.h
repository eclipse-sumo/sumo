#ifndef MSJunctionControl_H
#define MSJunctionControl_H
/***************************************************************************
                          MSJunctionControl.h  -  Coordinates
                          Junction-operations.
                             -------------------
    begin                : Mon, 12 Mar 2001
    copyright            : (C) 2001 by ZAIK http://www.zaik.uni-koeln.de/AFS
    author               : Christian Roessel
    email                : roessel@zpr.uni-koeln.de
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
// Revision 1.3  2003/02/07 10:41:51  dkrajzew
// updated
//
// Revision 1.2  2002/10/16 16:39:02  dkrajzew
// complete deletion within destructors implemented; clear-operator added for container; global file include
//
// Revision 1.1  2002/10/16 14:48:26  dkrajzew
// ROOT/sumo moved to ROOT/src
//
// Revision 1.1.1.1  2002/04/08 07:21:23  traffic
// new project name
//
// Revision 2.1  2002/02/21 18:45:17  croessel
// Method "printJunctions()" removed.
//
// Revision 2.0  2002/02/14 14:43:15  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.8  2002/02/05 13:51:52  croessel
// GPL-Notice included.
// In *.cpp files also config.h included.
//
// Revision 1.7  2001/12/20 11:58:40  croessel
// using namespace std replaced by std::
// Copy-ctor and assignment-operator moved to private.
//
// Revision 1.6  2001/11/15 17:12:14  croessel
// Outcommented the inclusion of the inline *.iC files. Currently not
// needed.
//
// Revision 1.5  2001/11/14 11:45:54  croessel
// Resolved conflicts which appeared during suffix-change and
// CR-line-end commits.
//
// Revision 1.4  2001/11/14 10:49:07  croessel
// CR-line-end removed.
//
// Revision 1.3  2001/10/23 09:30:50  traffic
// parser bugs removed
//
// Revision 1.2  2001/07/16 12:55:47  croessel
// Changed id type from unsigned int to string. Added string-pointer
// dictionaries and dictionary methods.
//
// Revision 1.1.1.1  2001/07/11 15:51:13  traffic
// new start
//

/* =========================================================================
 * included modules
 * ======================================================================= */
#include <vector>
#include <map>
#include <string>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class MSJunction;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 */
class MSJunctionControl
{
public:
    /// Container for Junctions.
    typedef std::vector< MSJunction* > JunctionCont;

    /// Use this constructor only.
    MSJunctionControl( std::string id, JunctionCont* j );

    /// Destructor.
    ~MSJunctionControl();

    /** resets the requests for all lanes */
    void resetRequests();

    /// Sets the responds
    void setAllowed();

    /** @brief Inserts MSJunctionControl into the static dictionary
        Returns true if the key id isn't already in the dictionary.
        Otherwise returns false (the control is not inserted then). */
    static bool dictionary( std::string id, MSJunctionControl* junControl );

    /** @brief Returns the MSEdgeControl associated to the key id if exists
        otherwise returns 0. */
    static MSJunctionControl* dictionary( std::string id );

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
    MSJunctionControl( const MSJunctionControl& );

    /// Assignment operator.
    MSJunctionControl& operator=( const MSJunctionControl& );
};


/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

//#ifndef DISABLE_INLINE
//#include "MSJunctionControl.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:
