/***************************************************************************
                          MSLane.h  -  The place where Vehicles
                          operate. 
                             -------------------
    begin                : Mon, 12 Mar 2001
    copyright            : (C) 2001 by ZAIK http://www.zaik.uni-koeln.de/AFS
    author               : Christian Roessel
    email                : roessel@zpr.uni-koeln.de
 ***************************************************************************/

#ifndef MSLane_H
#define MSLane_H

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// $Log$
// Revision 1.5  2002/04/24 13:06:47  croessel
// Changed signature of void detectCollisions() to void detectCollisions(
// MSNet::Time )
//
// Revision 1.4  2002/04/11 15:25:56  croessel
// Changed float to double.
//
// Revision 1.3  2002/04/11 12:32:07  croessel
// Added new lookForwardState "URGENT_LANECHANGE_WISH" for vehicles that
// may drive beyond the lane but are not on a lane that is linked to
// their next route-edge. A second succLink method, named succLinkSec was
// needed.
//
// Revision 1.2  2002/04/10 16:17:00  croessel
// Added friend detectors.
// Added public id() member-function.
//
// Revision 1.1.1.1  2002/04/08 07:21:23  traffic
// new project name
//
// Revision 2.7  2002/03/20 15:58:32  croessel
// Return to previous revision.
//
// Revision 2.5  2002/03/20 11:11:09  croessel
// Splitted friend from class-declaration.
//
// Revision 2.4  2002/03/13 17:40:59  croessel
// Calculation of rearPos in setLookForwardState() fixed by introducing
// the new member myLastVeh in addition to myLastVehState. We need both,
// the state and the length of the vehicle.
//
// Revision 2.3  2002/03/13 16:56:35  croessel
// Changed the simpleOutput to XMLOutput by introducing nested classes
// XMLOut. Output is now indented.
//
// Revision 2.2  2002/02/27 14:31:57  croessel
// Changed access of "typedef VehCont" to public because it is used by
// MSLaneChanger.
//
// Revision 2.1  2002/02/27 13:14:05  croessel
// Prefixed ostream with "std::".
//
// Revision 2.0  2002/02/14 14:43:16  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.16  2002/02/05 13:51:52  croessel
// GPL-Notice included.
// In *.cpp files also config.h included.
//
// Revision 1.15  2002/02/05 11:48:48  croessel
// Made MSLaneChanger a friend class and provided a temporary
// vehicle-container for lane-changing.
//
// Revision 1.14  2002/01/23 11:16:28  croessel
// emit() rewritten to avoid redundant code and make things clearer. This
// leads to some overloaded emitTry()'s and a new enoughSpace() method.
//
// Revision 1.13  2002/01/16 15:41:42  croessel
// New implementation of emit(). Should be collision free now. Introduced
// a new protected method "bool emitTry()" for convenience.
//
// Revision 1.12  2002/01/09 14:58:42  croessel
// Added MSLane::Link::setPriority( bool ) for StopLights to modify the
// link's priority. Used to implement the "green arrow".
//
// Revision 1.11  2001/12/20 14:28:14  croessel
// using namespace std replaced by std::
//
// Revision 1.10  2001/12/19 16:30:57  croessel
// Changes due to new junction-hierarchy.
//
// Revision 1.9  2001/11/21 15:18:09  croessel
// Renamed buffer2lane() into integrateNewVehicle() and moved it to
// public, so that Junctions can perform this action.
// Introduction of private member myLastState, a temporary that is used
// to return the correct position of the last vehicle to lookForward
// (position before it was moved).
//
// Revision 1.8  2001/11/15 17:17:36  croessel
// Outcommented the inclusion of the inline *.iC files. Currently not needed.
// Vehicle-State introduced. Simulation is now independant of vehicle's
// speed. Still, the position is fundamental, also a gap between
// vehicles. But that's it.
//
// Revision 1.7  2001/11/14 11:45:55  croessel
// Resolved conflicts which appeared during suffix-change and
// CR-line-end commits.
//
// Revision 1.6  2001/11/14 10:49:07  croessel
// CR-line-end removed.
//
// Revision 1.5  2001/10/23 09:31:07  traffic
// parser bugs removed
//
// Revision 1.3  2001/09/06 15:47:08  croessel
// Numerous changes during debugging session.
//
// Revision 1.2  2001/07/16 12:55:47  croessel
// Changed id type from unsigned int to string. Added string-pointer  
// dictionaries and dictionary methods.
//
// Revision 1.1.1.1  2001/07/11 15:51:13  traffic
// new start
//


#include "MSLogicJunction.h"
#include "MSEdge.h"
#include "MSVehicle.h"
#include <deque>
#include <vector>
#include <utility>
#include <map>
#include <string>
#include <iostream>
#include "MSVehicle.h"
#include "MSNet.h"

class MSModel;
class MSLaneChanger;
class MSEmitter;


/**
 */
class MSLane
{
public:
    friend class MSLaneChanger;
    friend class XMLOut;
    friend class MSDetector;
    friend class MSInductLoop;

    /** Class to generate XML-output for an edges and all lanes hold by 
        this edge. 
        Usage, e.g.: cout << XMLOut( edge, 4, true) << endl; */
    class XMLOut
    {
    public:
        XMLOut( const MSLane& obj,
                unsigned indentWidth ,
                bool withChildElemes );
        friend std::ostream& operator<<( std::ostream& os, 
                                         const XMLOut& obj ); 
    private:
        const MSLane& myObj;
        unsigned myIndentWidth;
        bool myWithChildElemes;
    };    
    
    friend std::ostream& operator<<( std::ostream& os, 
                                     const XMLOut& obj ); 
                                     
    /** Links represent the connnection between lanes. */
    class Link 
    {
    public:
        friend class MSLane;
        
        /// Constructor
        Link( MSLane* succLane, bool yield );

        /// Some Junctions need to switch the Priority
        void setPriority( bool prio );
        
        /// Link's destination lane.
        MSLane* myLane;

        /// Links's default right of way, true for right of way links.
        bool myPrio;

        /** Indicator if a vehicle wants to take this link. Set only
            if it's velocity is sufficient. */
        bool myDriveRequest;
        
    protected:
        /** Function object in order to find the requested link out
            of myLinks, if there is a used one. */
        class LinkRequest
        {
        public:
            
            typedef const Link* first_argument_type;
            typedef bool result_type;
               
            result_type operator() ( first_argument_type link ) const;
        };
          
     
    private:
        Link();
    };
     
    /// Container for the lane's links.
    typedef std::vector< Link* > LinkCont;
     
    /// Destructor.
    ~MSLane();

    /** Use this constructor only. Later use initialize to complete
        lane initialization. */
    MSLane( std::string id, double maxSpeed, double length, MSEdge* egde ); 
     
    /** Not all lane-members are known at the time the lane is born,
        above all the pointers to other lanes, so we have to
        initialize later. */ 
    void initialize( MSJunction* backJunction, 
                     LinkCont* succs);

    /** Move (i.e. make v- and x-update) all the lane's vehicles
        except the first one. The first is update by a junction. If
        there are prioritized neighbour lanes, i.e. it's not allowed
        to overtake vehicles on this lane under some speed
        conditions, choose the second form. */
    void moveExceptFirst(); 
    void moveExceptFirst( MSEdge::LaneCont::const_iterator firstNeighLane,
                          MSEdge::LaneCont::const_iterator lastNeighLane );

    /// Check if vehicles are too close.
    void detectCollisions( MSNet::Time timestep ) const;

    /// Emit vehicle into lane if possible.
    bool emit( MSVehicle& newVeh );

//--------------- Methods used by Junctions  ---------------------
         
    /** Clear all request-related data members to be ready for a new
        run. */
    void clearRequest();
     
    /** Sets myFirst, the vehicle in front of the junction if there is one 
        and lets the first vehicle look forward and tell all visited lanes          
        about it's intention. */      
    void setRequest();
     
    /** Returns the all request data previously calculated in
        setRequest. */
    MSLogicJunction::DriveBrakeRequest request() const;

    /** Move first vehicle according to the previously calculated
        next speed if respond is true. This may imply that the first vehicle
        leaves this lane. If repond is false, decelerate towards the lane's
        end. Should only be called, if request was set. */
    void moveFirst( bool respond );
     
    /// Insert buffered vehicle into the real lane.
    void integrateNewVehicle();    
    
    /** Returns the lane that set this lane's request. It is possible
        that a first vehicle looks over more than one junction. These
        junctions need to know which lane's first vehicle requested
        the drive through. */
    MSLane& requestLane() const;

    /** Returns true if first vehicle will have to decelerate more
        than decelFactor*vehicle.decelmax() when a side road vehicle
        is set in front of it. Otherwise decel2much will modify the
        previously calculated next speed of the first vehicle
        according to it's new predecessor. */
    bool decel2much( const MSLane* compete, const MSLane* target,
                     double decelFactor );
     
    /** Returns the first vehicle's succeeding lane. */
    MSLane* firstVehSuccLane();

    /** Returns the first vehicle's lane that follows srcLane on it's
        route. We demand that srcLane is part of the route. */
    MSLane* firstVehSuccLane( const MSLane* srcLane );
     
//-------------- End of junction-used methods --------------------------------     
     

