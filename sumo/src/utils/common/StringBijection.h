/****************************************************************************/
/// @file    StringBijection.h
/// @author  Jakob Erdmann
/// @date    Mar 2011
/// @version $Id$
///
// Bijective Container between string and something else
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef StringBijection_h
#define StringBijection_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <iostream>
#include <map>
#include <string>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * Template container for maintaining a bijection between strings and something else
 */

template< class T  > 
class StringBijection {

public:

    struct Entry {
        const char *str;
        const T other;
    };


    StringBijection() {}


    StringBijection(Entry entries[], T terminator) {
        int i = 0;
        do{
            insert(entries[i].str, entries[i].other);
        } while (entries[i++].other != terminator);
    }


    void insert(const std::string str, const T other) {
        myString2T[str] = other;
        myT2String[other] = str.substr();
    }


    T get(const std::string &str) {
        return myString2T[str];
    }


    const std::string getString(const T other) {
        return myT2String[other];
    }


    bool hasString(const std::string &str) {
        return myString2T.count(str) != 0;
    }


    bool has(const T other) {
        return myT2String.count(other) != 0;
    }


    size_t size() const {
        return myString2T.size();
    }


private:
    std::map<std::string, T> myString2T;
    std::map<T, std::string> myT2String;

};

#endif

/****************************************************************************/

