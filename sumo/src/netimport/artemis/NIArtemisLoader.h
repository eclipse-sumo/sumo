#ifndef NIArtemisLoader_h
#define NIArtemisLoader_h
/***************************************************************************
                          NIArtemisLoader.h
			  A loader artemis-files
                             -------------------
    project              : SUMO
    begin                : Mon, 10 Feb 2003
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
// Revision 1.2  2003/03/12 16:44:47  dkrajzew
// further work on artemis-import
//
// Revision 1.1  2003/03/03 15:00:34  dkrajzew
// initial commit for artemis-import files
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include <map>
#include <netbuild/NBCapacity2Lanes.h>
#include <utils/importio/LineHandler.h>
#include <utils/importio/LineReader.h>
#include <utils/importio/NamedColumnsParser.h>
#include <utils/common/FileErrorReporter.h>

/* =========================================================================
 * class declaration
 * ======================================================================= */
class OptionsCont;

/* =========================================================================
 * class declaration
 * ======================================================================= */
/**
 * NIArtemisLoader
 */
class NIArtemisLoader :
        public FileErrorReporter
{
public:
    /// constructor
    NIArtemisLoader(const std::string &file);

    /// destructor
    ~NIArtemisLoader();

    /// starts the parsing
    void load(OptionsCont &options);

public:
    /**
     * NIArtemisSingleDataTypeParser
     * The class that parses entries of a certain data type, like edges, edge
     * connections etc.
     */
    class NIArtemisSingleDataTypeParser :
            public FileErrorReporter::Child,
            public LineHandler
    {
    public:
        /// constructor
        NIArtemisSingleDataTypeParser(NIArtemisLoader &parent,
            const std::string &dataName);

        /// Destructor
        virtual ~NIArtemisSingleDataTypeParser();

        /// Reads the data-type from the visum-file using the given reader
        bool parse(bool verbose);

        /// Returns the name of the data type
        const std::string &getDataName() const;

        /** @brief LineHandler-interface
            Returns values from the visum-file; Checks whether the data type is over */
        bool report(const std::string &line);

        /// returns the information whether the data is optional
        virtual bool amOptional() const;


    protected:
        /// performs some initialisation if necessary
        virtual void myInitialise();

        /// performs some procedures on closin if necessary
        virtual void myClose();

        /** @brief builds structures from read data
            When this method which must be implemented by derived classes, each
            loading a certain type of visum-data, the line parser contains the
            values of the next data line.
            This method is called only for data of a single data type, without
            the head and the tail */
        virtual void myDependentReport() = 0;

        /** @brief Builds and reports an error
            We had to name it this way, as otherwise it may be ambigous with the
            method from FileErrorReporter */
        void addError2(const std::string &type, const std::string &id,
            const std::string &exception);

    protected:
        /// The line parser to use
        NamedColumnsParser myLineParser;

        /// The name of the parsed data
        std::string myDataName;

        /// Information which line is read
        int myStep;

    private:
        /// Information whether to work in verbose mode
        bool myWorkVerbose;

    };

private:
    /// information whether the parser shall run in verbose mode
    bool _verbose;

    /** @brief definition of the list of known parsers
        (each reads a certain visum-type) */
    typedef std::vector<NIArtemisSingleDataTypeParser*> ParserVector;

    /// list of known parsers
    ParserVector mySingleDataParsers;

};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "NIArtemisLoader.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:
