#ifndef NBJunctionLogicCont_h
#define NBJunctionLogicCont_h
/***************************************************************************
                          NBJunctionLogicCont.h
			  Class for the io-ing between junctions (nodes) and the computers
        mass storage system
                             -------------------
    project              : SUMO
    subproject           : netbuilder / netconverter
    begin                : Tue, 20 Nov 2001
    copyright            : (C) 2001 by DLR http://ivf.dlr.de/
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
// Revision 1.4  2005/09/15 12:02:45  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.3  2005/04/27 11:48:25  dkrajzew
// level3 warnings removed; made containers non-static
//
// Revision 1.2  2003/02/07 10:43:44  dkrajzew
// updated
//
// Revision 1.1  2002/10/16 15:48:13  dkrajzew
// initial commit for net building classes
//
// Revision 1.5  2002/06/11 16:00:40  dkrajzew
// windows eol removed; template class definition inclusion depends now on the EXTERNAL_TEMPLATE_DEFINITION-definition
//
// Revision 1.4  2002/06/07 14:58:45  dkrajzew
// Bugs on dead ends and junctions with too few outgoing roads fixed; Comments improved
//
// Revision 1.3  2002/05/14 04:42:55  dkrajzew
// new computation flow
//
// Revision 1.2  2002/04/26 10:07:11  dkrajzew
// Windows eol removed; minor double to int conversions removed;
//
// Revision 1.1.1.1  2002/04/09 14:18:27  dkrajzew
// new version-free project name (try2)
//
// Revision 1.1.1.1  2002/04/09 13:22:00  dkrajzew
// new version-free project name
//
// Revision 1.2  2002/04/09 12:23:09  dkrajzew
// Windows-Memoryleak detection changed
//
// Revision 1.1.1.1  2002/02/19 15:33:04  traffic
// Initial import as a separate application.
//
// Revision 1.1  2001/12/06 13:38:01  traffic
// files for the netbuilder
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

#include <string>
#include <map>
#include <iostream>


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * NBJunctionLogicCont
 * This class tries to load the logic for a given logickey even when
 * key rotation must have been done
 * If this is not possible (no permutation of the logic has been build),
 * The class may be used for saving the logic build by NBRequest under the
 * given key
 */
class NBJunctionLogicCont
{
public:
    NBJunctionLogicCont();
    ~NBJunctionLogicCont();

    /** returns the number of rotations that must be performed to map the key
        onto an existing logic.
        returns a negative number when the key could not be fitted to any of
        the previously saved keys */
    //int try2convert(const std::string &key);

    /// saves the given junction logic bitfield
    void add(const std::string &key, const std::string &xmlDescription);

    /// saves all known logics
    void writeXML(std::ostream &into);

    /// destroys all stored logics
    void clear();

private:
    /// tries to load a logic with the given key (and all of his permutations)
    bool exists(const std::string &key);

private:
    /// definition o the logic-storage: a map from request to respond
    typedef std::map<std::string, std::string> LogicMap;

    /// the storage for the computed logic
    LogicMap _map;

private:
    /** invalid copy constructor */
    NBJunctionLogicCont(const NBJunctionLogicCont &s);

    /** invalid assignment operator */
    NBJunctionLogicCont &operator=(const NBJunctionLogicCont &s);
};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:
