/****************************************************************************/
/// @file    NIImporter_OpenDrive.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 14.04.2008
/// @version $Id: NIImporter_OpenDrive.h 7440 2009-07-15 09:04:03Z dkrajzew $
///
// Importer for networks stored in openDrive format
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
#ifndef NIImporter_OpenDrive_h
#define NIImporter_OpenDrive_h


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
#include <utils/xml/SUMOSAXHandler.h>


// ===========================================================================
// class declarations
// ===========================================================================
class NBNetBuilder;
class NBEdge;
class OptionsCont;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NIImporter_OpenDrive
 * @brief Importer for networks stored in openDrive format
 *
 */
class NIImporter_OpenDrive : public SUMOSAXHandler {
public:
    /** @brief Loads content of the optionally given SUMO file
     *
     * If the option "opendrive" is set, the file stored therein is read and
     *  the network definition stored therein is stored within the given network
     *  builder.
     *
     * If the option "opendrive" is not set, this method simply returns.
     *
     * The loading is done by parsing the network definition as an XML file
     *  using the SAXinterface and handling the incoming data via this class'
     *  methods.
     *
     * @param[in] oc The options to use
     * @param[in] nb The network builder to fill
     */
    static void loadNetwork(const OptionsCont &oc, NBNetBuilder &nb);


protected:
    /** @brief Constructor
     * @param[in] nc The node control to fill
     */
    NIImporter_OpenDrive(NBNodeCont &nc);


    /// @brief Destructor
    ~NIImporter_OpenDrive() throw();



    /// @name inherited from GenericSAXHandler
    //@{

    /** @brief Called on the opening of a tag;
     *
     * In dependence to the obtained type, an appropriate parsing
     *  method is called ("addEdge" if an edge encounters, f.e.).
     *
     * @param[in] element ID of the currently opened element
     * @param[in] attrs Attributes within the currently opened element
     * @exception ProcessError If something fails
     * @see GenericSAXHandler::myStartElement
     */
    void myStartElement(SumoXMLTag element,
                        const SUMOSAXAttributes &attrs) throw(ProcessError);


    /** @brief Called when characters occure
     *
     * @param[in] element ID of the last opened element
     * @param[in] chars The read characters (complete)
     * @exception ProcessError If something fails
     * @see GenericSAXHandler::myCharacters
     */
    void myCharacters(SumoXMLTag element,
                      const std::string &chars) throw(ProcessError);


    /** @brief Called when a closing tag occures
     *
     * @param[in] element ID of the currently opened element
     * @exception ProcessError If something fails
     * @see GenericSAXHandler::myEndElement
     */
    void myEndElement(SumoXMLTag element) throw(ProcessError);
    //@}


private:
    enum LinkType {
        OPENDRIVE_LT_SUCCESSOR,
        OPENDRIVE_LT_PREDECESSOR
    };

    enum ElementType {
        OPENDRIVE_ET_UNKNOWN,
        OPENDRIVE_ET_ROAD,
        OPENDRIVE_ET_JUNCTION
    };

    enum ContactPoint {
        OPENDRIVE_CP_UNKNOWN,
        OPENDRIVE_CP_START,
        OPENDRIVE_CP_END
    };

    enum GeometryType {
        OPENDRIVE_GT_UNKNOWN,
        OPENDRIVE_GT_LINE,
        OPENDRIVE_GT_SPIRAL,
        OPENDRIVE_GT_ARC,
        OPENDRIVE_GT_POLY3
    };

    /**
     * @struct OpenDriveLink
     * @brief Representation of an openDrive connection 
     */
    struct OpenDriveLink {
        OpenDriveLink(LinkType linkTypeArg, const std::string &elementIDArg)
            : linkType(linkTypeArg), elementID(elementIDArg), 
            elementType(OPENDRIVE_ET_UNKNOWN), contactPoint(OPENDRIVE_CP_UNKNOWN)
        { }

        LinkType linkType;
        std::string elementID;
        ElementType elementType;
        ContactPoint contactPoint;
    };


    /**
     * @struct OpenDriveGeometry
     * @brief Representation of an openDrive geometry part 
     */
    struct OpenDriveGeometry {
        OpenDriveGeometry(SUMOReal lengthArg, SUMOReal sArg, SUMOReal xArg, SUMOReal yArg, SUMOReal hdgArg)
            : length(lengthArg), s(sArg), x(xArg), y(yArg), hdg(hdgArg),
            type(OPENDRIVE_GT_UNKNOWN)
        { }

        SUMOReal length;
        SUMOReal s;
        SUMOReal x;
        SUMOReal y;
        SUMOReal hdg;
        GeometryType type;
        std::vector<SUMOReal> params;
    };


    /**
     * @struct OpenDriveEdge
     * @brief Representation of an openDrive road 
     */
    struct OpenDriveEdge {
        OpenDriveEdge(const std::string &idArg, const std::string &junctionArg, SUMOReal lengthArg)
            : id(idArg), junction(junctionArg), length(lengthArg)
        { }

        /// @brief The id of the edge
        std::string id;
        /// @brief The id of the junction the edge belongs to
        std::string junction;
        /// @brief The length of the edge
        SUMOReal length;
        std::vector<OpenDriveLink> links;
        std::vector<OpenDriveGeometry> geometries;
    };



private:
    void addLink(LinkType lt, const std::string &elementType, const std::string &elementID, 
        const std::string &contactPoint) throw(ProcessError);

    void addGeometryShape(GeometryType type, const std::vector<SUMOReal> &vals) throw(ProcessError);



    OpenDriveEdge myCurrentEdge;


};


#endif

/****************************************************************************/

