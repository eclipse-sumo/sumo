/****************************************************************************/
/// @file    NLTriggerBuilder.h
/// @author  Daniel Krajzewicz
/// @date    Thu, 17 Oct 2002
/// @version $Id$
///
// Builds trigger objects for microsim
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
#ifndef NLTriggerBuilder_h
#define NLTriggerBuilder_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <vector>


// ===========================================================================
// class declarations
// ===========================================================================
class MSTrigger;
class MSNet;
class MSLaneSpeedTrigger;
class MSEmitter;
class NLHandler;
class MSTriggeredRerouter;
class MSLane;
class MSEdge;
class MSBusStop;
class MSE1VehicleActor;
class MSCalibrator;

#ifdef HAVE_MESOSIM
class METriggeredCalibrator;
class MESegment;
class METriggeredScaler;
#endif


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NLTriggerBuilder
 * @brief Builds trigger objects for microsim
 *
 * Called on the occurence of a "trigger"-element, "buildTrigger" parses what
 *  kind of a trigger object shall be built and calls an appropriate parsing
 *  and building method.
 *
 * This parsing/building method retrieves the parameter needed to build the trigger,
 *  checks them, and, if they are ok, calls the appropriate building method.
 *
 * The building methods may be overridden, to build guisim-instances of the triggers,
 *  for example.
 *
 */
class NLTriggerBuilder
{
public:
    /// @brief Constructor
    NLTriggerBuilder() throw();


    /// @brief Destructor
    virtual ~NLTriggerBuilder() throw();


    /** @brief builds the specified trigger
     *
     * Determines the type of the trigger to build using a combination
     *  of "objecttype" and "attr" - attributes from the supplied attributes, first.
     *
     * Build the proper trigger using protected member helper methods.
     *
     * @param[in] net The network the trigger shall belong to
     * @param[in] attrs SAX-attributes which define the trigger
     * @param[in] base The base path
     * @return The built trigger
     * @exception InvalidArgument If a parameter is not valid
     * @todo Recheck behaviour if the "objecttype" attribute is not supported or one of the asked parameter is meaningless
     * @todo Recheck usage of the helper class
     */
    MSTrigger *buildTrigger(MSNet &net, const SUMOSAXAttributes &attrs,
                            const std::string &base) throw(InvalidArgument);


    /** @brief Builds a vaporization
     *
     * Parses the attributes, reporting errors if the time values are false 
     *  or the edge is not known. 
     * Instatiates events for enabling and disabling the vaporization otherwise.
     *
     * @param[in] attrs SAX-attributes which define the vaporizer
     * @recheck throwing the exception
     */
    void buildVaporizer(const SUMOSAXAttributes &attrs) throw();


protected:
    /// @name parsing methods
    ///
    /// These methods parse the attributes for each of the described trigger
    ///  and call the according methods to build the trigger
    //@{
    /** @brief Parses his values and builds a lane speed trigger
     *
     * If one of the declaration values is errornous, an InvalidArgument is thrown.
     *
     * If the XML-file parsed during initialisation is errornous, and the
     *  MSLaneSpeedTrigger-constructor throws a ProcessError due to this, this
     *  exception is catched and an InvalidArgument with the message given in the
     *  ProcessError is thrown.
     *
     * @param[in] net The network the lane speed trigger belongs to
     * @param[in] attrs SAX-attributes which define the trigger
     * @param[in] base The base path
     * @return The built lane speed trigger
     * @exception InvalidArgument If a parameter (lane/position) is not valid or the read definition is errornous
     * @see buildLaneSpeedTrigger
     */
    MSLaneSpeedTrigger *parseAndBuildLaneSpeedTrigger(MSNet &net,
            const SUMOSAXAttributes &attrs, const std::string &base) throw(InvalidArgument);


    /** @brief Parses his values and builds an emitter
     *
     * @param[in] net The network the emitter belongs to
     * @param[in] attrs SAX-attributes which define the trigger
     * @param[in] base The base path
     * @return The built emitter
     * @exception InvalidArgument If a parameter (lane/position) is not valid
     */
    MSEmitter *parseAndBuildLaneEmitTrigger(MSNet &net,
                                            const SUMOSAXAttributes &attrs, const std::string &base) throw(InvalidArgument);


    /** @brief Parses his values and builds a rerouter
     *
     * @param[in] net The network the rerouter belongs to
     * @param[in] attrs SAX-attributes which define the trigger
     * @param[in] base The base path
     * @return The built rerouter
     * @exception InvalidArgument If a parameter (edge) is not valid
     */
    MSTriggeredRerouter *parseAndBuildRerouter(MSNet &net,
            const SUMOSAXAttributes &attrs, const std::string &base) throw(InvalidArgument);


    /** @brief Parses his values and builds a bus stop
     *
     * @param[in] net The network the bus stop belongs to
     * @param[in] attrs SAX-attributes which define the trigger
     * @return The built bus stop
     * @exception InvalidArgument If a parameter (lane/position) is not valid
     */
    MSBusStop *parseAndBuildBusStop(MSNet &net,
                                    const SUMOSAXAttributes &attrs) throw(InvalidArgument);


    /** @brief Parses his values and builds a vehicle actor
     *
     * @param[in] net The network the vehicle actor belongs to
     * @param[in] attrs SAX-attributes which define the trigger
     * @return The built vehicle actor
     * @exception InvalidArgument If a parameter (lane/position) is not valid
     */
    MSE1VehicleActor *parseAndBuildVehicleActor(MSNet &net,
            const SUMOSAXAttributes &attrs) throw(InvalidArgument);


#ifndef HAVE_MESOSIM
    /** @brief Parses his values and builds a microscopic calibrator for online simulation
     *
     * @param[in] net The network the calibrator belongs to
     * @param[in] attrs SAX-attributes which define the trigger
     * @param[in] base The base path
     * @return The built calibrator
     * @exception InvalidArgument If a parameter is not valid
     */
    MSCalibrator *parseAndBuildCalibrator(MSNet &net,
                                          const SUMOSAXAttributes &attrs, const std::string &base) throw(InvalidArgument);
#endif


#ifdef HAVE_MESOSIM
    /** @brief Parses his values and builds a mesoscopic calibrator
     *
     * @param[in] net The network the calibrator belongs to
     * @param[in] attrs SAX-attributes which define the trigger
     * @param[in] base The base path
     * @return The built calibrator
     * @exception InvalidArgument If a parameter (edge/position) is not valid
     */
    METriggeredCalibrator *parseAndBuildCalibrator(MSNet &net,
            const SUMOSAXAttributes &attrs, const std::string &base) throw(InvalidArgument);


    /** @brief Parses his values and builds a mesoscopic flow scaler
     *
     * @param[in] net The network the flow scaler belongs to
     * @param[in] attrs SAX-attributes which define the trigger
     * @return The built flow scaler
     * @exception InvalidArgument If a parameter (edge/position) is not valid
     */
    METriggeredScaler *parseAndBuildScaler(MSNet &net,
                                           const SUMOSAXAttributes &attrs) throw(InvalidArgument);
#endif
    //@}


protected:
    /// @name building methods
    ///
    /// Called with parsed values, these methods build the trigger.
    ///
    /// These methods should be overriden for the gui loader in order
    ///  to build visualizable versions of the triggers.
    ///
    /// In most cases, these methods only call the constructor and
    ///  return the so build trigger.
    //@{
    /** @brief Builds a lane speed trigger
     *
     * Simply calls the MSLaneSpeedTrigger constructor.
     *
     * @param[in] net The net the lane speed trigger belongs to
     * @param[in] id The id of the lane speed trigger
     * @param[in] destLanes List of lanes affected by this speed trigger
     * @param[in] file Name of the file to read the speeds to set from
     * @return The built lane speed trigger
     * @see MSLaneSpeedTrigger
     * @exception ProcessError If the XML definition file is errornous
     */
    virtual MSLaneSpeedTrigger *buildLaneSpeedTrigger(MSNet &net,
            const std::string &id, const std::vector<MSLane*> &destLanes,
            const std::string &file) throw(ProcessError);


    /** @brief Builds an emitter
     *
     * Simply calls the MSEmitter constructor.
     *
     * @param[in] net The net the emitter belongs to
     * @param[in] id The id of the emitter
     * @param[in] destLane The lane the emitter is placed on
     * @param[in] pos Position of the emitter on the given lane
     * @param[in] file Name of the file to read the emission definitions from
     * @return The built emitter
     */
    virtual MSEmitter *buildLaneEmitTrigger(MSNet &net,
                                            const std::string &id, MSLane *destLane, SUMOReal pos,
                                            const std::string &file) throw();


    /** @brief Builds a bus stop
     *
     * Simply calls the MSBusStop constructor.
     *
     * @param[in] net The net the bus stop belongs to
     * @param[in] id The id of the bus stop
     * @param[in] lines Names of the bus lines that halt on this bus stop
     * @param[in] lane The lane the bus stop is placed on
     * @param[in] frompos Begin position of the bus stop on the lane
     * @param[in] topos End position of the bus stop on the lane
     * @return The built bus stop
     */
    virtual MSBusStop* buildBusStop(MSNet &net,
                                    const std::string &id, const std::vector<std::string> &lines,
                                    MSLane *lane, SUMOReal frompos, SUMOReal topos) throw();


    /** @brief builds a calibrator for online simulation
     *
     * Simply calls the MSCalibrator constructor.
     *
     * @param[in] net The net the calibrator belongs to
     * @param[in] id The id of the calibrator
     * @param[in] destLane The lane the calibrator is placed on
     * @param[in] pos Position of the calibrator on the given lane
     * @param[in] file Name of the file to read the calibration definitions from
     * @return The built calibrator
     * @todo Recheck and describe parameter
     */
    virtual MSCalibrator *buildLaneCalibrator(MSNet &net,
            const std::string &id, MSLane *destLane, SUMOReal pos,
            const std::string &file) throw();


#ifdef HAVE_MESOSIM
    /** @brief builds a mesoscopic calibrator
     *
     * Simply calls the METriggeredCalibrator constructor.
     *
     * @param[in] net The net the calibrator belongs to
     * @param[in] id The id of the calibrator
     * @param[in] edge The edge the calibrator is placed at
     * @param[in] pos The position on the edge the calibrator lies at
     * @param[in] rfile The file to read routes from
     * @param[in] file The file to read the flows from
     * @return The built mesoscopic calibrator
     * @todo Is the position correct/needed
     * @todo Is the route file still necessary?
     */
    virtual METriggeredCalibrator *buildCalibrator(MSNet &net,
            const std::string &id, MESegment *edge, SUMOReal pos,
            const std::string &rfile, const std::string &file) throw();

    /** @brief builds a scaler
     *
     * Simply calls the METriggeredScaler constructor.
     *
     * @param[in] net The net the scaler belongs to
     * @param[in] id The id of the scaler
     * @param[in] edge The edge the scaler is placed at
     * @param[in] pos The position on the edge the scaler lies at
     * @param[in] scale The scaling amount
     * @return The built mesoscopic scaler
     * @todo Is the position correct/needed
     * @todo Is the route file still necessary?
     */
    virtual METriggeredScaler *buildScaler(MSNet &net,
                                           const std::string &id, MESegment *edge, SUMOReal pos,
                                           SUMOReal scale) throw();
#endif


    /** @brief builds an rerouter
     *
     * Simply calls the MSTriggeredRerouter constructor.
     *
     * @param[in] net The net the rerouter belongs to
     * @param[in] id The id of the rerouter
     * @param[in] edges The edges the rerouter is placed at
     * @param[in] prob The probability the rerouter reoutes vehicles with
     * @param[in] file The file to read the reroute definitions from
     * @return The built rerouter
     */
    virtual MSTriggeredRerouter *buildRerouter(MSNet &net,
            const std::string &id, std::vector<MSEdge*> &edges,
            SUMOReal prob, const std::string &file) throw();


    /** @brief builds a vehicle actor
     *
     * Simply calls the MSE1VehicleActor constructor.
     *
     * @param[in] net The net the actor belongs to
     * @param[in] id The id of the actor
     * @param[in] edges The edges the rerouter is placed at
     * @param[in] prob The probability the rerouter reoutes vehicles with
     * @param[in] file The file to read the reroute definitions from
     * @return The built vehicle actor
     * @todo Recheck usage of TOL-actors
     */
    virtual MSE1VehicleActor *buildVehicleActor(MSNet &net,
            const std::string &id, MSLane *lane, SUMOReal pos,
            unsigned int la, unsigned int cell, unsigned int type) throw();
    //@}


protected:
    /// @name helper method for obtaining and checking values
    /// @{
    /** @brief Helper method to obtain the filename
     *
     * Retrieves "file" from attributes, checks whether it is absolute
     *  and extends it by the given base path if not. Returns this
     *  information.
     *
     * @param[in] attrs The attributes to obtain the file name from
     * @param[in] base The base path (the path the loaded additional file lies in)
     * @return The (expanded) path to the named file
     * @todo Recheck usage of the helper class
     */
    std::string getFileName(const SUMOSAXAttributes &attrs,
                            const std::string &base) throw();


    /** @brief Returns the lane defined by objectid
     *
     * Retrieves the lane id from the given attrs. Tries to retrieve the lane,
     *  throws an InvalidArgument if it does not exist.
     *
     * @param[in] attrs The attributes to obtain the lane id from
     * @param[in] tt The trigger type (for user output)
     * @param[in] tid The trigger id (for user output)
     * @return The named lane if it is known
     * @exception InvalidArgument If the named lane does not exist or a lane is not named
     */
    MSLane *getLane(const SUMOSAXAttributes &attrs, 
                    const std::string &tt, const std::string &tid) throw(InvalidArgument);


    /** @brief returns the position on the lane checking it
     *
     * This method extracts the position, checks whether it shall be mirrored
     *  and checks whether it is within the lane. If not, an error is reported
     *  and a InvalidArgument is thrown.
     *
     * @param[in] attrs The attributes to obtain the position from
     * @param[in] lane The lane the position shall be valid for
     * @param[in] tt The trigger type (for user output)
     * @param[in] tid The trigger id (for user output)
     * @return The position on the lane
     * @exception InvalidArgument If the position is beyond the lane
     */
    SUMOReal getPosition(const SUMOSAXAttributes &attrs, 
                         MSLane *lane, const std::string &tt, const std::string &tid) throw(InvalidArgument);
    /// @}


};


#endif

/****************************************************************************/

