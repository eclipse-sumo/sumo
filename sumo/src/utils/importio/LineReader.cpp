/***************************************************************************
                          LineReader.cpp
                Retrieves a file linewise and reports the lines to a handler.
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
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.11  2005/09/15 12:20:59  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.10  2005/05/04 09:25:27  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.9  2005/04/28 09:02:49  dkrajzew
// level3 warnings removed
//
// Revision 1.8  2004/07/02 09:47:01  dkrajzew
// a simpler API added (should be reworked, subject to change)
//
// Revision 1.7  2004/03/19 13:02:29  dkrajzew
// not reporting one-char-lines bug patched
//
// Revision 1.6  2003/04/01 15:28:56  dkrajzew
// minor changes
//
// Revision 1.5  2003/03/26 12:06:39  dkrajzew
// errors on reading small files in readline debugged
//
// Revision 1.4  2003/03/18 13:23:22  dkrajzew
// windows eol removed
//
// Revision 1.3  2003/03/12 16:36:12  dkrajzew
// status information retrieval added
//
// Revision 1.2  2003/02/07 10:51:26  dkrajzew
// updated
//
// Revision 1.1  2002/10/16 14:59:13  dkrajzew
// initial commit for classes that handle import functions
//
// Revision 1.2  2002/07/25 09:54:08  dkrajzew
// using ios instead of ios_base for seek options now (had problems with linux)
//
// Revision 1.1  2002/07/25 08:55:42  dkrajzew
// support for Visum7.5 & Cell import added
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <utils/common/UtilExceptions.h>
#include "LineHandler.h"
#include "LineReader.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG

/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;

/* =========================================================================
 * method definitions
 * ======================================================================= */
LineReader::LineReader()
{
}


LineReader::LineReader(const std::string &file)
    : _fileName(file),
    _read(0)
{
    _strm.unsetf (ios::skipws);
    setFileName(file);
}

LineReader::~LineReader()
{
}

bool
LineReader::hasMore() const
{
    return _rread<_available;
}


void
LineReader::readAll(LineHandler &lh)
{
    while(_rread<_available) {
        if(!readLine(lh)) {
            return;
        }
    }
}


bool
LineReader::readLine(LineHandler &lh)
{
    string toReport;
    bool moreAvailable = true;
    while(toReport.length()==0) {
        size_t idx = _strBuffer.find('\n');
        if(idx==0) {
            _strBuffer = _strBuffer.substr(1);
            _rread++;
            return lh.report("");
        }
        if(idx!=string::npos) {
            toReport = _strBuffer.substr(0, idx);
            _strBuffer = _strBuffer.substr(idx+1);
            _rread += idx+1;
        } else {
            if(_read<_available) {
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
                if(toReport=="") {
                    return lh.report(toReport);
                }
            }
        }
    }
    // remove trailing blanks
    int idx = toReport.length()-1;
    while(idx>=0&&toReport.at(idx)<32) {
        idx--;
    }
    if(idx>=0) {
        toReport = toReport.substr(0, idx+1);
    } else {
        toReport = "";
    }
    // give it to the handler
    if(!lh.report(toReport)) {
        return false;
    }
    return moreAvailable;
}


std::string
LineReader::readLine()
{
    string toReport;
    bool moreAvailable = true;
    while(toReport.length()==0&&_strm.good()) {
        size_t idx = _strBuffer.find('\n');
        if(idx==0) {
            _strBuffer = _strBuffer.substr(1);
            _rread++;
            return "";
        }
        if(idx!=string::npos) {
            toReport = _strBuffer.substr(0, idx);
            _strBuffer = _strBuffer.substr(idx+1);
            _rread += idx+1;
        } else {
            if(_read<_available) {
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
                if(toReport=="") {
                   return toReport;
                }
            }
        }
    }
    if(!_strm.good()) {
        return "";
    }
    // remove trailing blanks
    int idx = toReport.length()-1;
    while(idx>=0&&toReport.at(idx)<32) {
        idx--;
    }
    if(idx>=0) {
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
    if(_strm.is_open()) {
        _strm.close();
    }
//    _strm.clear();
    _fileName = file;
    _strm.open(file.c_str(), ios::binary);
    _strm.unsetf (ios::skipws);
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
    if(_strm.is_open()) {
        _strm.close();
    }
    _strm.open(_fileName.c_str(), ios::binary);
    _strm.unsetf (ios::skipws);
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


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
