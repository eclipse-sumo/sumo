#ifndef NBJunctionTypesMatrix_h
#define NBJunctionTypesMatrix_h
/***************************************************************************
                          NBJunctionTypesMatrix.h
                          Definition of a junction's type in dependence
                          on the incoming edges
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
// Revision 1.1  2002/10/16 15:48:13  dkrajzew
// initial commit for net building classes
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
// Revision 1.1.1.1  2002/02/19 15:33:04  traffic
// Initial import as a separate application.
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <vector>
#include <string>
#include <map>

/* =========================================================================
 * class declarations
 * ======================================================================= */
/**
 * NBJunctionTypesMatrix
 * A class that stores the relationship between incoming edges and the
 * junction type resulting from their types.
 */
class NBJunctionTypesMatrix {
private:
  /** a container type for edge priority ranges */
  typedef std::vector<std::pair<int, int> > RangeCont;
  /** A container type for the resulting junction types (cross matrix) */
  typedef std::vector<std::string> StringCont;
  /** a map of chars to ints */
  typedef std::map<char, int> CharToVal;
  /** a container for edge priority ranges */
  RangeCont   _ranges;
  /** A container for the resulting junction types (cross matrix)
      The informations are stored as chars:
      't': Traffic Light Junction
      'r': Right-before-Left Junction
      'p': Priority Junction
      'x': no Junction */
  StringCont  _values;
  /** a map of chars to their NBNode-representation */
  CharToVal   _map;
public:
  /** constructor */
  NBJunctionTypesMatrix();
  /** destructor */
  ~NBJunctionTypesMatrix();
  /** returns the type of the junction on the crossing of edges of the given types */
  int getType(int prio1, int prio2);
private:
  /** returns the one-char name of the junction type between the two given ranges */
  char getNameAt(int pos1, int pos2);
  /**
   * priority_finder
   * Searches for the named priority in the range container
   */
  class priority_finder {
  private:
    int _prio;
  public:
    /** constructor */
    explicit priority_finder(int prio) : _prio(prio) { }
    /** the comparing function */
    bool operator() (std::pair<int, int> range) {
      return range.first > range.second ?
        (_prio<=range.first && _prio>=range.second) :
        (_prio>=range.first && _prio<=range.second);
    }
  };
};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "NBJunctionTypesMatrix.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:
