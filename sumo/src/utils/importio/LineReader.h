#ifndef LineReader_h
#define LineReader_h
/***************************************************************************
                          LineReader.h
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
// $Log$
// Revision 1.3  2003/03/12 16:36:13  dkrajzew
// status information retrieval added
//
// Revision 1.2  2003/02/07 10:51:26  dkrajzew
// updated
//
// Revision 1.1  2002/10/16 14:59:13  dkrajzew
// initial commit for classes that handle import functions
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

/* =========================================================================
 * class declarations
 * ======================================================================= */
class LineHandler;

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class LineReader
 * This class reads the contents from a file line by line and report them to
 * a LineHandler-derivate.
 */
class LineReader {
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

    size_t _read, _available, _rread;

};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "LineReader.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:
