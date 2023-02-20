/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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

#include <utils/foxtools/fxheader.h>
#include <foreign/rtree/SUMORTree.h>
#include <netbuild/NBEdge.h>
#include <netbuild/NBTrafficLightLogicCont.h>
#include <netbuild/NBVehicle.h>
#include <netedit/changes/GNEChange.h>
#include <utils/common/IDSupplier.h>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/geom/Boundary.h>
#include <utils/geom/PositionVector.h>
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/gui/globjects/GUIShapeContainer.h>
#include <utils/gui/settings/GUIVisualizationSettings.h>
#include <utils/router/SUMOAbstractRouter.h>
#include <utils/xml/SUMOXMLDefinitions.h>


// ===========================================================================
// class declarations
// ===========================================================================

class NBNetBuilder;
class GNEAdditional;
class GNEDataSet;
class GNEDemandElement;
class GNEApplicationWindow;
class GNEAttributeCarrier;
class GNEConnection;
class GNECrossing;
class GNEJunction;
class GNEEdgeType;
class GNELaneType;
class GNEMeanData;
class GNEEdge;
class GNELane;
class GNENetworkElement;
class GNEPOI;
class GNEPoly;
class GNEUndoList;
class GNEViewNet;

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
        friend class GNEChange_Junction;
        friend class GNEChange_EdgeType;
        friend class GNEChange_Edge;
        friend class GNEChange_Additional;
        friend class GNEChange_Shape;
        friend class GNEChange_TAZElement;
        friend class GNEChange_DemandElement;
        friend class GNEChange_DataSet;
        friend class GNEChange_DataInterval;
        friend class GNEChange_GenericData;
        friend class GNEChange_MeanData;

    public:
        /// @brief constructor
        AttributeCarriers(GNENet* net);

        /// @brief destructor
        ~AttributeCarriers();

        /// @brief remap junction and edge IDs
        void remapJunctionAndEdgeIds();

        /// @brief check if shape of given AC (network element) is around the given shape
        bool isNetworkElementAroundShape(GNEAttributeCarrier* AC, const PositionVector& shape) const;

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

        /// @brief registers a junction in GNENet containers
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
         * @param[in] AC The attribute carrier related with the crossing
         * @param[in] hardFail Whether attempts to retrieve a nonexisting Crossing should result in an exception
         * @throws UnknownElement
         */
        GNECrossing* retrieveCrossing(GNEAttributeCarrier* AC, bool hardFail = true) const;

        /// @brief get crossings
        const std::set<GNECrossing*>& getCrossings() const;

        /// @brief return all selected crossings
        std::vector<GNECrossing*> getSelectedCrossings() const;

        /// @brief insert crossing
        void insertCrossing(GNECrossing* crossing);

        /// @brief delete crossing
        void deleteCrossing(GNECrossing* crossing);

        /// @brief get number of selected crossings
        int getNumberOfSelectedCrossings() const;

        /// @}

        /// @name function for walkingAreas
        /// @{
        /**@brief get WalkingArea by AC
        * @param[in] AC The attribute carrier related with the walkingArea
        * @param[in] hardFail Whether attempts to retrieve a nonexisting WalkingArea should result in an exception
        * @throws UnknownElement
        */
        GNEWalkingArea* retrieveWalkingArea(GNEAttributeCarrier* AC, bool hardFail = true) const;

        /// @brief get walkingAreas
        const std::set<GNEWalkingArea*>& getWalkingAreas() const;

        /// @brief return all selected walkingAreas
        std::vector<GNEWalkingArea*> getSelectedWalkingAreas() const;

        /// @brief insert walkingArea
        void insertWalkingArea(GNEWalkingArea* walkingArea);

        /// @brief delete walkingArea
        void deleteWalkingArea(GNEWalkingArea* walkingArea);

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

        /// @brief registers a edge in GNENet containers
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

        /// @brief registers an edge with GNENet containers
        GNEEdge* registerEdge(GNEEdge* edge);

        /// @brief clear edges
        void clearEdges();

        /// @brief add prefix to all edges
        void addPrefixToEdges(const std::string& prefix);

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

        /**@brief get lane by Attribute Carrier
         * @param[in] AC The attribute carrier related with the lane
         * @param[in] hardFail Whether attempts to retrieve a nonexisting lane should result in an exception
         * @throws UnknownElement
         */
        GNELane* retrieveLane(GNEAttributeCarrier* AC, bool hardFail = true) const;

        /// @brief get lanes
        const std::set<GNELane*>& getLanes() const;

        /// @brief get selected lanes
        std::vector<GNELane*> getSelectedLanes() const;

        /// @brief insert lane
        void insertLane(GNELane* lane);

        /// @brief delete lane
        void deleteLane(GNELane* lane);

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

        /**@brief get connection by Attribute Carrier
         * @param[in] AC The attribute carrier related with the connection
         * @param[in] hardFail Whether attempts to retrieve a nonexisting connection should result in an exception
         * @throws UnknownElement
         */
        GNEConnection* retrieveConnection(GNEAttributeCarrier* AC, bool hardFail = true) const;

        /// @brief get connections
        const std::set<GNEConnection*>& getConnections() const;

        /// @brief get selected connections
        std::vector<GNEConnection*> getSelectedConnections() const;

        /// @brief insert connection
        void insertConnection(GNEConnection* connection);

        /// @brief delete connection
        void deleteConnection(GNEConnection* connection);

        /// @brief get number of selected connections
        int getNumberOfSelectedConnections() const;

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
         * @param[in] hardFail Whether attempts to retrieve a nonexisting additional should result in an exception
         */
        GNEAdditional* retrieveAdditional(GNEAttributeCarrier* AC, bool hardFail = true) const;

        /**@brief Returns the rerouter interval defined by given begin and end
         * @param[in] rerouter ID
         * @param[in] begin SUMOTime begin
         * @param[in] end SUMOTime begin
         */
        GNEAdditional* retrieveRerouterInterval(const std::string& rerouterID, const SUMOTime begin, const SUMOTime end) const;

        /// @brief get additionals
        const std::map<SumoXMLTag, std::set<GNEAdditional*> >& getAdditionals() const;

        /// @brief get selected additionals
        std::vector<GNEAdditional*> getSelectedAdditionals() const;

        /// @brief get selected shapes
        std::vector<GNEAdditional*> getSelectedShapes() const;

        /// @brief get number of additionals
        int getNumberOfAdditionals() const;

        /// @brief clear additionals
        void clearAdditionals();

        /// @brief generate additional id
        std::string generateAdditionalID(SumoXMLTag type) const;

        /// @brief get number of selected additionals (Including POIs, Polygons, TAZs and Wires)
        int getNumberOfSelectedAdditionals() const;

        /// @brief get number of selected pure additionals (Except POIs, Polygons, TAZs and Wires)
        int getNumberOfSelectedPureAdditionals() const;

        /// @brief get number of selected polygons
        int getNumberOfSelectedPolygons() const;

        /// @brief get number of selected POIs
        int getNumberOfSelectedPOIs() const;

        /// @brief get number of selected TAZs
        int getNumberOfSelectedTAZs() const;

        /// @brief get number of selected TAZSources
        int getNumberOfSelectedTAZSources() const;

        /// @brief get number of selected TAZSinks
        int getNumberOfSelectedTAZSinks() const;

        /// @brief get number of selected Wires
        int getNumberOfSelectedWires() const;

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
         * @param[in] id The attribute carrier related with the demand element
         * @param[in] hardFail Whether attempts to retrieve a nonexisting demand element should result in an exception
         */
        GNEDemandElement* retrieveDemandElement(GNEAttributeCarrier* AC, bool hardFail = true) const;

        /// @brief get selected demand elements
        std::vector<GNEDemandElement*> getSelectedDemandElements() const;

        /// @brief get demand elements
        const std::map<SumoXMLTag, std::set<GNEDemandElement*> >& getDemandElements() const;

        /// @brief Return the number of demand elements
        int getNumberOfDemandElements() const;

        /// @brief generate demand element id
        std::string generateDemandElementID(SumoXMLTag tag) const;

        /// @brief get default type
        GNEDemandElement* getDefaultType() const;

        /// @brief clear demand elements
        void clearDemandElements();

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

        /**@brief Returns the named data set
         * @param[in] id The attribute carrier related with the dataSet element
         * @param[in] hardFail Whether attempts to retrieve a nonexisting data set should result in an exception
         */
        GNEDataSet* retrieveDataSet(GNEAttributeCarrier* AC, bool hardFail = true) const;

        /// @brief get demand elements
        const std::set<GNEDataSet*>& getDataSets() const;

        /// @brief generate data set id
        std::string generateDataSetID(const std::string& prefix) const;

        /// @}

        /// @name function for data intervals
        /// @{
        /**@brief Returns the data interval
         * @param[in] id The attribute carrier related with the dataInterval element
         * @param[in] hardFail Whether attempts to retrieve a nonexisting data set should result in an exception
         */
        GNEDataInterval* retrieveDataInterval(GNEAttributeCarrier* AC, bool hardFail = true) const;

        /// @brief get all data intervals of network
        const std::set<GNEDataInterval*>& getDataIntervals() const;

        /// @brief insert data interval
        void insertDataInterval(GNEDataInterval* dataInterval);

        /// @brief delete data interval
        void deleteDataInterval(GNEDataInterval* dataInterval);

        /// @}

        /// @name function for generic datas
        /// @{
        /**@brief Returns the generic data
         * @param[in] id The attribute carrier related with the genericData element
         * @param[in] hardFail Whether attempts to retrieve a nonexisting data set should result in an exception
         */
        GNEGenericData* retrieveGenericData(GNEAttributeCarrier* AC, bool hardFail = true) const;

        /// @brief get selected generic datas
        std::vector<GNEGenericData*> getSelectedGenericDatas() const;

        /// @brief get all generic datas
        const std::map<SumoXMLTag, std::set<GNEGenericData*> >& getGenericDatas() const;

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

        /// @brief insert generic data
        void insertGenericData(GNEGenericData* genericData);

        /// @brief delete generic data
        void deleteGenericData(GNEGenericData* genericData);

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

        /**@brief Returns the named meanData
         * @param[in] id The attribute carrier related with the meanData element
         * @param[in] hardFail Whether attempts to retrieve a nonexisting meanData should result in an exception
         */
        GNEMeanData* retrieveMeanData(GNEAttributeCarrier* AC, bool hardFail = true) const;

        /// @brief get meanDatas
        const std::map<SumoXMLTag, std::set<GNEMeanData*> >& getMeanDatas() const;

        /// @brief get number of meanDatas
        int getNumberOfMeanDatas() const;

        /// @brief clear meanDatas
        void clearMeanDatas();

        /// @brief generate meanData id
        std::string generateMeanDataID(SumoXMLTag type) const;

        /// @}

    protected:
        /// @name Insertion and erasing of GNEJunctions
        /// @{
        /// @brief inserts a single junction into the net and into the underlying netbuild-container
        void insertJunction(GNEJunction* junction);

        /// @brief deletes a single junction
        void deleteSingleJunction(GNEJunction* junction);

        /// @}

        /// @name Insertion and erasing of GNEEdgeTypes
        /// @{
        /// @brief return true if given edgeType exist
        bool edgeTypeExist(const GNEEdgeType* edgeType) const;

        /// @brief inserts a single edgeType into the net and into the underlying netbuild-container
        void insertEdgeType(GNEEdgeType* edgeType);

        /// @brief deletes edgeType
        void deleteEdgeType(GNEEdgeType* edgeType);

        /// @}

        /// @name Insertion and erasing of GNEEdges
        /// @{
        /// @brief inserts a single edge into the net and into the underlying netbuild-container
        void insertEdge(GNEEdge* edge);

        /// @brief deletes a single edge
        void deleteSingleEdge(GNEEdge* edge);

        /// @}

        /// @name Insertion and erasing of GNEAdditionals items
        /// @{

        /// @brief return true if given additional exist
        bool additionalExist(const GNEAdditional* additional) const;

        /**@brief Insert a additional element int GNENet container.
         * @throw processError if route was already inserted
         */
        void insertAdditional(GNEAdditional* additional);

        /**@brief delete additional element of GNENet container
         * @throw processError if additional wasn't previously inserted
         */
        void deleteAdditional(GNEAdditional* additional);

        /// @}

        /// @name Insertion and erasing of GNEDemandElements items
        /// @{

        /// @brief return true if given demand element exist
        bool demandElementExist(GNEDemandElement* demandElement) const;

        /**@brief Insert a demand element element int GNENet container.
         * @throw processError if route was already inserted
         */
        void insertDemandElement(GNEDemandElement* demandElement);

        /**@brief delete demand element element of GNENet container
         * @throw processError if demand element wasn't previously inserted
         */
        void deleteDemandElement(GNEDemandElement* demandElement);

        /// @}

        /// @name Insertion and erasing of data items
        /// @{

        /// @brief return true if given demand element exist
        bool dataSetExist(GNEDataSet* dataSet) const;

        /**@brief Insert a demand element element int GNENet container.
         * @throw processError if route was already inserted
         */
        void insertDataSet(GNEDataSet* dataSet);

        /**@brief delete demand element element of GNENet container
         * @throw processError if demand element wasn't previously inserted
         */
        void deleteDataSet(GNEDataSet* dataSet);

        /// @}

        /// @name Insertion and erasing of GNEMeanDatas items
        /// @{

        /// @brief return true if given meanData exist
        bool meanDataExist(const GNEMeanData* meanData) const;

        /**@brief Insert a meanData element int GNENet container.
         * @throw processError if route was already inserted
         */
        void insertMeanData(GNEMeanData* meanData);

        /**@brief delete meanData element of GNENet container
         * @throw processError if meanData wasn't previously inserted
         */
        void deleteMeanData(GNEMeanData* meanData);

        /// @}

    private:
        /// @brief pointer to net
        GNENet* myNet;

        /// @brief stop index
        int myStopIndex;

        /// @brief map with the ID and pointer to junctions of net
        std::map<std::string, GNEJunction*> myJunctions;

        /// @brief set with crossings
        std::set<GNECrossing*> myCrossings;

        /// @brief set with walkingAreas
        std::set<GNEWalkingArea*> myWalkingAreas;

        /// @brief map with the ID and pointer to edgeTypes of net
        std::map<std::string, GNEEdgeType*> myEdgeTypes;

        /// @brief map with the ID and pointer to edges of net
        std::map<std::string, GNEEdge*> myEdges;

        /// @brief set with lanes
        std::set<GNELane*> myLanes;

        /// @brief set with connetions
        std::set<GNEConnection*> myConnections;

        /// @brief map with the tag and pointer to additional elements of net
        std::map<SumoXMLTag, std::set<GNEAdditional*> > myAdditionals;

        /// @brief map with the tag and pointer to demand elements of net
        std::map<SumoXMLTag, std::set<GNEDemandElement*> > myDemandElements;

        /// @brief set with the ID and pointer to all datasets of net
        std::set<GNEDataSet*> myDataSets;

        /// @brief set with all data intervals of network
        std::set<GNEDataInterval*> myDataIntervals;

        /// @brief map with the tag and pointer to all generic datas
        std::map<SumoXMLTag, std::set<GNEGenericData*> > myGenericDatas;

        /// @brief map with the tag and pointer to meanData elements of net
        std::map<SumoXMLTag, std::set<GNEMeanData*> > myMeanDatas;

        /// @brief Invalidated copy constructor.
        AttributeCarriers(const AttributeCarriers&) = delete;

        /// @brief Invalidated assignment operator.
        AttributeCarriers& operator=(const AttributeCarriers&) = delete;
    };

    /// @brief modul for Saving status
    class SavingStatus {

    public:
        /// @brief constructor
        SavingStatus();

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

    private:
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
