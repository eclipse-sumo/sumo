//---------------------------------------------------------------------------//
//                        FileErrorReporter.cpp -
//  A class that realises easier reports on errors occured while loading
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
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
// Revision 1.3  2003/06/18 11:23:29  dkrajzew
// new message and error processing: output to user may be a message, warning or an error now; it is reported to a Singleton (MsgHandler); this handler puts it further to output instances. changes: no verbose-parameter needed; messages are exported to singleton
//
// Revision 1.2  2003/02/07 10:47:17  dkrajzew
// updated
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H
#include <string>
#include <utils/common/MsgHandler.h>
#include "FileErrorReporter.h"


/* =========================================================================
 * used namepsaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */

FileErrorReporter::Child::Child(FileErrorReporter &parent)
    : myParent(parent)
{
}


FileErrorReporter::Child::~Child()
{
}


void
FileErrorReporter::Child::addError(const std::string &error) const
{
    myParent.addError(error);
}


/* -------------------------------------------------------------------------
 * method definitions
 * ----------------------------------------------------------------------- */
FileErrorReporter::FileErrorReporter(const std::string &filetype,
                                     const std::string &file)
    : _file(file), _filetype(filetype)
{
}


FileErrorReporter::~FileErrorReporter()
{
}

/*
void
FileErrorReporter::addError(const std::string &filetype,
                            const std::string &msg)
{
    SErrorHandler::add(
        string("The ") + filetype + string(" '") + _file
        + string("' is corrupt:"));
    SErrorHandler::add(msg);
}
*/

void
FileErrorReporter::addError(const std::string &msg)
{
    MsgHandler::getErrorInstance()->inform(
        string("The ") + _filetype + string(" '") + _file
        + string("' is corrupt:"));
    MsgHandler::getErrorInstance()->inform(msg);
}


void
FileErrorReporter::setFileName(const std::string &file)
{
    _file = file;
}


const string &
FileErrorReporter::getFileName() const
{
   return _file;
}



const std::string &
FileErrorReporter::getDataType() const
{
    return _filetype;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "FileErrorReporter.icc"
//#endif

// Local Variables:
// mode:C++
// End:


