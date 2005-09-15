//---------------------------------------------------------------------------//
//                        FileWriter.cpp -
//
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Tue, 29.05.2005
//  copyright            : (C) 2005 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.4  2005/09/15 12:11:05  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.3  2005/07/12 12:43:48  dkrajzew
// code style adapted
//
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
#include <iostream>
#include "FileWriter.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG

/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;

/* =========================================================================
 * member method definitions
 * ======================================================================= */
FileWriter::FileWriter(const std::string &file)
    : myFile(file.c_str())
{
}


FileWriter::~FileWriter()
{
    myFile.close();
}


void
FileWriter::beginStep()
{
}


void
FileWriter::add(const std::string &str)
{
    myFile << str;
}


void
FileWriter::closeStep()
{
    myFile << endl;
}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:

