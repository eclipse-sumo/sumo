#ifndef NIVisumLoader_h
#define NIVisumLoader_h
/***************************************************************************
                          NIVisumLoader.h
			  A loader visum-files
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
// Revision 1.2  2003/05/20 09:39:14  dkrajzew
// Visum traffic light import added (by Markus Hartinger)
//
// Revision 1.1  2003/02/07 11:14:54  dkrajzew
// updated
//
// Revision 1.1  2002/10/16 15:44:01  dkrajzew
// initial commit for visum importing classes
//
// Revision 1.1  2002/07/25 08:41:45  dkrajzew
// Visum7.5 and Cell import added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include <map>
#include <vector>
#include <netbuild/NBCapacity2Lanes.h>
#include <utils/importio/LineHandler.h>
#include <utils/importio/LineReader.h>
#include <utils/importio/NamedColumnsParser.h>
#include <utils/common/FileErrorReporter.h>
#include "NIVisumTL.h"

/* =========================================================================
 * class declaration
 * ======================================================================= */
class OptionsCont;

/* =========================================================================
 * class declaration
 * ======================================================================= */
/**
 * NIVisumLoader
 * This class parses the given visum file.
 * When the edge definition is before the node and the type definitions, it
 * will be parsed using a second step; otherwise this class parses the file
 * using a single step.
 * Types are loaded optionally.
 */
class NIVisumLoader :
        public FileErrorReporter
{
public:
    typedef std::map<std::string, std::string> VSysTypeNames;
	typedef std::map<std::string, NIVisumTL*> NIVisumTL_Map;
public:
    /// constructor
    NIVisumLoader(const std::string &file, NBCapacity2Lanes capacity2Lanes);

    /// destructor
    ~NIVisumLoader();

    /// starts the parsing
    void load(OptionsCont &options);

public:
    /**
     * NIVisumSingleDataTypeParser
     * The class that parses entries of a certain data type, like edges, edge
     * connections etc.
     */
    class NIVisumSingleDataTypeParser :
    public FileErrorReporter::Child,
            public LineHandler
    {
    public:
        /// constructor
        NIVisumSingleDataTypeParser(NIVisumLoader &parent,
            const std::string &dataName);

        /// Destructor
        virtual ~NIVisumSingleDataTypeParser();

        /** @brief Sets the position of the data-type within the visum-file
            The position is saved as a byte-offset within the file */
        void setStreamPosition(long pos);

        /** @brief Returns the information whether the data type was found within the visum-file
            This method returns valid values only after the first step - the scanning process */
        bool positionKnown() const;

        /// Reads the data-type from the visum-file using the given reader
        bool readUsing(LineReader &reader, bool verbose);

        /// Returns the name of the data type
        const std::string &getDataName() const;

        /** @brief LineHandler-interface
            Returns values from the visum-file; Checks whether the data type is over */
        bool report(const std::string &line);

        /// Initialises the line parser
        void initLineParser(const std::string &pattern);

    protected:
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

        /** @brief tries to get a float which is possibly assigned to a certain modality
            When the float cannot be extracted using the given name, "IV" is
            appended to the begin of the name. Remark that this function does not
            yet support public traffic. */
        float getWeightedFloat(const std::string &name);

        /** @brief tries to get a bool which is possibly assigned to a certain modality
            When the bool cannot be extracted using the given name, "IV" is
            appended to the begin of the name. Remark that this function does not
            yet support public traffic. */
        bool getWeightedBool(const std::string &name);

    protected:
        /// The line parser to use
        NamedColumnsParser myLineParser;

        /// The name of the parsed data
        std::string myDataName;

    private:
        /// Sets the stream position to the
        void rewind(std::istream &strm) const;

        /// Returns the information whether the current line is an end-of-data line
        bool dataTypeEnded(const std::string &msg);

    private:
        /// Position the certain data type starts at within the stream
        long myPosition;

        /// Information whether to work in verbose mode
        bool myWorkVerbose;

    };

private:
    /// the line reader to read from the file
    LineReader myLineReader;

    /** the parser to parse the information from the data lines
        the visum format seems to vary, so a named parser is needed */
    NamedColumnsParser _lineParser;

    /// the converter to compute the lane number of edges from their capacity
    NBCapacity2Lanes _capacity2Lanes;

    /// information whether the parser shall run in verbose mode
    bool _verbose;

    /// the used vsystypes
    VSysTypeNames myVSysTypes;

    /** @brief definition of the list of known parsers
        (each reads a certain visum-type) */
    typedef std::vector<NIVisumSingleDataTypeParser*> ParserVector;

    /// list of known parsers
    ParserVector mySingleDataParsers;

	// list of visum traffic lights
    NIVisumTL_Map myNIVisumTLs;
private:
    /**
     * PositionSetter
     * Used within the scanning step for setting the positions of
     * the data
     */
    class PositionSetter : public LineHandler
    {
    public:
        /// Constructor
        PositionSetter(NIVisumLoader &parent);

        /// Destructor
        ~PositionSetter();

        /** @brief Interface of line handler - receives all lines, individually */
        bool report(const std::string &result);

    private:
        /** @brief The loader to inform about the occurence of a new data type begin
            The positions are not set within the PositionSetter itself. Rather, the Loader performs this. */
        NIVisumLoader &myParent;

    };

public:
    /** The PositionSetter is an internal class for retrieving data positions
        It shall have access to "checkForPosition" */
    friend class PositionSetter;

private:
    /** @brief Sets the begin of known data
        Checks whether the current line is the begin of one of the known data
        and saves the current position within the stream into the data type loader
        if so */
    bool checkForPosition(const std::string &line);

};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "NIVisumLoader.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:
