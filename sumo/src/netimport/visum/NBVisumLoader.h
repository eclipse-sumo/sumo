#ifndef NBVisumLoader_h
#define NBVisumLoader_h
/***************************************************************************
                          NBVisumLoader.h
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
#include <netbuild/NBCapacity2Lanes.h>
#include <utils/importio/LineHandler.h>
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
 * NBVisumLoader
 * This class parses the given visum file.
 * When the edge definition is before the node and the type definitions, it
 * will be parsed using a second step; otherwise this class parses the file
 * using a single step.
 * Types are loaded optionally.
 */
class NBVisumLoader : public FileErrorReporter,
                      public LineHandler {
private:
    /// enumeration of currently loaded data type
    enum DataType { NOTHING, VSYSTYPES, EDGES, TYPES, NODES, DISTRICTS,
        CONNECTORS, TURNS };
    /// the data type that is currently loaded
    DataType _nowReading;
    /// the current step
    size_t _step;
    /// the positions of the data within the file
    long _vsystypePos, _nodePos, _edgePos, _typePos, _districtPos,
        _connectorPos, _turnPos, _oldPos;
    /// the line reader to read from the file
    LineReader _lineReader;
    /// information, whether the types shall be loaded
    bool _vsystypesRead, _nodesRead, _edgesRead, _typesRead, _districtsRead,
        _connectorsRead, _turnsRead;
    /// the information whether types shall be loaded
    bool _loadTypes;
    /** the parser to parse the information from the data lines
        the visum format seems to vary, so a named parser is needed */
    NamedColumnsParser _lineParser;
    /// the converter to compute the lane number of edges from their cpaacity
    NBCapacity2Lanes _capacity2Lanes;
    /// information whether the parser shall run in verbose mode
    bool _verbose;
    /// a map of VSysTypes to the traffic type they represent
    std::map<std::string, std::string> _vsystypes;
public:
    /// constructor
    NBVisumLoader(const std::string &file, NBCapacity2Lanes capacity2Lanes);
    /// destructor
    ~NBVisumLoader();
    /// starts the parsing
    void load(OptionsCont &options);
    /** implementation of the LineHandler-interface called by a LineReader
        determines which information is currently read and parses it using
        submethods */
    bool report(const std::string &result);
private:
    /// initialises the parsing of nodes
    void initKnoten(const std::string &msg);
    /// initialises the parsing of edges
    void initStrecken(const std::string &msg);
    /// initialises the parsing of types
    void initTypen(const std::string &msg);
    /// initialises the parsing of districts
    void initBezirk(const std::string &msg);
    /// initialises the parsing of connectors
    void initAnbindung(const std::string &msg);
    /// initialises the parsing of turning definitions
    void initAbbiegeBeziehung(const std::string &msg);
    /// initialises a vsystype
    void initVSysType(const std::string &msg);

    /** interprets a type and stores it into NBTypeCont */
    bool addType(const std::string &msg);
    /** interprets a edge and stores it into NBEdgeCont */
    bool addEdge(const std::string &msg);
    /** interprets a node and stores it into NBNodeCont */
    bool addNode(const std::string &msg);
    /** interprets a district and stores it into NBDistrictCont */
    bool addDistrict(const std::string &msg);
    /** interprets a connector and stores it into NBConnectorCont */
    bool addConnector(const std::string &msg);
    /** interprets a tuning definition and stores it into
        the information about the certain edge via NBNodeCont */
    bool addTurningDefinition(const std::string &msg);
    /// adds a vsystype
    bool addVSysType(const std::string &msg);

    /** checks whether the current node is a valid one */
    bool checkNode(NBNode *node, const std::string &type,
        const std::string &nodeTypeName);
    /** returns the information whether the current turning definition
        is one for the individual traffic */
    bool isVehicleTurning();

    /// returns true, when the table of the type currently read has ended
    bool dataTypeEnded(const std::string &msg);
    /// returns the information whether the current step should be continued
    bool continueStep() const;
    /** adds an error which may occure when the expected data format is not used */
    void myAddError(const std::string &type, const std::string &id,
        const std::string &exception);
    /** returns the information whether the named bool is true
        when the parameter does not exist, the method retries the
            operation appending the string "(IV)" to the name
        the default return value is false */
    bool getWeightedBool(const std::string &name);
    /** returns the information whether the named float is true
        when the parameter does not exist, the method retries the
            operation appending the string "(IV)" to the name
        the default return value is -1 */
    float getWeightedFloat(const std::string &name);

};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "NBVisumLoader.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:
