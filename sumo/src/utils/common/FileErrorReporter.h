#ifndef FileErrorReporter_h
#define FileErrorReporter_h
//---------------------------------------------------------------------------//
//                        FileErrorReporter.h -
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
// $Log$
// Revision 1.5  2005/04/28 09:02:46  dkrajzew
// level3 warnings removed
//
// Revision 1.4  2004/11/23 10:27:45  dkrajzew
// debugging
//
// Revision 1.3  2003/03/03 15:24:31  dkrajzew
// extra functionality added
//
// Revision 1.2  2003/02/07 10:47:17  dkrajzew
// updated
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <string>

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class FileErrorReporter
 * A class for file handlers that allows to generate errors which contain
 * the name of the parsed file.
 */
class FileErrorReporter {
public:
    /** constructor */
    FileErrorReporter(const std::string &filetype,
        const std::string &file="");

    /** destructor */
    virtual ~FileErrorReporter();

    /** adds an error to the static error handler */
    //void addError(const std::string &filetype, const std::string &msg);

    /** adds an error to the static error handler */
    void addError(const std::string &msg);

    /** sets the name of the parsed file */
    void setFileName(const std::string &file);

    /** returns the name of the parsed file */
    const std::string &getFileName() const;

    /// returns the type of data used by the reporter
    const std::string &getDataType() const;
public:
    class Child {
    public:
        Child(FileErrorReporter &parent);
        ~Child();
        void addError(const std::string &error) const;
    protected:
        FileErrorReporter &myParent;
    };
protected:
    /** the file currently parsed */
    std::string _file;

    /// name of the file type currently parsed
    std::string _filetype;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

