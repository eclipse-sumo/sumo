#ifndef NBHelpers_h
#define NBHelpers_h
/***************************************************************************
                          NBHelpers.h
			  Some mathematical helper methods
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
// Revision 1.2  2003/02/07 10:43:44  dkrajzew
// updated
//
// Revision 1.1  2002/10/16 15:48:13  dkrajzew
// initial commit for net building classes
//
// Revision 1.5  2002/07/25 08:34:16  dkrajzew
// conversion to a unique string representation of integer ids added
//
// Revision 1.4  2002/06/11 16:00:40  dkrajzew
// windows eol removed; template class definition inclusion depends now on the EXTERNAL_TEMPLATE_DEFINITION-definition
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
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class NBNode;
class Position2D;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * NBHelpers
 * Some mathmatical methods for the computation of angles
 */
class NBHelpers {
public:
    /** computes the angle of the straight which is described by the two
        coordinates */
    static double angle(double x1, double y1, double x2, double y2);

    /** computes the angle between the first angle and the straigth which
        is described by her coordinates (parameter 2-5) */
    static double relAngle(double angle,
        double x1, double y1,
        double x2, double y2);

    /** computes the relative angle between the two angles */
    static double relAngle(double angle1, double angle2);

    /** normalises angle <-170 and >170 to 180 after the computation with
        "relAngle" */
    static double normRelAngle(double angle1, double angle2);

    /** computes number of a logic's setting possibilities for the given key
        In other words: how many permutation of vehicle wishes are possible on
        this junction */
    static long computeLogicSize(std::string key);

    /** converts the numerical id to its "normal" string representation */
    static std::string normalIDRepresentation(const std::string &id);

    /** returns the distance between both nodes */
    static double distance(NBNode *node1, NBNode *node2);

};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "NBHelpers.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:
