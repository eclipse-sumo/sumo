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
// Revision 1.1  2002/10/16 14:59:13  dkrajzew
// initial commit for classes that handle import functions
//
// Revision 1.2  2002/07/25 09:54:08  dkrajzew
// using ios instead of ios_base for seek options now (had problems with linux)
//
// Revision 1.1  2002/07/25 08:55:42  dkrajzew
// support for Visum7.5 & Cell import added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <utils/common/UtilExceptions.h>
#include "LineHandler.h"
#include "LineReader.h"

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
    : _fileName(file), _strm(file.c_str(), ios::binary)
{
    _strm.unsetf (ios::skipws);
}

LineReader::~LineReader()
{
}

bool
LineReader::hasMore() const
{
    return !_strm.eof();
}


void LineReader::readAll(LineHandler &lh)
{
    while(_strm.good()) {
        if(!readLine(lh)) {
            return;
        }
    }
}

bool LineReader::readLine(LineHandler &lh)
{
    string tmp;
    if(getline(_strm, tmp)) {
        size_t idx = tmp.length()-1;
        while(tmp.at(idx)<32) {
            idx--;
        }
        if(!lh.report(tmp.substr(0, idx+1))) {
            return false;
        }
    }
    return true;
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
    _strm.clear();
    _fileName = file;
    _strm.open(file.c_str(), ios::binary);
    _strm.unsetf (ios::skipws);
    _strm.seekg(0, ios::beg);
    return _strm.good();
}

unsigned long
LineReader::getPosition()
{
    return _strm.tellg();
}

void
LineReader::reinit()
{
    if(_strm.is_open()) {
        _strm.close();
    }
    _strm.open(_fileName.c_str(), ios::binary);
    _strm.unsetf (ios::skipws);
}

void
LineReader::setPos(unsigned long pos)
{
    _strm.seekg(pos, ios::beg);
}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "LineReader.icc"
//#endif

// Local Variables:
// mode:C++
// End:
