#ifndef MSEdge_H
#define MSEdge_H
/***************************************************************************
                          MSEdge.h  -  Provides routing. Superior to Lanes.
                             -------------------
    begin                : Mon, 12 Mar 2001
    copyright            : (C) 2001 by ZAIK http://www.zaik.uni-koeln.de/AFS
    author               : Christian Roessel
    email                : roessel@zpr.uni-koeln.de
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
// Revision 1.6  2003/06/04 16:12:05  roessel
// Added methods getEdgeVector and getLanes needed by MSTravelcostDetector.
//
// Revision 1.5  2003/04/15 09:09:14  dkrajzew
// documentation added
//
// Revision 1.4  2003/02/07 10:41:50  dkrajzew
// updated
//
// Revision 1.3  2002/10/17 10:41:12  dkrajzew
// retrival of the id added
//
// Revision 1.2  2002/10/16 16:40:35  dkrajzew
// usage of MSPerson removed; will be reimplemented later
//
// Revision 1.1  2002/10/16 14:48:26  dkrajzew
// ROOT/sumo moved to ROOT/src
//
// Revision 1.3  2002/09/25 17:14:42  roessel
// MeanData calculation and output implemented.
//
// Revision 1.2  2002/04/24 13:06:47  croessel
// Changed signature of void detectCollisions() to void detectCollisions(
// MSNet::Time )
//
// Revision 1.1.1.1  2002/04/08 07:21:23  traffic
// new project name
//
// Revision 2.3  2002/03/20 11:11:08  croessel
// Splitted friend from class-declaration.
//
// Revision 2.2  2002/03/13 16:56:34  croessel
// Changed the simpleOutput to XMLOutput by introducing nested classes
// XMLOut. Output is now indented.
//
// Revision 2.1  2002/02/27 13:14:05  croessel
// Prefixed ostream with "std::".
//
// Revision 2.0  2002/02/14 14:43:13  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.14  2002/02/05 13:51:51  croessel
// GPL-Notice included.
// In *.cpp files also config.h included.
//
// Revision 1.13  2002/02/05 11:42:35  croessel
// Lane-change implemented.
//
// Revision 1.12  2001/12/19 16:51:01  croessel
// Replaced using namespace std with std::
//
// Revision 1.11  2001/12/06 13:08:28  traffic
// minor change
//
// Revision 1.10  2001/11/15 17:12:13  croessel
// Outcommented the inclusion of the inline *.iC files. Currently not
// needed.
//
// Revision 1.9  2001/11/14 11:45:53  croessel
// Resolved conflicts which appeared during suffix-change and
// CR-line-end commits.
//
// Revision 1.8  2001/11/14 10:49:07  croessel
// CR-line-end removed.
//
// Revision 1.7  2001/10/23 09:29:25  traffic
// parser bugs removed
//
// Revision 1.5  2001/09/06 15:29:55  croessel
// Added operator<< to class MSEdge for simple text output.
//
// Revision 1.4  2001/07/18 09:41:02  croessel
// Added public method nLanes() that returns the edge's number of lanes.
//
// Revision 1.3  2001/07/16 12:55:46  croessel
// Changed id type from unsigned int to string. Added string-pointer dictionaries and dictionary methods.
//
// Revision 1.2  2001/07/13 16:09:17  traffic
// Just a test, changed id-type from unsigned int to string.
//
// Revision 1.1.1.1  2001/07/11 15:51:13  traffic
// new start
//

/* =========================================================================
 * included modules
 * ======================================================================= */
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include "MSNet.h"
#include "MSLinkCont.h"


/* =========================================================================
 * class declarations
 * ======================================================================= */
class MSLane;
class MSLaneChanger;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class MSEdge
 * A single connection between two junctions. As there is no certain relationship
 * over junctions, the word "street" or "road" may be ambigous.
 * Holds lanes which are reponsible for vehicle movements.
 */
class MSEdge
{
public:
    /**
     * For different purposes, it is necessary to know whether the edge
     * is a normal street or just a sink or a source
     * This information is represented by values from this enumeration
     */
    enum EdgeBasicFunction {
    	/// the purpose of the edge is not known
        EDGEFUNCTION_UNKNOWN = -1,
	    /// the edge is a normal street
        EDGEFUNCTION_NORMAL = 0,
	    /// the edge is only used for vehicle emission (begin of trips)
        EDGEFUNCTION_SOURCE = 1,
	    /// the edge is only used for vehicle deletion (end of trips)
        EDGEFUNCTION_SINK = 2
    };

public:
    /// for output purposes
    friend class XMLOut;
    /// for access to the dictionary
    friend class GUIEdgeGrid;

    /** Class to generate XML-output for an edges and all lanes hold by
        this edge.
        Usage, e.g.: cout << XMLOut( edge, 4, true) << endl; */
    class XMLOut
    {
    public:
	    /// constructor
        XMLOut( const MSEdge& obj,
                unsigned indentWidth ,
                bool withChildElemes );

	    /** writes xml-formatted information about the edge
            and optionally her lanes */
        friend std::ostream& operator<<( std::ostream& os,
                                         const XMLOut& obj );

    private:
	    /// the edge to format information from
        const MSEdge& myObj;

	    /// the number of indent spaces
        unsigned myIndentWidth;

	    /// information, whether lane information shall also be written
        bool myWithChildElemes;
    };

    /// output operator for XML-raw-output
    friend std::ostream& operator<<( std::ostream& os,
                                     const XMLOut& obj );

