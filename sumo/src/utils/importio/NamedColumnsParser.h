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
// Revision 1.7  2005/05/04 09:25:27  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.6  2005/04/28 09:02:49  dkrajzew
// level3 warnings removed
//
// Revision 1.5  2004/11/23 10:35:28  dkrajzew
// debugging
//
// Revision 1.4  2003/06/05 14:29:12  dkrajzew
// building problems under Linux patched
//
// Revision 1.3  2003/05/20 09:51:13  dkrajzew
// further work and debugging
//
// Revision 1.2  2003/02/07 10:51:26  dkrajzew
// updated
//
// Revision 1.1  2002/10/16 14:59:13  dkrajzew
// initial commit for classes that handle import functions
//
// Revision 1.1  2002/07/25 08:55:43  dkrajzew
// support for Visum7.5 & Cell import added
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


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
 * @class NamedColumnsParser
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
public:
    /// constructor
    NamedColumnsParser();

    /// constructor
    NamedColumnsParser(const std::string &def, const std::string &defDelim=";",
        const std::string &lineDelim=";", bool chomp=false);

    /// destructor
    ~NamedColumnsParser();

    /** @brief reinitialises the parser
        (does the same like the constructor without the reallocation
        of the object) */
    void reinit(const std::string &def, const std::string &defDelim=";",
        const std::string &lineDelim=";", bool chomp=false);

    /// parses the contents of the line
    void parseLine(const std::string &line);

    /** @brief returns the named information
        throws an UnknownElement when the element was not named during the
            initialisation
        throws an OutOfBoundsException when the line was too
            short and did not contain the item */
    std::string get(const std::string &name, bool prune=false) const;

private:
    /** returns the map of attribute names to their positions in a table */
    void reinitMap(const std::string &s, const std::string &delim=";",
        bool chomp=false);

    void checkPrune(std::string &str, bool prune) const;

private:
    /** the map's definition of column item names to their positions
        within the table */
    typedef std::map<std::string, size_t> PosMap;

    /// the map of column item names to their positions within the table
    PosMap _defMap;

    /// the delimiter to split the column items on
    std::string _lineDelim;

    /// the contents of the current line
    StringTokenizer _line;

};


/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:
