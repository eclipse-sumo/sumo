#ifndef NBType_h
#define NBType_h
/***************************************************************************
                          NBType.h
			  A single edge type
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
// Revision 1.4  2002/06/11 16:00:40  dkrajzew
// windows eol removed; template class definition inclusion depends now on the EXTERNAL_TEMPLATE_DEFINITION-definition
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
// Revision 1.1.1.1  2002/02/19 15:33:04  traffic
// Initial import as a separate application.
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>

/* =========================================================================
 * class declarations
 * ======================================================================= */
/**
 * NBType
 * This class holds all informations about a single type of the sections/edges
 */
class NBType {
private:
  /** the name of the type */
  std::string _name;
  /** the number of lanes of this type */
  int       _noLanes;
  /** the speed on a section/edge of this type */
  double    _speed;
  /** the priority of the edge/section of this type */
  int       _priority;
public:
  /** parameterised constructor */
  NBType(std::string name, int noLanes, double speed, int priority);
  /** destructor */
  ~NBType();
  /** the container may access the private members */
  friend class NBTypeCont;
};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "NBType.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:
