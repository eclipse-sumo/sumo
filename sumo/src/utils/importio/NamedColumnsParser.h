#ifndef NamedColumnsParser_h
#define NamedColumnsParser_h
/***************************************************************************
                          NamedColumnsParser.h
             A parser to retrieve information from a table with known column
             names where the comlumn order may change
                             -------------------
    project              : SUMO
    begin                : Fri, 19 Jul 2002
    copyright            : (C) 2002 by DLR/IVF http://ivf.dlr.de/
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
// Revision 1.1  2002/10/16 14:59:13  dkrajzew
// initial commit for classes that handle import functions
//
// Revision 1.1  2002/07/25 08:55:43  dkrajzew
// support for Visum7.5 & Cell import added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <map>
#include <string>
#include <utils/common/StringTokenizer.h>

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * NamedColumnsParser
 * When initialised, this parser stores the given information about the
 * order of the named elements and allows the retrieval of lines using the
 * names of these elements.
 * Use it like this:
 * - Initialise with "Name;PositionX;PositionY"
 *      (defDelim=default=";")
 *      (lineDelim=default=";")
 * - Parse each line of a table using "parseLine" (parseLine("Dummy;0;0"))
 * - get values using operations like: string posX = get("PositionX");
 */
class NamedColumnsParser {
private:
    /** the map's definition of column item names to their positions
        within the table */
    typedef std::map<std::string, int> PosMap;
    /// the map of column item names to their positions within the table
    PosMap _defMap;
    /// the delimiter to split the column items on
    std::string _lineDelim;
    /// the contents of the current line
    StringTokenizer _line;
public:
    /// constructor
    NamedColumnsParser();
    /// initialising constructor
    NamedColumnsParser(const std::string &def, const std::string &defDelim=";",
        const std::string &lineDelim=";");
    /// destructor
    ~NamedColumnsParser();
    /** reinitialises the parser
        (does the same like the constructor without the reallocation
        of the object) */
    void reinit(const std::string &def, const std::string &defDelim=";",
        const std::string &lineDelim=";");
    /// parses the contents of the line
    void parseLine(const std::string &line);
    /** returns the named information
        throws an UnknownElement when the element was not named during the
            initialisation
        throws an OutOfBoundsException when the line was too
            short and did not contain the item */
    std::string get(const std::string &name);
private:
    /** returns the map of attribute names to their positions in a table */
    void reinitMap(const std::string &s, const std::string &delim=";");
};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "NamedColumnsParser.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:
