/****************************************************************************/
/// @file    StringTokenizer.cpp
/// @author  Daniel Krajzewicz
/// @date    ?
/// @version $Id: $
///
// A java-style StringTokenizer for c++ (stl)
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

#include <string>
#include <vector>
#include <iostream> // !!! debug only
#include "UtilExceptions.h"
#include "StringTokenizer.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// variable definitions
// ===========================================================================
const int StringTokenizer::NEWLINE = -256;
const int StringTokenizer::WHITECHARS = -257;


// ===========================================================================
// method definitions
// ===========================================================================
StringTokenizer::StringTokenizer(std::string tosplit)
        : _tosplit(tosplit), _pos(0)
{
    prepareWhitechar(tosplit);
}


StringTokenizer::StringTokenizer(std::string tosplit, std::string token)
        : _tosplit(tosplit), _pos(0)
{
    if (token.length()==1) {
        prepare(tosplit, token[0]);
    } else {
        prepare(tosplit, token);
    }
}


StringTokenizer::StringTokenizer(std::string tosplit, int special)
        : _tosplit(tosplit), _pos(0)
{
    switch (special) {
    case NEWLINE:
        prepareNewline(tosplit);
        break;
    case WHITECHARS:
        prepareWhitechar(tosplit);
        break;
    default:
        prepare(tosplit, (char) special);
        break;
    }
}


StringTokenizer::~StringTokenizer()
{}

void StringTokenizer::reinit()
{
    _pos = 0;
}

bool StringTokenizer::hasNext()
{
    return _pos!=_starts.size();
}

std::string StringTokenizer::next()
{
    if (_pos>=_starts.size()) {
        throw OutOfBoundsException();
    }
    if (_lengths[_pos]==0) {
        _pos++;
        return "";
    }
    size_t start = _starts[_pos];
    size_t length = _lengths[_pos++];
    return _tosplit.substr(start,length);
}

std::string StringTokenizer::front()
{
    if (_starts.size()==0) {
        throw OutOfBoundsException();
    }
    if (_lengths[0]==0) {
        return "";
    }
    return _tosplit.substr(_starts[0],_lengths[0]);
}

std::string StringTokenizer::get(size_t pos) const
    {
        if (pos>=_starts.size()) {
            throw OutOfBoundsException();
        }
        if (_lengths[pos]==0) {
            return "";
        }
        size_t start = _starts[pos];
        size_t length = _lengths[pos];
        return _tosplit.substr(start, length);
    }


size_t StringTokenizer::size() const
{
    return _starts.size();
}

void StringTokenizer::prepare(const string &tosplit, const string &token)
{
    size_t len = token.length();
    size_t beg = 0;
    while (beg!=string::npos&&beg<tosplit.length()) {
        size_t end = tosplit.find(token, beg);
        _starts.push_back(beg);
        _lengths.push_back(end-beg);
        beg = end;
        if (end!=string::npos) {
            beg += len;
            if (beg==tosplit.length()) {
                _starts.push_back(beg);
                _lengths.push_back(0);
            }
        }
    }
}

void StringTokenizer::prepare(const string &tosplit, char token)
{
    size_t beg = 0;
    while (beg!=string::npos&&beg<tosplit.length()) {
        size_t end = tosplit.find(token, beg);
        _starts.push_back(beg);
        _lengths.push_back(end-beg);
        beg = end;
        if (end!=string::npos) {
            beg++;
            if (beg==tosplit.length()) {
                _starts.push_back(beg);
                _lengths.push_back(0);
            }
        }
    }
}

void StringTokenizer::prepare(const string &tosplit, const string &token, int /*dummy*/)
{
    size_t beg = tosplit.find_first_not_of(token);
    while (beg!=string::npos&&beg<tosplit.length()) {
        size_t end = tosplit.find_first_of(token, beg);
        _starts.push_back(beg);
        _lengths.push_back(end-beg);
        beg = tosplit.find_first_not_of(token, beg);
    }
}

void StringTokenizer::prepareWhitechar(const string &tosplit)
{
    size_t len = tosplit.length();
    size_t beg = 0;
    while (beg<len&&tosplit.at(beg)<=32) {
        beg++;
    }
    while (beg!=string::npos&&beg<len) {
        size_t end = beg;
        while (end<len&&tosplit.at(end)>32) {
            end++;
        }
        _starts.push_back(beg);
        _lengths.push_back(end-beg);
        beg = end;
        while (beg<len&&tosplit.at(beg)<=32) {
            beg++;
        }
    }
}

void StringTokenizer::prepareNewline(const string &tosplit)
{
    size_t len = tosplit.length();
    size_t beg = 0;
    while (beg<len&&(tosplit.at(beg)==13||tosplit.at(beg)==10)) {
        _starts.push_back(beg);
        _lengths.push_back(0);
        beg++;
    }
    while (beg!=string::npos&&beg<len) {
        size_t end = beg;
        while (end<len&&(tosplit.at(end)!=13&&tosplit.at(end)!=10)) {
            end++;
        }
        _starts.push_back(beg);
        _lengths.push_back(end-beg);
        beg = end;
        if (beg==len-1&&(tosplit.at(beg)==13||tosplit.at(beg)==10)) {
            _starts.push_back(beg);
            _lengths.push_back(0);
        }
        beg++;
        while (beg<len&&(tosplit.at(beg)==13||tosplit.at(beg)==10)) {
            _starts.push_back(beg);
            _lengths.push_back(0);
            beg++;
        }
    }
}


std::vector<std::string>
StringTokenizer::getVector()
{
    std::vector<std::string> ret;
    ret.reserve(size());
    while (hasNext()) {
        ret.push_back(next());
    }
    reinit();
    return ret;
}



/****************************************************************************/

