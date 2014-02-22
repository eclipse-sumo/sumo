/****************************************************************************/
/// @file    NIImporter_Vissim.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// Importer for networks stored in Vissim format
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef NIImporter_Vissim_h
#define NIImporter_Vissim_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <map>
#include <vector>
#include <utils/common/RGBColor.h>
#include <utils/geom/Position.h>
#include "tempstructs/NIVissimExtendedEdgePoint.h"
#include "NIVissimElements.h"


// ===========================================================================
// class declarations
// ===========================================================================
class OptionsCont;
class NBNetBuilder;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NIImporter_Vissim
 * @brief Importer for networks stored in Vissim format
 */
class NIImporter_Vissim {
public:
    /** @brief Loads network definition from the assigned option and stores it in the given network builder
     *
     * If the option "vissim-file" is set, the file stored therein is read and
     *  the network definition stored therein is stored within the given network
     *  builder.
     *
     * If the option "vissim-file" is not set, this method simply returns.
     *
     * @param[in] oc The options to use
     * @param[in] nb The network builder to fill
     */
    static void loadNetwork(const OptionsCont& oc, NBNetBuilder& nb);


protected:
    /// constructor
    NIImporter_Vissim(NBNetBuilder& nb, const std::string& file);

    /// destructor
    ~NIImporter_Vissim();

    /// loads the vissim file
    void load(const OptionsCont& options);

    bool admitContinue(const std::string& tag);

public:
    class VissimSingleTypeParser {
    public:
        /// Constructor
        VissimSingleTypeParser(NIImporter_Vissim& parent);

        /// Destructor
        virtual ~VissimSingleTypeParser();

        /** @brief Parses a single data type.
            Returns whether no error occured */
        virtual bool parse(std::istream& from) = 0;

    protected:
        /// reads from the stream and returns the lower case version of the read value
        std::string myRead(std::istream& from);

        /// as myRead, but returns "DATAEND" when the current field has ended
        std::string readEndSecure(std::istream& from,
                                  const std::string& excl = "");

        std::string readEndSecure(std::istream& from,
                                  const std::vector<std::string>& excl);

        /// overrides the optional label definition; returns the next tag as done by readEndSecure
        std::string overrideOptionalLabel(std::istream& from,
                                          const std::string& tag = "");

        /// returns the 2d-position saved as next within the stream
        Position getPosition(std::istream& from);

        /** @brief parses a listof vehicle types assigned to the current data field
            One should remeber, that -1 means "all" vehicle types */
        std::vector<int> parseAssignedVehicleTypes(std::istream& from,
                const std::string& next);

        NIVissimExtendedEdgePoint readExtEdgePointDef(std::istream& from);

        /** @brief Reads the structures name
            We cannot use the "<<" operator, as names may contain more than one word
            which are joined using '"'. */
        std::string readName(std::istream& from);

        /** @brief Overreads the named parameter (if) given and skips the rest until "DATAEND"
         */
        bool skipOverreading(std::istream& from, const std::string& name = "");

        /// Reads from the stream until the keywor occurs
        void readUntil(std::istream& from, const std::string& name);

    private:
        NIImporter_Vissim& myVissimParent;

    private:
        /// @brief Invalidated assignment operator.
        VissimSingleTypeParser& operator=(const VissimSingleTypeParser&);

    };


    /// definition of a map from color names to color definitions
    typedef std::map<std::string, RGBColor> ColorMap;

private:
    bool readContents(std::istream& strm);
    void postLoadBuild(SUMOReal offset);


    /// adds name-to-id - relationships of known elements into myKnownElements
    void insertKnownElements();

    /// adds id-to-parser - relationships of elements to parse into myParsers
    void buildParsers();

private:
    /// Definition of a map from element names to their numerical representation
    typedef std::map<std::string, NIVissimElement> ToElemIDMap;

    /// Map from element names to their numerical representation
    ToElemIDMap myKnownElements;

    /// Definition of a map from an element's numerical id to his parser
    typedef std::map<NIVissimElement, VissimSingleTypeParser*> ToParserMap;

    /// Parsers by element id
    ToParserMap myParsers;

    /// a map from color names to color definitions
    ColorMap myColorMap;

    std::string myLastSecure;

    NBNetBuilder& myNetBuilder;

private:
    /// @brief Invalidated copy constructor.
    NIImporter_Vissim(const NIImporter_Vissim&);

    /// @brief Invalidated assignment operator.
    NIImporter_Vissim& operator=(const NIImporter_Vissim&);

};


#endif

/****************************************************************************/

