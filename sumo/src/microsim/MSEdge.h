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

#ifndef MSEdge_H
#define MSEdge_H

// $Log$
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

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include "MSNet.h"

class MSLane;
class MSPerson;
class MSLaneChanger;

/**
 */
class MSEdge
{
public:
    friend class XMLOut;
    /** Class to generate XML-output for an edges and all lanes hold by 
        this edge. 
        Usage, e.g.: cout << XMLOut( edge, 4, true) << endl; */
    class XMLOut
    {
    public:
        XMLOut( const MSEdge& obj,
                unsigned indentWidth ,
                bool withChildElemes );
        friend std::ostream& operator<<( std::ostream& os, 
                                         const XMLOut& obj ); 
    private:
        const MSEdge& myObj;
        unsigned myIndentWidth;
        bool myWithChildElemes;
    };    
    
    friend std::ostream& operator<<( std::ostream& os, 
                                     const XMLOut& obj );     

    
    friend class MeanData;
    /** Class to generate mean-data-output for all lanes hold by an
     * edge. Usage, e.g.: cout << MeanData( myEdge, index, interval)
     * << endl; where myEdge is an edge object, index correspond to
     * the lanes and vehicles data-struct and interval is the sample
     * length.  */
    class MeanData
    {
    public:
        MeanData( const MSEdge& obj,
                  unsigned index ,
                  MSNet::Time interval );
        friend std::ostream& operator<<( std::ostream& os, 
                                         const MeanData& obj ); 
    private:
        const MSEdge& myObj;
        unsigned myIndex;
        MSNet::Time myInterval;
    };    
    
    friend std::ostream& operator<<( std::ostream& os, 
                                     const MeanData& obj );    

    
    /// a container specifying container of persons that are waiting for 
    /// the vehicle-type with the id that is used as key
    typedef std::map< std::string, MSNet::PersonCont* > WaitingPersonsCont;


    /// Constructor.
    MSEdge( std::string id );
    
    /// Container for lanes.
    typedef std::vector< MSLane* > LaneCont;
     
    /** Associative container with suceeding edges (keys) and allowed 
        lanes to reach these edges. */
    typedef std::map< const MSEdge*, LaneCont* > AllowedLanesCont;

    /// Destructor.
    ~MSEdge();

    /// Initialize the edge.
    void initialize( AllowedLanesCont* allowed, MSLane* departLane,
                     LaneCont* lanes );
     
    /** Moves (i.e. makes v- and x-updates) all vehicles currently on 
        the edge's lanes (single- or multi-lane-edge), except the
        first ones on each lane. They will be moved by the
        junctions. */ 
    void moveExceptFirstSingle();
    void moveExceptFirstMulti();

    /** Ask edge's lanes about collisions. Shouldn't be neccessary if
        model is implemented correctly. */
    void detectCollisions( MSNet::Time timestep );
     
    /** Get the allowed lanes to reach the destination-edge. If there 
        is no such edge, get 0. Then you are on the wrong edge. */
    const LaneCont* allowedLanes( const MSEdge& destination ) const;

    /** Return the lane from which vehicles which this edge as
        starting point, will depart. Usually the rightmost. */
    MSLane& departLane() const;

    /** Returns the left-lane of lane if there is one, 0 otherwise. */
    MSLane* leftLane( const MSLane* lane ) const;
     
    /// loads waiting persons into vehicles if both are on the same edge
    void loadPersons();
    
    /// processes all persons which public transport system has reached the 
    /// point the leave it
    void unloadPersons( MSNet* net, unsigned int time );

    /** Inserts edge into the static dictionary and returns true if the key
        id isn't already in the dictionary. Otherwise returns false. */
    static bool dictionary( std::string id, MSEdge* edge );

    /** Returns the MSEdge associated to the key id if exists, otherwise
        returns 0. */
    static MSEdge* dictionary( std::string id );

    /** Returns the edge's number of lanes. */
    unsigned int nLanes() const;

    /** Adds a person waiting for a public vehicle */
    void addWaitingForPublicVehicle( MSPerson *person, std::string lineId );

    /** Returns the persons waiting for the public transport vehicle 
        with the given id */
    MSNet::PersonCont* getWaitingPersonsFor( std::string lineId );
    
    friend std::ostream& operator<<( std::ostream& os, const MSEdge& edge );
    
    /** Let the edge's vehicles try to change their lanes. */
    void changeLanes();

protected:

private:
    /// Unique ID.
    std::string myID;
     
    /** Associative container for destination-edge/allowed-lanes
        matching. */
    AllowedLanesCont* myAllowed;

    /** Lane from which vehicles will depart. Usually the rightmost,
        except for those countries which drive on the _wrong_
        side. */
    MSLane* myDepartLane;

    /** Container for the edge's lane. Should be sorted:
        (right-hand-traffic) the more left the lane, the higher the
        container-index. */
    LaneCont* myLanes;
    
    /** This member will do the lane-change. */
    MSLaneChanger* myLaneChanger;

    /** the container for awaited vehicles; the key is the id of the 
        vehicle while the value is a list of persons waiting for it */
    WaitingPersonsCont myWaitingPersons;

    /// Static dictionary to associate string-ids with objects.
    typedef std::map< std::string, MSEdge* > DictType;
    static DictType myDict;

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










