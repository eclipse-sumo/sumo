#ifndef Types_h
#define Types_h
/***************************************************************************
                          NBTypeCont.h
    A storage for the available types of edges
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
// Revision 1.8  2005/09/15 12:02:45  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.7  2005/04/27 11:48:27  dkrajzew
// level3 warnings removed; made containers non-static
//
// Revision 1.6  2004/01/12 15:25:09  dkrajzew
// node-building classes are now lying in an own folder
//
// Revision 1.5  2004/01/12 15:09:28  dkrajzew
// some work on the documentation
//
// Revision 1.4  2003/07/07 08:22:42  dkrajzew
// some further refinements due to the new 1:N traffic lights and usage of geometry information
//
// Revision 1.3  2003/06/18 11:13:13  dkrajzew
// new message and error processing: output to user may be a message,
//  warning or an error now; it is reported to a Singleton (MsgHandler);
//  this handler puts it further to output instances.
//  changes: no verbose-parameter needed; messages are exported to singleton
//
// Revision 1.2  2003/02/07 10:43:44  dkrajzew
// updated
//
// Revision 1.1  2002/10/16 15:48:13  dkrajzew
// initial commit for net building classes
//
// Revision 1.5  2002/07/25 08:31:42  dkrajzew
// Report methods transfered from loader to the containers
//
// Revision 1.4  2002/06/11 16:00:40  dkrajzew
// windows eol removed; template class definition inclusion depends now
//  on the EXTERNAL_TEMPLATE_DEFINITION-definition
//
// Revision 1.3  2002/05/14 04:42:56  dkrajzew
// new computation flow
//
// Revision 1.2  2002/04/26 10:07:12  dkrajzew
// Windows eol removed; minor double to int conversions removed;
//
// Revision 1.1.1.1  2002/04/09 14:18:27  dkrajzew
// new version-free project name (try2)
//
// Revision 1.1.1.1  2002/04/09 13:22:00  dkrajzew
// new version-free project name
//
// Revision 1.3  2002/04/09 12:23:09  dkrajzew
// Windows-Memoryleak detection changed
//
// Revision 1.2  2002/03/22 10:51:26  dkrajzew
// Clearing of members added
//
// Revision 1.1.1.1  2002/02/19 15:33:04  traffic
// Initial import as a separate application.
//
// Revision 1.1  2001/12/06 13:37:59  traffic
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
#include "nodes/NBNode.h"
#include "NBType.h"
#include "NBJunctionTypesMatrix.h"


/* =========================================================================
 * class definitions
 * ======================================================================= */
class NBTypeCont {
public:
    NBTypeCont();

    ~NBTypeCont();

    /** sets the default values */
    void setDefaults(const std::string &formatName, int defaultNoLanes,
        double defaultSpeed, int defaultPriority);

    /** adds a possible type into the list */
    bool insert(NBType *type);

    /** returns the number of lanes
        (the default value if type does not exist) */
    int getNoLanes(const std::string &type);

    /** returns the maximal velocity on a section/edge in m/s
        (the default value if the type does not exist) */
    double getSpeed(const std::string &type);

    /** returns the priority of the section/edge with the given type
        (the default value if the type does not exist) */
    int getPriority(const std::string &type);

    /** returns the name of the used format */
    std::string getFormatName();

    /** returns the default number of lanes */
    int getDefaultNoLanes();

    /** returns the default speed */
    double getDefaultSpeed();

    /** returns the default priority */
    int getDefaultPriority();

    /** returns the number of known types */
    size_t getNo();

    /** returns the type of the junction between two edges of the given types */
    NBNode::BasicNodeType getJunctionType(int edgetype1, int edgetype2) const;

    /** deletes all types */
    void clear();

    /// reports how many nodes were loaded
    void report();

private:
    /** a container of types, accessed by the string key */
    typedef std::map<std::string, NBType*> TypesCont;

private:
    /** the name of the used format */
    std::string              _formatName;

    /** the default number of lanes of a section/edge */
    int                      _defaultNoLanes;

    /** the default maximal velocity on a section/edge in m/s */
    double                   _defaultSpeed;

    /** the default priority of a section/edge */
    int                      _defaultPriority;

    /** the container of types */
    TypesCont                _types;

    /** the matrix of roads to junction type mappings */
    NBJunctionTypesMatrix    _junctionTypes;

private:
    /** invalid copy constructor */
    NBTypeCont(const NBTypeCont &s);
    /** invalid assignment operator */
    NBTypeCont &operator=(const NBTypeCont &s);
};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:
