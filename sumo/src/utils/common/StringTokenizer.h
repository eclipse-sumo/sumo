#ifndef StringTokenizer_h
#define StringTokenizer_h
/***************************************************************************
                          StringTokenizer.h
			  A java-style StringTokenizer for c++ (stl)
                             -------------------
    project              : none
    begin                : ?
    copyright            : (C) Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

/***************************************************************************
    Attention!!!
    As one of few, this module is under the
        Lesser GNU General Public Licence
    *********************************************************************
    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.
 ***************************************************************************/
/* =========================================================================
 * included modules
 * ======================================================================= */
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
/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
class StringTokenizer {
public:
    /** identifier for splitting the given string at all newline characters */
    const static int NEWLINE;

    /** identifier for splitting the given string at all whitespace
        characters */
    const static int WHITECHARS;

public:
    /** default constructor */
    StringTokenizer() { }

    /** @brief constructor
        same as StringTokenizer(tosplit, StringTokenizer.WHITECHARS)
        tosplit is the string to split into substrings */
    StringTokenizer(std::string tosplit);

    /** @brief constructor
        the first string will be split at the second string's occurences */
    StringTokenizer(std::string tosplit, std::string token);

    /** @brief constructor
        The second parameter may be a character - the strig will then be
        split at all occurences of this character.
        When StringTokenizer.NEWLINE is used as second parameter, the string
        will be split at all occurences of a newline character (0x0d / 0x0a)
        When StringTokenizer.WHITECHARS is used as second parameter, the
        string will be split at all characters below 0x20
        The behaviour is undefined for other parameter */
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
    size_t size();

    /** returns the first substring without moving the iterator */
    std::string front();

    /** returns the item at the given position */
    std::string get(size_t pos) const;

private:
    /** splits the first string at all occurences of the second */
    void prepare(const std::string &tosplit, const std::string &token);

    /** splits the first string at all occurences of the given char */
    void prepare(const std::string &tosplit, char token);

    /** unsused */
    void prepare(const std::string &tosplit, const std::string &token,
        int dummy);

    /** splits the first string at all occurences of whitechars */
    void prepareWhitechar(const std::string &tosplit);

    /** splits the first string at all occurences of newlines */
    void prepareNewline(const std::string &tosplit);

private:
    /** a list of positions/lengths */
    typedef std::vector<size_t> SizeVector;

    /** the string to split */
    std::string   _tosplit;

    /** the current position in the list of substrings */
    size_t        _pos;

    /** the list of substring starts */
    SizeVector    _starts;

    /** the list of substring lengths */
    SizeVector    _lengths;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "StringTokenizer.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

