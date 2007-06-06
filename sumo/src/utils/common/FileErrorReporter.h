/****************************************************************************/
/// @file    FileErrorReporter.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A class that realises easier reports on errors occured while loading
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
#ifndef FileErrorReporter_h
#define FileErrorReporter_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class FileErrorReporter
 * A class for file handlers that allows to generate errors which contain
 * the name of the parsed file.
 */
class FileErrorReporter
{
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
    class Child
    {
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


#endif

/****************************************************************************/

