/****************************************************************************/
/// @file    LineReader.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 19 Jul 2002
/// @version $Id: $
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
#ifndef LineReader_h
#define LineReader_h
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


// ===========================================================================
// class declarations
// ===========================================================================
class LineHandler;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class LineReader
 * This class reads the contents from a file line by line and report them to
 * a LineHandler-derivate.
 */
class LineReader
{
public:
    /// constructor
    LineReader();

    /// constructor; initialises the reading
    LineReader(const std::string &file);

    /// destructor
    ~LineReader();

    /// returns the information whether another line may be read
    bool hasMore() const;

    /** reads the whole file linewise, reporting every line to the
        given LineHandler;
        When the LineHandler returns false, the reading will be aborted */
    void readAll(LineHandler &lh);

    /** reads a single (the next) line from the file */
    bool readLine(LineHandler &lh);

    /** reads a single (the next) line from the file and returns it */
    std::string readLine();

    /// closes the reading
    void close();

    /// returns the name of the used file
    std::string getFileName() const;

    /** reinitialises the reader for reading from the given file
        return false when the file is not readable */
    bool setFileName(const std::string &file);

    /// returns the current position within the file
    unsigned long getPosition();

    /// reinitialises the reading (of the previous file)
    void reinit();

    /// sets the current position within the file to the given value
    void setPos(unsigned long pos);

    /// Returns the information whether the stream is readable
    bool good() const;

private:
    /// the name of the file to read the contents from
    std::string     _fileName;

    /// the stream used
    std::ifstream    _strm;

    /// ha ha, to override MSVC++-bugs, we use an own getline which uses this buffer
    char _buffer[1024];

    /// a string-buffer
    std::string _strBuffer;

    /// Information about how many characters were supplied to the LineHandler
    size_t _read;

    /// Information how many bytes are available within the used file
    size_t _available;

    /// Information how many bytes were read by the reader from the file
    size_t _rread;

};


#endif

/****************************************************************************/