    /// for data collection
    friend class MeanData;

    /** Class to generate mean-data-output for all lanes hold by an
     * edge. Usage, e.g.: cout << MeanData( myEdge, index, interval)
     * << endl; where myEdge is an edge object, index correspond to
     * the lanes and vehicles data-struct and interval is the sample
     * length.  */
    class MeanData
    {
    public:
	    /// constructor
        MeanData( const MSEdge& obj,
                  unsigned index ,
                  MSNet::Time interval );

	    /// output operator
        friend std::ostream& operator<<( std::ostream& os,
                                         const MeanData& obj );

    private:
	    /// the edge write information from
        const MSEdge& myObj;

	    /// the index of the information within the lanes' MeanData fields
        unsigned myIndex;

	    /// the output interval (??? ...is already stored in MSLane::MeanData?)
        MSNet::Time myInterval;
    };


    /// output operator for XML-mean-data output
    friend std::ostream& operator<<( std::ostream& os,
                                     const MeanData& obj );


    /// Constructor.
    MSEdge( std::string id );

    /// Container for lanes.
    typedef std::vector< MSLane* > LaneCont;

    /** Associative container with suceeding edges (keys) and allowed
        lanes to reach these edges. */
    typedef std::map< const MSEdge*, LaneCont* > AllowedLanesCont;

    /// Destructor.
    virtual ~MSEdge();

    /// Initialize the edge.
    virtual void initialize(
        AllowedLanesCont* allowed, MSLane* departLane, LaneCont* lanes,
        EdgeBasicFunction function);

    /** @brief Moves all vehicles of the edge until the first occures which
        movement shall be rechecked
        See the according lane-function for further information
        Use this for edges with only one lane */
    virtual void moveNonCriticalSingle();


    /** @brief Moves all vehicles the edge, beginning at the first vehicle
        which may leave the edge
        See the according lane-function for further information
        Use this for edges with only one lane */
    virtual void moveCriticalSingle();

    /** @brief Moves all vehicles of the edge until the first occures which
        movement shall be rechecked
        See the according lane-function for further information
        Use this for edges with more than one lane */
    virtual void moveNonCriticalMulti();

    /** @brief Moves all vehicles the edge, beginning at the first vehicle
        which may leave the edge
        See the according lane-function for further information
        Use this for edges with more than one lane */
    virtual void moveCriticalMulti();

    /** @brief moves vehicles which has to be recheked (may leave the lane in
        the near future)
        See the according lane-function for further information */
    virtual void setCritical();

    /** @brief Moves vehicles to their destination lane
        Copies the lane's buffer for incoming vehicles into the lane's vehicle
        vector */
    void vehicle2target();

    /** @brief Ask edge's lanes about collisions.
        Shouldn't be neccessary if model is implemented correctly. */
    void detectCollisions( MSNet::Time timestep );

    /** @brief Get the allowed lanes to reach the destination-edge.
        If there is no such edge, get 0. Then you are on the wrong edge. */
    const LaneCont* allowedLanes( const MSEdge& destination ) const;

    /** Returns the left-lane of lane if there is one, 0 otherwise. */
    MSLane* leftLane( const MSLane* lane ) const;

    /** @brief Inserts edge into the static dictionary
        Returns true if the key id isn't already in the dictionary. Otherwise
        returns false. */
    static bool dictionary( std::string id, MSEdge* edge );

    /** Returns the MSEdge associated to the key id if exists, otherwise
     * returns 0. */
    static MSEdge* dictionary( std::string id );

    /** Clears the dictionary */
    static void clear();

    /** Returns the edge's number of lanes. */
    unsigned int nLanes() const;

    /// outputs the id of the edge
    friend std::ostream& operator<<( std::ostream& os, const MSEdge& edge );

    /** Let the edge's vehicles try to change their lanes. */
    virtual void changeLanes();

    /** returns the id of the edge */
    const std::string &id() const;

    /** @brief Returns the edge type
        Returns information whether the edge is a sink, a source or a
        normal street; see EdgeBasicFunction */
    EdgeBasicFunction getPurpose() const;

    /// returns the information whether the edge is a source
    virtual bool isSource() const;

    /// emits a vehicle on an appropriate lane
    virtual bool emit(MSVehicle &v);

    static std::vector< MSEdge* > getEdgeVector( void );

    LaneCont* getLanes( void );
    

protected:
    /// Unique ID.
    std::string myID;

    /** @brief Container for the edge's lane.
        Should be sorted: (right-hand-traffic) the more left the lane, the
        higher the
        container-index. */
    LaneCont* myLanes;

    /** Associative container for destination-edge/allowed-lanes
        matching. */
    AllowedLanesCont* myAllowed;

    /** @brief Lane from which vehicles will depart.
        Usually the rightmost,
        except for those countries which drive on the _wrong_
        side. */
    MSLane* myDepartLane;

    /** This member will do the lane-change. */
    MSLaneChanger* myLaneChanger;

    /// the purpose of the edge
    EdgeBasicFunction _function;

    /// definition of the static dictionary type
    typedef std::map< std::string, MSEdge* > DictType;

    /// Static dictionary to associate string-ids with objects.
    static DictType myDict;

private:
    /// Default constructor.
    MSEdge();

    /// Copy constructor.
    MSEdge( const MSEdge& );

    /// Assignment operator.
    MSEdge& operator=(const MSEdge&);

};



/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

//#ifndef DISABLE_INLINE
//#include "MSEdge.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:
