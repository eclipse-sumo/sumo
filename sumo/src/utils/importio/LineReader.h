/****************************************************************************/
/// @file    LineReader.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 19 Jul 2002
/// @version $Id$
///
// Retrieves a file linewise and reports the lines to a handler.
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
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
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <fstream>
#include <utils/common/UtilExceptions.h>


// ===========================================================================
// class declarations
// ===========================================================================
class LineHandler;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class LineReader
 * @brief Retrieves a file linewise and reports the lines to a handler.
 *
 * This class reads the contents from a file line by line and report them to
 *  a LineHandler-derivate.
 * @see LineHandler
 * @todo No checks are done so far during reading/setting position etc.
 * @todo Should not IOError be thrown if something fails?
 */
class LineReader
{
public:
    /// @brief Constructor
    LineReader() throw();


    /** @brief Constructor
     *
     * Initialises reading from the file with the given name using setFile.
     *
     * @param[in] file The name of the file to open
     * @see setFile
     */
    LineReader(const std::string &file) throw();


    /// @brief Destructor
    ~LineReader() throw();


    /** @brief Returns whether another line may be read (the file was not read completely)
     * @return Whether further reading is possible
     */
    bool hasMore() const throw();


    /** @brief Reads the whole file linewise, reporting every line to the given LineHandler
     *
     * When the LineHandler returns false, the reading will be aborted
     *
     * @param[in] lh The LineHandler to report read lines to
     */
    void readAll(LineHandler &lh) throw(ProcessError);


    /** @brief Reads a single (the next) line from the file and reports it to the given LineHandler
     *
     * When the LineHandler returns false, the reading will be aborted
     *
     * @param[in] lh The LineHandler to report read lines to
     * @return Whether a further line exists
     */
    bool readLine(LineHandler &lh) throw(ProcessError);


    /** @brief Reads a single (the next) line from the file and returns it
     *
     * @return The next line in the file
     */
    std::string readLine() throw();


    /// @brief Closes the reading
    void close() throw();


    /** @brief Returns the name of the used file
     * @return The name of the opened file
     */
    std::string getFileName() const throw();


    /** @brief Reinitialises the reader for reading from the given file
     *
     * Returns false when the file is not readable
     *
     * @param[in] file The name of the file to open
     * @return Whether the file could be opened
     */
    bool setFile(const std::string &file) throw();


    /** @brief Returns the current position within the file
     * @return The current position within the opened file
     */
    unsigned long getPosition() throw();


    /// @brief Reinitialises the reading (of the previous file)
    void reinit() throw();


    /** @brief Sets the current position within the file to the given value
     *
     * @param[in] pos The new position within the file
     */
    void setPos(unsigned long pos) throw();


    /** @brief Returns the information whether the stream is readable
     * @return Whether the file is usable (good())
     */
    bool good() const throw();


private:
    /// @brief the name of the file to read the contents from
    std::string myFileName;

    /// @brief the stream used
    std::ifstream myStrm;

    /// @brief To override MSVC++-bugs, we use an own getline which uses this buffer
    char myBuffer[1024];

    /// @brief a string-buffer
    std::string myStrBuffer;

    /// @brief Information about how many characters were supplied to the LineHandler
    size_t myRead;

    /// @brief Information how many bytes are available within the used file
    size_t myAvailable;

    /// @brief Information how many bytes were read by the reader from the file
    size_t myRread;

};


#endif

/****************************************************************************/

