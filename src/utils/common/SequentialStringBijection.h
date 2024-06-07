/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2011-2024 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    SequentialStringBijection.h
/// @author  Jakob Erdmann
/// @date    June 2024
///
// Bidirectional map between string and sequential interger-like-keys (i.e. default enum values)

/****************************************************************************/
#pragma once
#include <config.h>
#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <utils/common/UtilExceptions.h>

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * Template container for maintaining a bidirectional map between strings and something else
 * It is not always a bijection since it allows for duplicate entries on both sides if either
 * checkDuplicates is set to false in the constructor or the insert function or if
 * the addAlias function is used.
 */

class SequentialStringBijection {

public:

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4510 4512 4610) // no default constructor and no assignment operator; conflicts with initializer
#endif
    struct Entry {
        const char* str;
        int key;
    };
#ifdef _MSC_VER
#pragma warning(pop)
#endif


    SequentialStringBijection() {}


    SequentialStringBijection(Entry entries[], int terminatorKey, bool checkDuplicates = true) {
        int i = 0;
        myT2String.resize(terminatorKey + 1);
        do {
            insert(entries[i].str, entries[i].key, checkDuplicates);
        } while (entries[i++].key != terminatorKey);
    }


    void insert(const std::string str, int key, bool checkDuplicates = true) {
        if (checkDuplicates) {
            if (has(key)) {
                // cannot use toString(key) because that might create an infinite loop
                throw InvalidArgument("Duplicate key.");
            }
            if (hasString(str)) {
                throw InvalidArgument("Duplicate string '" + str + "'.");
            }
        }
        myString2T[str] = key;
        myT2String[key] = str;
    }


    void addAlias(const std::string str, int key) {
        myString2T[str] = key;
    }


    void remove(const std::string str, int key) {
        myString2T.erase(str);
        myT2String[key] = "";
    }


    int get(const std::string& str) const {
        if (hasString(str)) {
            return myString2T.find(str)->second;
        } else {
            throw InvalidArgument("String '" + str + "' not found.");
        }
    }


    const std::string& getString(int key) const {
        if ((int)myT2String.size() > key) {
            return myT2String[key];
        } else {
            // cannot use toString(key) because that might create an infinite loop
            throw InvalidArgument("Key not found.");
        }
    }


    bool hasString(const std::string& str) const {
        return myString2T.count(str) != 0;
    }


    bool has(int key) const {
        return (int)myT2String.size() > key && myT2String[key] != "";
    }


    int size() const {
        return (int)myString2T.size();
    }


    std::vector<std::string> getStrings() const {
        return myT2String;
    }


    std::vector<int> getValues() const {
        std::vector<int> result;
        for (auto item : myString2T) {
            result.push_back(item.second);
        }
        return result;
    }


    void addKeysInto(std::vector<int>& list) const {
        typename std::map<std::string, int>::const_iterator it; // learn something new every day
        for (it = myString2T.begin(); it != myString2T.end(); it++) {
            list.push_back(it->second);
        }
    }


private:
    std::map<std::string, int> myString2T;
    std::vector<std::string> myT2String;

};
