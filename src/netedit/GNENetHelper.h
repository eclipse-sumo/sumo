/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    GNENetHelper.h
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2020
///
// Helper for GNENet
//
/****************************************************************************/
#pragma once
#include <config.h>

#include <foreign/rtree/SUMORTree.h>
#include <netbuild/NBEdge.h>
#include <netbuild/NBTrafficLightLogicCont.h>
#include <netbuild/NBVehicle.h>
#include <netedit/changes/GNEChange.h>
#include <netedit/dialogs/GNEDialog.h>
#include <utils/common/IDSupplier.h>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/foxtools/fxheader.h>
#include <utils/geom/Boundary.h>
#include <utils/geom/PositionVector.h>
#include <utils/geom/Triangle.h>
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/gui/globjects/GUIShapeContainer.h>
#include <utils/gui/settings/GUIVisualizationSettings.h>
#include <utils/router/SUMOAbstractRouter.h>
#include <utils/xml/SUMOXMLDefinitions.h>

// ===========================================================================
// class declarations
// ===========================================================================

class GNEAdditional;
class GNEApplicationWindow;
class GNEAttributeCarrier;
class GNEConnection;
class GNECrossing;
class GNEDataInterval;
class GNEDataSet;
class GNEDemandElement;
class GNEEdge;
class GNEEdgeType;
class GNEJunction;
class GNELane;
class GNELaneType;
class GNEMeanData;
class GNENet;
class GNENetworkElement;
class GNEPOI;
class GNEPoly;
class GNEUndoList;
class GNEViewNet;
class GNEWalkingArea;
class NBNetBuilder;

// ===========================================================================
// class definitions
// ===========================================================================

struct GNENetHelper {

    /// @brief struct used for saving all attribute carriers of net, in different formats
    class AttributeCarriers {

        /// @brief declare friend class
        friend class GNEAdditionalHandler;
        friend class GNERouteHandler;
        friend class GNEDataHandler;
        friend class GNEMeanDataHandler;
        friend class GNEJunction;
        friend class GNEEdge;
        friend class GNEDataSet;
        friend class GNEDataInterval;
        friend class GNEChange_Junction;
        friend class GNEChange_EdgeType;
        friend class GNEChange_Edge;
        friend class GNEChange_TAZSourceSink;
        friend class GNEChange_Additional;
        friend class GNEChange_Shape;
        friend class GNEChange_TAZElement;
        friend class GNEChange_DemandElement;
        friend class GNEChange_DataSet;
        friend class GNEChange_DataInterval;
        friend class GNEChange_GenericData;
        friend class GNEChange_MeanData;
        friend class GNETLSEditorFrame;

    public:
        /// @brief constructor
        AttributeCarriers(GNENet* net);

        /// @brief destructor
        ~AttributeCarriers();

        /// @brief remap junction and edge IDs
        void remapJunctionAndEdgeIds();

        /// @brief check if shape of given AC (network element) is around the given triangle
        bool isNetworkElementAroundTriangle(GNEAttributeCarrier* AC, const Triangle& triangle) const;

        /// @brief functions related with number of elements sorted by categories
        /// @{
        /// @brief get number of current network elements saved in AttributeCarriers
        int getNumberOfNetworkElements() const;

        /// @brief get number of current demand elements saved in AttributeCarriers (default vTypes are NOT included)
        int getNumberOfDemandElements() const;

        /// @brief get number of current data elements saved in AttributeCarriers
        int getNumberOfDataElements() const;

        /// @}

        /// @name function for attribute carriers
        /// @{

        /**@brief get a single attribute carrier based on a GLID
         * @param[in] ids the GL IDs for which to retrieve the AC
         * @param[in] hardFail Whether attempts to retrieve a nonexisting AttributeCarrier should result in an exception
         * @throws InvalidArgument if GL ID doesn't have a associated Attribute Carrier
         */
        GNEAttributeCarrier* retrieveAttributeCarrier(const GUIGlID id, bool hardFail = true) const;

        /**@brief get the attribute carriers based on Type
         * @param[in] type The GUI-type of the AC. SUMO_TAG_NOTHING returns all elements (Warning: bottleneck)
         * @note tag could not exist
         */
        std::vector<GNEAttributeCarrier*> retrieveAttributeCarriers(SumoXMLTag tag = SUMO_TAG_NOTHING);

        /// @brief get the attribute carriers based on supermode and selected
        std::vector<GNEAttributeCarrier*> retrieveAttributeCarriers(Supermode supermode, const bool onlySelected);

        /// @brief get all selected attribute carriers (or only relative to current supermode
        std::vector<GNEAttributeCarrier*> getSelectedAttributeCarriers(const bool ignoreCurrentSupermode);

        /// @}

        /// @name function for junctions
        /// @{
        /**@brief get junction by id
         * @param[in] id The id of the desired junction
         * @param[in] hardFail Whether attempts to retrieve a nonexisting junction should result in an exception
         * @throws UnknownElement
         */
        GNEJunction* retrieveJunction(const std::string& id, bool hardFail = true) const;

        /// @brief get junctions
        const std::map<std::string, GNEJunction*>& getJunctions() const;

        /// @brief return selected junctions
        std::vector<GNEJunction*> getSelectedJunctions() const;

        /// @brief registers a junction in containers
        GNEJunction* registerJunction(GNEJunction* junction);

        /// @brief clear junctions
        void clearJunctions();

        /// @brief add prefix to all junctions
        void addPrefixToJunctions(const std::string& prefix);

        /// @brief update junction ID in container
        void updateJunctionID(GNEJunction* junction, const std::string& newID);

        /// @brief get number of selected junctions
        int getNumberOfSelectedJunctions() const;

        /// @}

        /// @name function for crossings
        /// @{
        /**@brief get Crossing by AC
         * @param[in] glObject The GUIGlObject associated with the element
         * @param[in] hardFail Whether attempts to retrieve a nonexisting Crossing should result in an exception
         * @throws UnknownElement
         */
        GNECrossing* retrieveCrossing(const GUIGlObject* glObject, bool hardFail = true) const;

        /// @brief get crossings
        const std::unordered_map<const GUIGlObject*, GNECrossing*>& getCrossings() const;

        /// @brief return all selected crossings
        std::vector<GNECrossing*> getSelectedCrossings() const;

        /// @brief get number of selected crossings
        int getNumberOfSelectedCrossings() const;

        /// @}

        /// @name function for walkingAreas
        /// @{
        /**@brief get WalkingArea by GlObject
         * @param[in] glObject The GUIGlObject associated with the element
         * @param[in] hardFail Whether attempts to retrieve a nonexisting WalkingArea should result in an exception
         * @throws UnknownElement
         */
        GNEWalkingArea* retrieveWalkingArea(const GUIGlObject* glObject, bool hardFail = true) const;

        /// @brief get walkingAreas
        const std::unordered_map<const GUIGlObject*, GNEWalkingArea*>& getWalkingAreas() const;

        /// @brief return all selected walkingAreas
        std::vector<GNEWalkingArea*> getSelectedWalkingAreas() const;

        /// @brief get number of selected walkingAreas
        int getNumberOfSelectedWalkingAreas() const;

        /// @}

        /// @name function for edgeTypes
        /// @{
        /**@brief get edge type by id
         * @param[in] id The id of the desired edge type
         * @param[in] hardFail Whether attempts to retrieve a nonexisting edge type should result in an exception
         * @throws UnknownElement
         */
        GNEEdgeType* retrieveEdgeType(const std::string& id, bool hardFail = true) const;

        /// @brief registers a edge in containers
        GNEEdgeType* registerEdgeType(GNEEdgeType* edgeType);

        /// @brief map with the ID and pointer to edgeTypes of net
        const std::map<std::string, GNEEdgeType*>& getEdgeTypes() const;

        /// @brief clear edgeTypes
        void clearEdgeTypes();

        /// @brief update edgeType ID in container
        void updateEdgeTypeID(GNEEdgeType* edgeType, const std::string& newID);

        /// @brief generate edgeType id
        std::string generateEdgeTypeID() const;

        /// @}

        /// @name function for edges
        /// @{
        /**@brief get edge by id
         * @param[in] id The id of the desired edge
         * @param[in] hardFail Whether attempts to retrieve a nonexisting edge should result in an exception
         * @throws UnknownElement
         */
        GNEEdge* retrieveEdge(const std::string& id, bool hardFail = true) const;

        /**@brief get all edges by from and to GNEJunction
         * @param[in] id The id of the desired edge
         * @param[in] hardFail Whether attempts to retrieve a nonexisting edge should result in an exception
         * @throws UnknownElement
         */
        std::vector<GNEEdge*> retrieveEdges(GNEJunction* from, GNEJunction* to) const;

        /// @brief map with the ID and pointer to edges of net
        const std::map<std::string, GNEEdge*>& getEdges() const;

        /**@brief return all edges
         * @param[in] onlySelected Whether to return only selected edges
         */
        std::vector<GNEEdge*> getSelectedEdges() const;

        /// @brief registers an edge with containers
        GNEEdge* registerEdge(GNEEdge* edge);

        /// @brief clear edges
        void clearEdges();

        /// @brief add prefix to all edges
        void addPrefixToEdges(const std::string& prefix);

        /// @brief generate edge ID
        std::string generateEdgeID() const;

        /// @brief update edge ID in container
        void updateEdgeID(GNEEdge* edge, const std::string& newID);

        /// @brief get number of selected edges
        int getNumberOfSelectedEdges() const;

        /// @}

        /// @name function for lanes
        /// @{
        /**@brief get lane by id
         * @param[in] id The id of the desired lane
         * @param[in] hardFail Whether attempts to retrieve a nonexisting lane should result in an exception
         * @param[in] checkVolatileChange Used by additionals after recomputing with volatile options.
         * @throws UnknownElement
         */
        GNELane* retrieveLane(const std::string& id, bool hardFail = true, bool checkVolatileChange = false) const;

        /**@brief get Lane by GlObject
         * @param[in] glObject The GUIGlObject associated with the element
         * @param[in] hardFail Whether attempts to retrieve a nonexisting Lane should result in an exception
         * @throws UnknownElement
         */
        GNELane* retrieveLane(const GUIGlObject* glObject, bool hardFail = true) const;

        /// @brief get lanes
        const std::unordered_map<const GUIGlObject*, GNELane*>& getLanes() const;

        /// @brief get selected lanes
        std::vector<GNELane*> getSelectedLanes() const;

        /// @brief get number of selected lanes
        int getNumberOfSelectedLanes() const;

        /// @}

        /// @name function for connections
        /// @{
        /**@brief get Connection by id
         * @param[in] id The id of the desired Connection
         * @param[in] hardFail Whether attempts to retrieve a nonexisting Connection should result in an exception
         * @throws UnknownElement
         */
        GNEConnection* retrieveConnection(const std::string& id, bool hardFail = true) const;

        /**@brief get Connection by GUIGlObject
         * @param[in] glObject The GUIGlObject associated with the element
         * @param[in] hardFail Whether attempts to retrieve a nonexisting Connection should result in an exception
         * @throws UnknownElement
         */
        GNEConnection* retrieveConnection(const GUIGlObject* glObject, bool hardFail = true) const;

        /// @brief get connections
        const std::unordered_map<const GUIGlObject*, GNEConnection*>& getConnections() const;

        /// @brief get selected connections
        std::vector<GNEConnection*> getSelectedConnections() const;

        /// @brief get number of selected connections
        int getNumberOfSelectedConnections() const;

        /// @}

        /// @name function for internalLanes
        /// @{

        /**@brief get InternalLane by GUIGlObject
         * @param[in] glObject The GUIGlObject associated with the element
         * @param[in] hardFail Whether attempts to retrieve a nonexisting InternalLane should result in an exception
         * @throws UnknownElement
         */
        GNEInternalLane* retrieveInternalLane(const GUIGlObject* glObject, bool hardFail = true) const;

        /// @}

        /// @name function for additionals
        /// @{
        /**@brief Returns the named additional
         * @param[in] id The attribute carrier related with the additional element
         * @param[in] type tag with the type of additional
         * @param[in] id The id of the additional to return.
         * @param[in] hardFail Whether attempts to retrieve a nonexisting additional should result in an exception
         */
        GNEAdditional* retrieveAdditional(SumoXMLTag type, const std::string& id, bool hardFail = true) const;

        /**@brief Returns the named additional
         * @param[in] id The attribute carrier related with the additional element
         * @param[in] types tags with the type of additional
         * @param[in] id The id of the additional to return.
         * @param[in] hardFail Whether attempts to retrieve a nonexisting additional should result in an exception
         */
        GNEAdditional* retrieveAdditionals(const std::vector<SumoXMLTag> types, const std::string& id, bool hardFail = true) const;

        /**@brief Returns the named additional
         * @param[in] glObject The GUIGlObject associated with the element
         * @param[in] hardFail Whether attempts to retrieve a nonexisting additional should result in an exception
         */
        GNEAdditional* retrieveAdditional(const GUIGlObject* glObject, bool hardFail = true) const;

        /**@brief Returns the rerouter interval defined by given begin and end
         * @param[in] rerouter ID
         * @param[in] begin SUMOTime begin
         * @param[in] end SUMOTime begin
         */
        GNEAdditional* retrieveRerouterInterval(const std::string& rerouterID, const SUMOTime begin, const SUMOTime end) const;

        /// @brief get additionals
        const std::unordered_map<SumoXMLTag, std::unordered_map<const GUIGlObject*, GNEAdditional*>, std::hash<int> >& getAdditionals() const;

        /// @brief get selected additionals
        std::vector<GNEAdditional*> getSelectedAdditionals() const;

        /// @brief get selected shapes
        std::vector<GNEAdditional*> getSelectedShapes() const;

        /// @brief get number of additionals
        int getNumberOfAdditionals() const;

        /// @brief clear additionals
        void clearAdditionals();

        /// @brief update additional ID in container
        void updateAdditionalID(GNEAdditional* additional, const std::string& newID);

        /// @brief generate additional id
        std::string generateAdditionalID(SumoXMLTag type) const;

        /// @brief get number of selected additionals (Including POIs, Polygons, TAZs and Wires)
        int getNumberOfSelectedAdditionals() const;

        /// @brief get number of selected pure additionals (Except POIs, Polygons, TAZs and Wires)
        int getNumberOfSelectedPureAdditionals() const;

        /// @brief get number of selected polygons
        int getNumberOfSelectedPolygons() const;

        /// @brief get number of selected walkable areas
        int getNumberOfSelectedJpsWalkableAreas() const;

        /// @brief get number of selected obstacles
        int getNumberOfSelectedJpsObstacles() const;

        /// @brief get number of selected POIs
        int getNumberOfSelectedPOIs() const;

        /// @brief get number of selected TAZs
        int getNumberOfSelectedTAZs() const;

        /// @brief get number of selected Wires
        int getNumberOfSelectedWires() const;

        /// @brief return list of available POI parameters
        std::vector<std::string> getPOIParamKeys() const;
        /// @}

        /// @name function for TAZ sourceSinks
        /// @{
        /**@brief Returns the named sourceSink
         * @param[in] sourceSink The GNETAZSourceSink to retrieve
         * @param[in] hardFail Whether attempts to retrieve a nonexisting sourceSink should result in an exception
         */
        GNETAZSourceSink* retrieveTAZSourceSink(const GNEAttributeCarrier* sourceSink, bool hardFail = true) const;

        /// @brief get sourceSinks
        const std::unordered_map<SumoXMLTag, std::unordered_map<const GNEAttributeCarrier*, GNETAZSourceSink*>, std::hash<int> >& getTAZSourceSinks() const;

        /// @brief get number of TAZSourceSinks
        int getNumberOfTAZSourceSinks() const;

        /// @brief clear sourceSinks
        void clearTAZSourceSinks();

        /// @}

        /// @name function for demand elements
        /// @{
        /**@brief Returns the named demand element
         * @param[in] type tag with the type of demand element
         * @param[in] id The id of the demand element to return.
         * @param[in] hardFail Whether attempts to retrieve a nonexisting demand element should result in an exception
         */
        GNEDemandElement* retrieveDemandElement(SumoXMLTag type, const std::string& id, bool hardFail = true) const;

        /**@brief Returns the named demand element
         * @param[in] types tag with the type of demand element
         * @param[in] id The id of the demand element to return.
         * @param[in] hardFail Whether attempts to retrieve a nonexisting demand element should result in an exception
         */
        GNEDemandElement* retrieveDemandElements(const std::vector<SumoXMLTag> types, const std::string& id, bool hardFail = true) const;

        /**@brief Returns the named demand
         * @param[in] glObject The GUIGlObject associated with the element
         * @param[in] hardFail Whether attempts to retrieve a nonexisting demand should result in an exception
         */
        GNEDemandElement* retrieveDemandElement(const GUIGlObject* glObject, bool hardFail = true) const;

        /// @brief get selected demand elements
        std::vector<GNEDemandElement*> getSelectedDemandElements() const;

        /// @brief get demand elements
        const std::unordered_map<SumoXMLTag, std::unordered_map<const GUIGlObject*, GNEDemandElement*>, std::hash<int> >& getDemandElements() const;

        /// @brief generate demand element id
        std::string generateDemandElementID(SumoXMLTag tag) const;

        /// @brief get default type
        GNEDemandElement* getDefaultType() const;

        /// @brief clear demand elements
        void clearDemandElements();

        /// @brief update demand element ID in container
        void updateDemandElementID(GNEDemandElement* demandElement, const std::string& newID);

        /// @brief add default VTypes
        void addDefaultVTypes();

        /// @brief get (and update) stop index
        int getStopIndex();

        /// @brief get number of selected demand elements
        int getNumberOfSelectedDemandElements() const;

        /// @brief get number of selected routes
        int getNumberOfSelectedRoutes() const;

        /// @brief get number of selected vehicles
        int getNumberOfSelectedVehicles() const;

        /// @brief get number of selected persons
        int getNumberOfSelectedPersons() const;

        /// @brief get number of selected person trips
        int getNumberOfSelectedPersonTrips() const;

        /// @brief get number of selected walks
        int getNumberOfSelectedWalks() const;

        /// @brief get number of selected rides
        int getNumberOfSelectedRides() const;

        /// @brief get number of selected containers
        int getNumberOfSelectedContainers() const;

        /// @brief get number of selected transports
        int getNumberOfSelectedTransport() const;

        /// @brief get number of selected tranships
        int getNumberOfSelectedTranships() const;

        /// @brief get number of selected stops
        int getNumberOfSelectedStops() const;

        /// @}

        /// @name function for data sets
        /// @{
        /**@brief Returns the named data set
         * @param[in] id The id of the data set to return.
         * @param[in] hardFail Whether attempts to retrieve a nonexisting data set should result in an exception
         */
        GNEDataSet* retrieveDataSet(const std::string& id, bool hardFail = true) const;

        /// @brief get demand elements
        const std::map<const std::string, GNEDataSet*>& getDataSets() const;

        /// @brief generate data set id
        std::string generateDataSetID() const;

        /// @}

        /// @name function for data intervals
        /// @{
        /**@brief Returns the data interval
         * @param[in] id The attribute carrier related with the dataInterval element
         * @param[in] hardFail Whether attempts to retrieve a nonexisting data set should result in an exception
         */
        GNEDataInterval* retrieveDataInterval(const GNEAttributeCarrier* AC, bool hardFail = true) const;

        /// @brief get all data intervals of network
        const std::unordered_map<const GNEAttributeCarrier*, GNEDataInterval*>& getDataIntervals() const;

        /// @}

        /// @name function for generic datas
        /// @{
        /**@brief Returns the generic data
         * @param[in] id The attribute carrier related with the genericData element
         * @param[in] hardFail Whether attempts to retrieve a nonexisting data set should result in an exception
         */
        GNEGenericData* retrieveGenericData(const GUIGlObject* glObject, bool hardFail = true) const;

        /// @brief get selected generic datas
        std::vector<GNEGenericData*> getSelectedGenericDatas() const;

        /// @brief get all generic datas
        const std::unordered_map<SumoXMLTag, std::unordered_map<const GUIGlObject*, GNEGenericData*>, std::hash<int> >& getGenericDatas() const;

        /// @brief retrieve generic datas within the given interval
        std::vector<GNEGenericData*> retrieveGenericDatas(const SumoXMLTag genericDataTag, const double begin, const double end);

        /// @brief Return the number of generic datas
        int getNumberOfGenericDatas() const;

        /// @brief get number of selected edge datas
        int getNumberOfSelectedEdgeDatas() const;

        /// @brief get number of selected edge rel datas
        int getNumberOfSelectedEdgeRelDatas() const;

        /// @brief get number of selected edge TAZ Rels
        int getNumberOfSelectedEdgeTAZRel() const;

        /// @brief return a set of parameters for the given data Interval
        std::set<std::string> retrieveGenericDataParameters(const std::string& genericDataTag, const double begin, const double end) const;

        /// @brief return a set of parameters for the given dataSet, generic data Type, begin and end
        std::set<std::string> retrieveGenericDataParameters(const std::string& dataSetID, const std::string& genericDataTag,
                const std::string& beginStr, const std::string& endStr) const;

        /// @}

        /// @name function for meanDatas
        /// @{
        /**@brief Returns the named meanData
         * @param[in] id The attribute carrier related with the meanData element
         * @param[in] type tag with the type of meanData
         * @param[in] id The id of the meanData to return.
         * @param[in] hardFail Whether attempts to retrieve a nonexisting meanData should result in an exception
         */
        GNEMeanData* retrieveMeanData(SumoXMLTag type, const std::string& id, bool hardFail = true) const;

        /// @brief get meanDatas
        const std::unordered_map<SumoXMLTag, std::map<const std::string, GNEMeanData*>, std::hash<int> >& getMeanDatas() const;

        /// @brief get number of meanDatas
        int getNumberOfMeanDatas() const;

        /// @brief clear meanDatas
        void clearMeanDatas();

        /// @brief update meanData ID in container
        void updateMeanDataID(GNEMeanData* meanData, const std::string& newID);

        /// @brief generate meanData id
        std::string generateMeanDataID(SumoXMLTag type) const;

        /// @}

    protected:
        /// @name Junctions protected functions
        /// @{

        /// @brief insert junction in container
        void insertJunction(GNEJunction* junction);

        /// @brief delete junction from container
        void deleteSingleJunction(GNEJunction* junction);

        /// @}

        /// @name edge types protected functions
        /// @{

        /// @brief insert edge type in container
        void insertEdgeType(GNEEdgeType* edgeType);

        /// @brief delete edge type from container
        void deleteEdgeType(GNEEdgeType* edgeType);

        /// @}

        /// @name edges protected functions
        /// @{
        /// @brief insert edge in container
        void insertEdge(GNEEdge* edge);

        /// @brief delete edge from container
        void deleteSingleEdge(GNEEdge* edge);

        /// @}

        /// @name lane protected functions
        /// @{

        /// @brief insert lane in container
        void insertLane(GNELane* lane);

        /// @brief delete lane from container
        void deleteLane(GNELane* lane);

        /// @}

        /// @name crossing protected functions
        /// @{

        /// @brief insert crossing in container
        void insertCrossing(GNECrossing* crossing);

        /// @brief delete crossing from container
        void deleteCrossing(GNECrossing* crossing);

        /// @}

        /// @name walking areas protected functions
        /// @{

        /// @brief insert walkingArea in container
        void insertWalkingArea(GNEWalkingArea* walkingArea);

        /// @brief delete walkingArea from container
        void deleteWalkingArea(GNEWalkingArea* walkingArea);

        /// @}

        /// @name connection protected functions
        /// @{

        /// @brief insert connection in container
        void insertConnection(GNEConnection* connection);

        /// @brief delete connection from container
        void deleteConnection(GNEConnection* connection);

        /// @}

        /// @name internalLane protected functions
        /// @{

        /// @brief insert internalLane in container
        void insertInternalLane(GNEInternalLane* internalLane);

        /// @brief delete internalLane from container
        void deleteInternalLane(GNEInternalLane* internalLane);

        /// @}

        /// @name additionals protected functions
        /// @{

        /// @brief Insert a additional element in container.
        void insertAdditional(GNEAdditional* additional);

        /// @brief delete additional element of container
        void deleteAdditional(GNEAdditional* additional);

        /// @}

        /// @name TAZ Source Sinks protected functions
        /// @{

        /// @brief Insert a sourceSink element in container.
        void insertTAZSourceSink(GNETAZSourceSink* sourceSink);

        /// @brief delete sourceSink element of container
        void deleteTAZSourceSink(GNETAZSourceSink* sourceSink);

        /// @}

        /// @name demand elements protected functions
        /// @{

        /// @brief Insert a demand element in container.
        void insertDemandElement(GNEDemandElement* demandElement);

        /// @brief delete demand element of container
        void deleteDemandElement(GNEDemandElement* demandElement, const bool updateFrames);

        /// @}

        /// @name datas protected functions
        /// @{

        /// @brief Insert a data set in container.
        void insertDataSet(GNEDataSet* dataSet);

        /// @brief delete data set of container
        void deleteDataSet(GNEDataSet* dataSet);

        /// @}

        /// @name data intervals protected functions
        /// @{

        /// @brief insert data interval in container
        void insertDataInterval(const GNEAttributeCarrier* AC, GNEDataInterval* dataInterval);

        /// @brief delete data interval of container
        void deleteDataInterval(GNEDataInterval* dataInterval);

        /// @}

        /// @name generic datas protected functions
        /// @{

        /// @brief insert generic data in container
        void insertGenericData(GNEGenericData* genericData);

        /// @brief delete generic data of container
        void deleteGenericData(GNEGenericData* genericData);

        /// @}

        /// @name Insertion and erasing of GNEMeanDatas items
        /// @{

        /// @brief Insert a meanData element in container.
        void insertMeanData(GNEMeanData* meanData);

        /// @brief delete meanData element of container
        void deleteMeanData(GNEMeanData* meanData);

        /// @}

        /// @brief update demand element frames (called after insert/delete demand element)
        void updateDemandElementFrames(const GNETagProperties* tagProperty);

        /// @brief retrieve attribute carriers recursively
        void retrieveAttributeCarriersRecursively(const GNETagProperties* tag, std::vector<GNEAttributeCarrier*>& ACs);

    private:
        /// @brief pointer to net
        GNENet* myNet;

        /// @brief stop index
        int myStopIndex;

        /// @brief number of network elemements inserted in AttributeCarriers
        int myNumberOfNetworkElements = 0;

        /// @brief number of demand elemements inserted in AttributeCarriers (excluding default vTypes)
        int myNumberOfDemandElements = 0;

        /// @brief number of data elemements inserted in AttributeCarriers
        int myNumberOfDataElements = 0;

        /// @brief number of mean data elemements inserted in AttributeCarriers
        int myNumberOfMeanDataElements = 0;

        /// @brief map with the ID and pointer to junctions of net
        std::map<std::string, GNEJunction*> myJunctions;

        /// @brief set with crossings
        std::unordered_map<const GUIGlObject*, GNECrossing*> myCrossings;

        /// @brief set with walkingAreas
        std::unordered_map<const GUIGlObject*, GNEWalkingArea*> myWalkingAreas;

        /// @brief map with the ID and pointer to edgeTypes of net
        std::map<std::string, GNEEdgeType*> myEdgeTypes;

        /// @brief map with the ID and pointer to edges of net
        std::map<std::string, GNEEdge*> myEdges;

        /// @brief map with lanes
        std::unordered_map<const GUIGlObject*, GNELane*> myLanes;

        /// @brief map with connetions
        std::unordered_map<const GUIGlObject*, GNEConnection*> myConnections;

        /// @brief map with internal lanes
        std::unordered_map<const GUIGlObject*, GNEInternalLane*> myInternalLanes;

        /// @brief map with the tag and pointer to additional elements of net, sorted by IDs
        std::unordered_map<SumoXMLTag, std::map<const std::string, GNEAdditional*>, std::hash<int> > myAdditionalIDs;

        /// @brief map with the tag and pointer to additional elements of net
        std::unordered_map<SumoXMLTag, std::unordered_map<const GUIGlObject*, GNEAdditional*>, std::hash<int> > myAdditionals;

        /// @brief map with the tag and pointer to TAZSourceSinks elements of net
        std::unordered_map<SumoXMLTag, std::unordered_map<const GNEAttributeCarrier*, GNETAZSourceSink*>, std::hash<int> > myTAZSourceSinks;

        /// @brief map with the tag and pointer to demand elements of net, sorted by IDs
        std::unordered_map<SumoXMLTag, std::map<const std::string, GNEDemandElement*>, std::hash<int> > myDemandElementIDs;

        /// @brief map with the tag and pointer to demand elements elements of net
        std::unordered_map<SumoXMLTag, std::unordered_map<const GUIGlObject*, GNEDemandElement*>, std::hash<int> > myDemandElements;

        /// @brief map with the ID and pointer to all datasets of net
        std::map<const std::string, GNEDataSet*> myDataSets;

        /// @brief map with all data intervals of network
        std::unordered_map<const GNEAttributeCarrier*, GNEDataInterval*> myDataIntervals;

        /// @brief map with the tag and pointer to all generic datas
        std::unordered_map<SumoXMLTag, std::unordered_map<const GUIGlObject*, GNEGenericData*>, std::hash<int> > myGenericDatas;

        /// @brief map with the tag and pointer to meanData elements of net
        std::unordered_map<SumoXMLTag, std::map<const std::string, GNEMeanData*>, std::hash<int> > myMeanDatas;

        /// @brief Invalidated default constructor.
        AttributeCarriers() = delete;

        /// @brief Invalidated copy constructor.
        AttributeCarriers(const AttributeCarriers&) = delete;

        /// @brief Invalidated assignment operator.
        AttributeCarriers& operator=(const AttributeCarriers&) = delete;
    };

    /// @brief modul for AC Templates
    class ACTemplate {

    public:
        /// @brief constructor
        ACTemplate(GNENet* net);

        /// @brief build templates
        void buildTemplates();

        /// @brief destructor
        ~ACTemplate();

        /// @brief get all AC templates
        std::map<SumoXMLTag, GNEAttributeCarrier*> getACTemplates() const;

        /// @brief get template AC by tag
        GNEAttributeCarrier* getTemplateAC(const SumoXMLTag tag) const;

        /// @brief get template AC by text (using selector text
        GNEAttributeCarrier* getTemplateAC(const std::string& selectorText) const;

    private:
        /// @brief pointer to net
        GNENet* myNet = nullptr;

        /// @brief map with templates
        std::map<SumoXMLTag, GNEAttributeCarrier*> myTemplates;

        /// @brief Invalidated default constructor.
        ACTemplate() = delete;

        /// @brief Invalidated copy constructor.
        ACTemplate(const ACTemplate&) = delete;

        /// @brief Invalidated assignment operator
        ACTemplate& operator=(const ACTemplate& src) = delete;
    };

    /// @brief modul for handling saving files
    class SavingFilesHandler {

    public:
        /// @brief typedef used for group ACs by filename
        typedef std::map<std::string, std::unordered_set<const GNEAttributeCarrier*> > ACsbyFilename;

        /// @brief constructor
        SavingFilesHandler(GNENet* net);

        /// @brief update netedit config
        void updateNeteditConfig();

        /// @brief additional elements
        /// @{

        /// @brief add additional filename
        void addAdditionalFilename(const GNEAttributeCarrier* additionalElement);

        /// @brief update additional elements with empty filenames with the given file
        void updateAdditionalEmptyFilenames(const std::string& file);

        /// @brief get vector with additional elements saving files (starting with default)
        const std::vector<std::string>& getAdditionalFilenames() const;

        /// @brief get additionals sorted by filenames (and also clear unused filenames)
        ACsbyFilename getAdditionalsByFilename();

        /// @brief check if the given additional file was already registered
        bool existAdditionalFilename(const std::string& file) const;

        /// @}

        /// @brief demand elements
        /// @{

        /// @brief add demand filename
        void addDemandFilename(const GNEAttributeCarrier* demandElement);

        /// @brief update demand elements with empty filenames with the given file
        void updateDemandEmptyFilenames(const std::string& file);

        /// @brief get vector with demand elements saving files (starting with default)
        const std::vector<std::string>& getDemandFilenames() const;

        /// @brief get demands sorted by filenames (and also clear unused filenames)
        ACsbyFilename getDemandsByFilename();

        /// @brief check if the given demand file was already registered
        bool existDemandFilename(const std::string& file) const;

        /// @}

        /// @brief data elements
        /// @{

        /// @brief add data filename
        void addDataFilename(const GNEAttributeCarrier* dataElement);

        /// @brief update data elements with empty filenames with the given file
        void updateDataEmptyFilenames(const std::string& file);

        /// @brief get vector with data elements saving files (starting with default)
        const std::vector<std::string>& getDataFilenames() const;

        /// @brief get datas sorted by filenames (and also clear unused filenames)
        ACsbyFilename getDatasByFilename();

        /// @brief check if the given data file was already registered
        bool existDataFilename(const std::string& file) const;

        /// @}

        /// @brief meanData elements
        /// @{

        /// @brief add meanData filename
        void addMeanDataFilename(const GNEAttributeCarrier* meanDataElement);

        /// @brief update meanData elements with empty filenames with the given file
        void updateMeanDataEmptyFilenames(const std::string& file);

        /// @brief get vector with meanData elements saving files (starting with default)
        const std::vector<std::string>& getMeanDataFilenames() const;

        /// @brief get meanDatas sorted by filenames (and also clear unused filenames)
        ACsbyFilename getMeanDatasByFilename();

        /// @brief check if the given meanData file was already registered
        bool existMeanDataFilename(const std::string& file) const;

        /// @}

    private:
        /// @brief pointer to net
        GNENet* myNet;

        /// @brief vector with additional elements saving files
        std::vector<std::string> myAdditionalElementsSavingFiles;

        /// @brief vector with demand elements saving files
        std::vector<std::string> myDemandElementsSavingFiles;

        /// @brief vector with data elements saving files
        std::vector<std::string> myDataElementsSavingFiles;

        /// @brief vector with mean data elements saving files
        std::vector<std::string> myMeanDataElementsSavingFiles;

        /// @brief parsing saving files
        std::string parsingSavingFiles(const std::vector<std::string>& savingFiles) const;

        /// @brief Invalidated default constructor.
        SavingFilesHandler() = delete;

        /// @brief Invalidated copy constructor.
        SavingFilesHandler(const SavingFilesHandler&) = delete;

        /// @brief Invalidated assignment operator.
        SavingFilesHandler& operator=(const SavingFilesHandler&) = delete;
    };

    /// @brief modul for Saving status
    class SavingStatus {

    public:
        /// @brief constructor
        SavingStatus(GNENet* net);

        /// @name SumoConfig
        /// @{

        /// @brief inform that SumoConfig has to be saved
        void requireSaveSumoConfig();

        /// @brief mark SumoConfig as saved
        void SumoConfigSaved();

        /// @brief check if SumoConfig is saved
        bool isSumoConfigSaved() const;

        /// @}

        /// @name NeteditConfig
        /// @{

        /// @brief inform that netedit config has to be saved
        void requireSaveNeteditConfig();

        /// @brief mark netedit config as saved
        void neteditConfigSaved();

        /// @brief check if netedit config is saved
        bool isNeteditConfigSaved() const;

        /// @}

        /// @name network
        /// @{

        /// @brief inform that network has to be saved
        void requireSaveNetwork();

        /// @brief mark network as saved
        void networkSaved();

        /// @brief check if network is saved
        bool isNetworkSaved() const;

        /// @}

        /// @name TLS
        /// @{

        /// @brief inform that TLS has to be saved
        void requireSaveTLS();

        /// @brief mark TLS as saved
        void TLSSaved();

        /// @brief check if TLS are saved
        bool isTLSSaved() const;

        /// @}

        /// @name edge types
        /// @{

        /// @brief inform that edgeType has to be saved
        void requireSaveEdgeType();

        /// @brief mark edgeType as saved
        void edgeTypeSaved();

        /// @brief check if edgeType are saved
        bool isEdgeTypeSaved() const;

        /// @}

        /// @name additionals
        /// @{

        /// @brief inform that additionals has to be saved
        void requireSaveAdditionals();

        /// @brief mark additionals as saved
        void additionalsSaved();

        /// @brief check if additionals are saved
        bool isAdditionalsSaved() const;

        /// @}

        /// @name demand elements
        /// @{

        /// @brief inform that demand elements has to be saved
        void requireSaveDemandElements();

        /// @brief mark demand elements as saved
        void demandElementsSaved();

        /// @brief check if demand elements are saved
        bool isDemandElementsSaved() const;

        /// @}

        /// @name data elements
        /// @{

        /// @brief inform that data elements has to be saved
        void requireSaveDataElements();

        /// @brief mark demand elements as saved
        void dataElementsSaved();

        /// @brief check if data elements are saved
        bool isDataElementsSaved() const;

        /// @}

        /// @name mean datas
        /// @{

        /// @brief inform that mean data elements has to be saved
        void requireSaveMeanDatas();

        /// @brief mark mean data elements as saved
        void meanDatasSaved();

        /// @brief check if mean data elements are saved
        bool isMeanDatasSaved() const;

        /// @}

        /// @name function to ask if save elements before close/quit
        /// @{

        /// @brief warns about unsaved changes in network and gives the user the option to abort
        GNEDialog::Result askSaveNetwork(GNEDialog::Result &commonResult) const;

        /// @brief warns about unsaved changes in additionals and gives the user the option to abort
        GNEDialog::Result askSaveAdditionalElements(GNEDialog::Result &commonResult) const;

        /// @brief warns about unsaved changes in demand elements and gives the user the option to abort
        GNEDialog::Result askSaveDemandElements(GNEDialog::Result &commonResult) const;

        /// @brief warns about unsaved changes in data elements and gives the user the option to abort
        GNEDialog::Result askSaveDataElements(GNEDialog::Result &commonResult) const;

        /// @brief warns about unsaved changes in meanData elements and gives the user the option to abort
        GNEDialog::Result askSaveMeanDataElements(GNEDialog::Result &commonResult) const;

        /// @}

    private:
        /// @brief pointer to net
        GNENet* myNet;

        /// @brief flag for SumoConfigSumoConfig saved
        bool mySumoConfigSaved = true;

        /// @brief flag for netedit config saved
        bool myNeteditConfigSaved = true;

        /// @brief flag for network  saved
        bool myNetworkSaved = true;

        /// @brief flag for TLS saved
        bool myTLSSaved = true;

        /// @brief flag for edgeType saved
        bool myEdgeTypeSaved = true;

        /// @brief flag for additional elements saved
        bool myAdditionalSaved = true;

        /// @brief flag for demand elements saved
        bool myDemandElementSaved = true;

        /// @brief flag for data elements saved
        bool myDataElementSaved = true;

        /// @brief flag for meanData elements saved
        bool myMeanDataElementSaved = true;

        /// @brief invalidate default constructor
        SavingStatus() = delete;

        /// @brief Invalidated copy constructor.
        SavingStatus(const SavingStatus&) = delete;

        /// @brief Invalidated assignment operator.
        SavingStatus& operator=(const SavingStatus&) = delete;
    };

    /// @brief class for GNEChange_ReplaceEdgeInTLS
    class GNEChange_ReplaceEdgeInTLS : public GNEChange {
        FXDECLARE_ABSTRACT(GNEChange_ReplaceEdgeInTLS)

    public:
        /// @brief constructor
        GNEChange_ReplaceEdgeInTLS(NBTrafficLightLogicCont& tllcont, NBEdge* replaced, NBEdge* by);

        /// @brief destructor
        ~GNEChange_ReplaceEdgeInTLS();

        /// @brief undo action
        void undo();

        /// @brief redo action
        void redo();

        /// @brief undo name
        std::string undoName() const;

        /// @brief get Redo name
        std::string redoName() const;

        /// @brief wether original and new value differ
        bool trueChange();

    private:
        /// @brief container for traffic light logic
        NBTrafficLightLogicCont& myTllcont;

        /// @brief replaced NBEdge
        NBEdge* myReplaced;

        /// @brief replaced by NBEdge
        NBEdge* myBy;
    };
};