    /// Returns true if there is not a single vehicle on the lane.
    bool empty() const;
     
    /// Returns the lane's maximum speed.
    double maxSpeed() const;
    
    /// Returns the lane's length.
    double length() const;

    /// Returns the lane's Edge.
    const MSEdge& edge() const;

    /** Inserts a MSLane into the static dictionary and returns true
        if the key id isn't already in the dictionary. Otherwise returns
        false. */
    static bool dictionary( std::string id, MSLane* lane );
     
    /** Returns the MSEdgeControl associated to the key id if exists,
        otherwise returns 0. */
    static MSLane* dictionary( std::string id );

    /// inserts all the waitig persons into the vehicles they are waiting 
    /// for if these are on the lane
    void loadPersons( MSEdge::WaitingPersonsCont& cont );

    void unloadPersons( MSNet* net, unsigned int time, 
                        MSEdge::WaitingPersonsCont& cont );


    friend std::ostream& operator<<( std::ostream& os, const MSLane& lane );

    /// Container for vehicles. 
    typedef std::deque< MSVehicle* > VehCont;
    
    /// Returns the objects id.
    string id() { return myID; }

protected:
    /** Function Object for use with Function Adater on vehicle
        containers. */ 
    class PosGreater
    {
    public:
        typedef const MSVehicle* first_argument_type;
        typedef const MSVehicle* second_argument_type;
        typedef bool result_type;

        /** Returns true if position of first vehicle is greater
            then positition of the second one. */
        result_type operator() ( first_argument_type veh1,
                                 second_argument_type veh2 ) const;  
    };


    /** Find nearest vehicle on neighboured lanes (there may be more
        than one) which isn't allowed to be overtaken. Position and
        speed are conditional parameters. Returns veh if there is no
        neigh to regard. */
    VehCont::const_iterator findNeigh(VehCont::const_iterator veh,
                                      MSEdge::LaneCont::const_iterator first,
                                      MSEdge::LaneCont::const_iterator last);

    // Set states and fill the LFLaneContainer.
    void setLookForwardState();
     
    // Is the vehicle alowed to accellerate after slowing down at a yield
    // junction
    bool accelAfterYielding( const MSVehicle& first ) const;

    // Moves vehicles that don't interact with the junction (no break- and no 
    // drive-request). Calculates a desired vnext for the interaction ones 
    // and sets the corresponding Link-requests and calculates the desired
    // destination lane and position.
    void setDriveRequests();   

    /** Return the link that veh will use when it will change from
        the link-source-lane to a lane that is contained in the edge
        which is nRouteSuccs edges away from this lane's edge. */
    MSLane::LinkCont::iterator succLink( MSVehicle& veh,
                                         unsigned int nRouteSuccs,
                                         MSLane& succLinkSource );

    /** Same as succLink, but does not throw any assertions when
        the succeeding link could not be found; returns the
        myLinks.end() instead */      
    MSLane::LinkCont::iterator succLinkSec( MSVehicle& veh,
                                            unsigned int nRouteSuccs,
                                            MSLane& succLinkSource );
     
    /** Insert a vehicle into the lane's vehicle buffer. After
        processing of all MSJunction::moveFirstVehicle, this buffer
        should be appended to myVehicles by buffer2lane(). The
        necessity of this two-stage process depends on the
        data-structure of myVehicles (Avoid simultaneous push and
        pop). */
    void push( MSVehicle* veh );

    /** Returns the first/front vehicle of the lane and removes it
        from the lane. */
    MSVehicle* pop();

    /** Tries to emit veh into lane. there are four kind of possible 
        emits that have to be handled differently: The line is empty,
        emission as last veh (in driving direction) (front insert),
        as first veh (back insert) and between a follower and a leader. 
        True is returned for successful emission. */
    bool emitTry( MSVehicle& veh ); // empty lane insert     
    bool emitTry( MSVehicle& veh, VehCont::iterator leaderIt ); // front ins.     
    bool emitTry( VehCont::iterator followIt, MSVehicle& veh ); // back ins.     
    bool emitTry( VehCont::iterator followIt, MSVehicle& veh, 
                  VehCont::iterator leaderIt ); // in between ins.
 
    /** Checks if there is enough space for emission and sets vehicle-state
        if there is. Common code used by emitTry-methods. Returns true if
        emission is possible. */   
    bool enoughSpace( MSVehicle& veh,
                      double followPos, double leaderPos, double safeSpace );
    
private:
    /// Unique ID.
    std::string myID;
     
