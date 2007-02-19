/****************************************************************************/
/// @file    LineReader.cpp
/// @author  Daniel Krajzewicz
/// @date    Fri, 19 Jul 2002
/// @version $Id$
///
// Retrieves a file linewise and reports the lines to a handler.
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
#include <fstream>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <utils/common/UtilExceptions.h>
#include "LineHandler.h"
#include "LineReader.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS

// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;

// ===========================================================================
// method definitions
// ===========================================================================
LineReader::LineReader()
{}


LineReader::LineReader(const std::string &file)
        : _fileName(file),
        _read(0)
{
    _strm.unsetf(ios::skipws);
    setFileName(file);
}

LineReader::~LineReader()
{}

bool
LineReader::hasMore() const
{
    return _rread<_available;
}


void
LineReader::readAll(LineHandler &lh)
{
    while (_rread<_available) {
        if (!readLine(lh)) {
            return;
        }
    }
}


bool
LineReader::readLine(LineHandler &lh)
{
    string toReport;
    bool moreAvailable = true;
    while (toReport.length()==0) {
        size_t idx = _strBuffer.find('\n');
        if (idx==0) {
            _strBuffer = _strBuffer.substr(1);
            _rread++;
            return lh.report("");
        }
        if (idx!=string::npos) {
            toReport = _strBuffer.substr(0, idx);
            _strBuffer = _strBuffer.substr(idx+1);
            _rread += idx+1;
        } else {
            if (_read<_available) {
                _strm.read(_buffer,
                           _available - _read<1024
                           ? _available - _read
                           : 1024);
                size_t noBytes = _available - _read;
                noBytes = noBytes > 1024 ? 1024 : noBytes;
                _strBuffer += string(_buffer, noBytes);
                _read += 1024;
            } else {
                toReport = _strBuffer;
                moreAvailable = false;
                if (toReport=="") {
                    return lh.report(toReport);
                }
            }
        }
    }
    // remove trailing blanks
    int idx = toReport.length()-1;
    while (idx>=0&&toReport.at(idx)<32) {
        idx--;
    }
    if (idx>=0) {
        toReport = toReport.substr(0, idx+1);
    } else {
        toReport = "";
    }
    // give it to the handler
    if (!lh.report(toReport)) {
        return false;
    }
    return moreAvailable;
}


std::string
LineReader::readLine()
{
    string toReport;
    bool moreAvailable = true;
    while (toReport.length()==0&&_strm.good()) {
        size_t idx = _strBuffer.find('\n');
        if (idx==0) {
            _strBuffer = _strBuffer.substr(1);
            _rread++;
            return "";
        }
        if (idx!=string::npos) {
            toReport = _strBuffer.substr(0, idx);
            _strBuffer = _strBuffer.substr(idx+1);
            _rread += idx+1;
        } else {
            if (_read<_available) {
                _strm.read(_buffer,
                           _available - _read<1024
                           ? _available - _read
                           : 1024);
                size_t noBytes = _available - _read;
                noBytes = noBytes > 1024 ? 1024 : noBytes;
                _strBuffer += string(_buffer, noBytes);
                _read += 1024;
            } else {
                toReport = _strBuffer;
                _rread += 1024; // toReport.length()
                moreAvailable = false;
                if (toReport=="") {
                    return toReport;
                }
            }
        }
    }
    if (!_strm.good()) {
        return "";
    }
    // remove trailing blanks
    int idx = toReport.length()-1;
    while (idx>=0&&toReport.at(idx)<32) {
        idx--;
    }
    if (idx>=0) {
        toReport = toReport.substr(0, idx+1);
    } else {
        toReport = "";
    }
    return toReport;
}



std::string
LineReader::getFileName() const
{
    return _fileName;
}

bool
LineReader::setFileName(const std::string &file)
{
    if (_strm.is_open()) {
        _strm.close();
    }
//    _strm.clear();
    _fileName = file;
    _strm.open(file.c_str(), ios::binary);
    _strm.unsetf(ios::skipws);
    _strm.seekg(0, ios::end);
    _available = _strm.tellg();
    _strm.seekg(0, ios::beg);
    _read = 0;
    _rread = 0;
    _strBuffer = "";
    return _strm.good();
}

unsigned long
LineReader::getPosition()
{
    return _rread;
}

void
LineReader::reinit()
{
    if (_strm.is_open()) {
        _strm.close();
    }
    _strm.open(_fileName.c_str(), ios::binary);
    _strm.unsetf(ios::skipws);
    _strm.seekg(0, ios::end);
    _available = _strm.tellg();
    _strm.seekg(0, ios::beg);
    _read = 0;
    _rread = 0;
    _strBuffer = "";
}

void
LineReader::setPos(unsigned long pos)
{
    _strm.seekg(pos, ios::beg);
    _read = pos;
    _rread = pos;
    _strBuffer = "";
}

bool
LineReader::good() const
{
    return _strm.good();
}



/****************************************************************************/

