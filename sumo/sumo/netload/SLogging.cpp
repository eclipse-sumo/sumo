/***************************************************************************
                          Slogging.cpp
			  container for strings that shall be presented to
			  the user
                             -------------------
    project              : SUMO
    begin                : Mon, 9 Jul 2001
    copyright            : (C) 2001 by DLR/IVF http://ivf.dlr.de/
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
     const char rcsid[] = "$Id$";
}
// $Log$
// Revision 1.3  2002/06/11 13:44:33  dkrajzew
// Windows eol removed
//
// Revision 1.2  2002/04/24 10:38:45  dkrajzew
// Strings are now passed as references
//
// Revision 1.1.1.1  2002/04/08 07:21:25  traffic
// new project name
//
// Revision 2.0  2002/02/14 14:43:26  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.4  2002/02/13 15:40:47  croessel
// Merge between SourgeForgeRelease and tesseraCVS.
//
// Revision 1.1  2001/12/06 13:36:13  traffic
// moved from netbuild
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <vector>
#include <string>
#include <iostream>
#include "SLogging.h"

/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;

/* =========================================================================
 * static member variables definitions
 * ======================================================================= */
SLogging::Messages SLogging::m_Messages;

/* =========================================================================
 * method definitions
 * ======================================================================= */
SLogging::SLogging()
{
}

SLogging::~SLogging()
{
}

void
SLogging::add(const string &msg, bool report, bool newLine)
{
  m_Messages.push_back(msg);
  if(report) {
    cout << msg;
    if(newLine)
      cout << endl;
  }
}

void
SLogging::add(char *msg, bool report, bool newLine)
{
  string str = msg;
  add(str, report, newLine);
}