    /// The lane's vehicles. The entering vehicles are inserted at the front 
    /// of  this container and the leaving ones leave from the back, e.g. the
    /// vehicle in front of the junction (often called first) is 
    /// myVehicles.back() (if it exists). And if it is an iterator at a 
    /// vehicle, ++it points to the vehicle in front. This is the interaction
    /// vehicle.
    VehCont myVehicles;
    
    /// Container for lane-changing vehicles. After completion of lane-change-
    /// process, the two containers will be swapped.
    VehCont myTmpVehicles;
    
    // Last vehicle's state. When the first vehicles look forward, the other
    // ones are already moved. But we need it's old state.
    MSVehicle::State myLastVehState;
    
    // Last vehicle. When the first vehicles look forward, the other
    // ones are already moved. But we need it's length.    
    MSVehicle* myLastVeh;

    /// Lane-wide speedlimit [m/s]
    double myMaxSpeed;

    /// Lane length [m]
    double myLength;

    /// The lane's edge, for routing only.
    MSEdge* myEdge;

    /** The lane's preceding Junction, for "look back" while lane
        changing. */
    MSJunction* myBackJunction; 

    /** The lane's Links to it's succeeding lanes and the default 
        right-of-way rule, i.e. blocked or not blocked. */
    LinkCont myLinks;
     
    /** Vehicle-buffer for vehicle that was put onto this lane by a
        junction. The  buffer is neccessary, because of competing
        push- and pop-operations on myVehicles during
        Junction::moveFirst() */
    MSVehicle* myVehBuffer;


    /** Lane that set this lane's request. An other lane's vehicle
        might be responsible for this lane's (brake and drive)
        request (this lane has to be empty, of course). */
    MSLane* myRequestLane;

    /** Indicator if a vehicle's look-forward distance (aka
        brake-distance) is beyond this lane. Set brake-request
        for prioritised vehicles only. */
    bool myBrakeRequest;

//-------- LOOK-FORWARD members-----------------------------

    // vehicle in front of the junction ( in driving direction )
    // 0 if there is none.
    MSVehicle* myFirst;
    
    enum LookForwardState { UNDEFINED, FREE_ON_CURR, YIELD_ON_CURR,
                            FREE_ON_SUCC, YIELD_ON_SUCC,
                            PRED_ON_SUCC, BEYOND_DEST,
                            URGENT_LANECHANGE_WISH };

    // This state set in setLookForwardState and used in setDriveRequests
    // to determine the first vehicles desired vnext.
    LookForwardState myLFState;

    // Indictor for special handling of vehicles that can look into their
    // destination edge or beyond. Use it (maybe) in setDriveRequest.
    // False for YIELD-states.
    bool myLFDestReached;

    // Gap to lane-end or predecessor of first vehicle. 0 for free driving 
    // states. 
    double myGap;
     
    // First vehicle's predecessor's state. Default, i.e. State(), if there 
    // is none.     
    MSVehicle::State myPredState;

    // During setLookForwardState, the used Links and visited Lanes are stored 
    // in this LFLinkLane struct for later use in setDriveRequests.      
    struct LFLinkLane
    {
    
        LFLinkLane( Link* link, MSLane* lane ) : 
            myLink( link ), myLane( lane ) { };
        
        // Link that leads to myLane.
        Link*   myLink;
        // Lane, a first car has looked into.
        MSLane* myLane;
        
    };
    
    typedef std::vector< LFLinkLane > LFLinkLanes;
    
    // Container for used Links/visited Lanes during lookForward.
    LFLinkLanes myLFLinkLanes;

    // Desired parameters for first vehicles. Set in setDriveRequests.
    MSLane*          myTargetLane;
    MSVehicle::State myTargetState;
    double            myTargetPos;
//----------------------------------------------------------

   
    /// Static dictionary to associate string-ids with objects.
    typedef std::map< std::string, MSLane* > DictType;
    static DictType myDict;

    /// Default constructor.
    MSLane();

    /// Copy constructor.
    MSLane( const MSLane& );

    /// Assignment operator.
    MSLane& operator=( const MSLane& );

    /**
     * The class that performs the checking whether the current item in 
     * a search is smaller or equal to a time
     */
    class my_equal : public std::unary_function<MSVehicle*, bool> 
    {
    public:
        my_equal( std::string value ) : m_value( value ) {}
        bool operator() ( MSVehicle *arg ) const 
        { 
            return arg->id()==m_value; 
        }
    private:
        std::string m_value;
    };


};


/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

//#ifndef DISABLE_INLINE
//#include "MSLane.iC"
//#endif

#endif

// Local Variables:
// mode:C++
// End:









