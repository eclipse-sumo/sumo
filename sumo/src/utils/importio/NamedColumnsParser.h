/****************************************************************************/
/// @file    NamedColumnsParser.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 19 Jul 2002
/// @version $Id: $
///
// A parser to retrieve information from a table with known column
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
#ifndef NamedColumnsParser_h
#define NamedColumnsParser_h
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

#include <map>
#include <string>
#include <utils/common/StringTokenizer.h>


// ===========================================================================
// class definitions
// ===========================================================================
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
class NamedColumnsParser
{
public:
    /// constructor
    NamedColumnsParser();

    /// constructor
    NamedColumnsParser(const std::string &def, const std::string &defDelim=";",
                       const std::string &lineDelim=";", bool chomp=false, bool ignoreCase=true);

    /// destructor
    ~NamedColumnsParser();

    /** @brief reinitialises the parser
     * (does the same like the constructor without the reallocation of the object) */
    void reinit(const std::string &def, const std::string &defDelim=";",
                const std::string &lineDelim=";", bool chomp=false,
                bool ignoreCase=true);

    /// parses the contents of the line
    void parseLine(const std::string &line);

    /** @brief returns the named information
     *
     * throws an UnknownElement when the element was not named during the initialisation
     * throws an OutOfBoundsException when the line was too short and did not contain the item */
    std::string get(const std::string &name, bool prune=false) const;

    /// Returns the information whether the named column is known
    bool know(const std::string &name) const;

private:
    /** returns the map of attribute names to their positions in a table */
    void reinitMap(std::string s, const std::string &delim=";",
                   bool chomp=false);

    void checkPrune(std::string &str, bool prune) const;

private:
    /** the map's definition of column item names to their positions
        within the table */
    typedef std::map<std::string, size_t> PosMap;

    /// the map of column item names to their positions within the table
    PosMap myDefinitionsMap;

    /// the delimiter to split the column items on
    std::string myLineDelimiter;

    /// the contents of the current line
    StringTokenizer myLineParser;

    /// Information whether case insensitive match shal be done
    bool myAmCaseInsensitive;

};


#endif

/****************************************************************************/

