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
#include <utils/common/UtilExceptions.h>
#include <utils/common/ToString.h>


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
        const T key;
    };


    StringBijection() {}


    StringBijection(Entry entries[], T terminatorKey) {
        int i = 0;
        do{
            insert(entries[i].str, entries[i].key);
        } while (entries[i++].key != terminatorKey);
    }


    void insert(const std::string str, const T key) {
        myString2T[str] = key;
        myT2String[key] = str;
    }


    T get(const std::string &str) {
        if (hasString(str)) {
            return myString2T[str];
        } else {
            throw new InvalidArgument("String '" + str + "' not found.");
        }
    }


    const std::string getString(const T key) {
        if (has(key)) {
            return myT2String[key];
        } else {
            throw new InvalidArgument("Key '" + toString(key) + "' not found.");
        }
    }


    bool hasString(const std::string &str) {
        return myString2T.count(str) != 0;
    }


    bool has(const T key) {
        return myT2String.count(key) != 0;
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

