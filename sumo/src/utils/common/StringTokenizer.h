/****************************************************************************/
/// @file    StringTokenizer.h
/// @author  Daniel Krajzewicz
/// @date    ?
/// @version $Id$
///
// A java-style StringTokenizer for c++ (stl)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef StringTokenizer_h
#define StringTokenizer_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <vector>

/**
 * StringTokenizer
 * A class similar to the StringTokenizer from Java. It splits a string at
 * the given string or character (or one of the special cases NEWLINE or
 * WHITECHAR) and allows to iterate over the so generated substrings.
 *
 * The normal usage is like this:
 * <pre>
 * StringTokenizer st(CString("This is a line"), ' ');
 * while(st.hasNext())
 *    cout << st.next() << endl;
 * </pre>
 * This would generate the output:
 * <pre>
 * This
 * is
 * a
 * line
 * </pre>
 *
 * There is something to know about the behaviour:
 * When using WHITECHAR, a list of whitechars occuring in  the string to
 * split is regarded as a single divider. All other parameter will use
 * multiple occurences of operators as a list of single divider and the
 * string between them will have a length of zero.
 */
// ===========================================================================
// class definitions
// ===========================================================================
/**
 *
 */
class StringTokenizer {
public:
    /** identifier for splitting the given string at all newline characters */
    static const int NEWLINE;

    /** identifier for splitting the given string at all whitespace
        characters */
    static const int WHITECHARS;

public:
    /** default constructor */
    StringTokenizer() { }

    /** @brief constructor
        same as StringTokenizer(tosplit, StringTokenizer.WHITECHARS)
        tosplit is the string to split into substrings. If the string between two split
        positions is empty, it will not be returned.  */
    StringTokenizer(std::string tosplit);

    /** @brief constructor
        the first string will be split at the second string's occurences.
        If the optional third parameter is true, the string will be split whenever
        a char from the second string occurs. If the string between two split
        positions is empty, it will nevertheless be returned. */
    StringTokenizer(std::string tosplit, std::string token, bool splitAtAllChars=false);

    /** @brief constructor
        When StringTokenizer.NEWLINE is used as second parameter, the string
        will be split at all occurences of a newline character (0x0d / 0x0a)
        When StringTokenizer.WHITECHARS is used as second parameter, the
        string will be split at all characters below 0x20
        All other ints specified as second parameter are casted int o a char
        at which the string will be splitted. */
    StringTokenizer(std::string tosplit, int special);

    /** destructor */
    ~StringTokenizer();

    /** reinitialises the internal iterator */
    void reinit();

    /** returns the information whether further substrings exist */
    bool hasNext();

    /** returns the next substring when it exists. Otherwise the behaviour is
        undefined */
    std::string next();

    /** returns the number of existing substrings */
    size_t size() const;

    /** returns the first substring without moving the iterator */
    std::string front();

    /** returns the item at the given position */
    std::string get(size_t pos) const;

    std::vector<std::string> getVector();

private:
    /** splits the first string at all occurences of the second. If the third parameter is true
        split at all chars given in the second */
    void prepare(const std::string &tosplit, const std::string &token,
                 bool splitAtAllChars);

    /** splits the first string at all occurences of whitechars */
    void prepareWhitechar(const std::string &tosplit);

private:
    /** a list of positions/lengths */
    typedef std::vector<size_t> SizeVector;

    /** the string to split */
    std::string   myTosplit;

    /** the current position in the list of substrings */
    size_t        myPos;

    /** the list of substring starts */
    SizeVector    myStarts;

    /** the list of substring lengths */
    SizeVector   myLengths;

};


#endif

/****************************************************************************/

