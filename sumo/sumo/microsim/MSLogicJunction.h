/***************************************************************************
                          MSLogicJunction.h  -  Base class for junctions 
                          with one ore more logics.
                             -------------------
    begin                : Wed, 12 Dez 2001
    copyright            : (C) 2001 by Christian Roessel
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
// Revision 1.1  2002/04/08 07:21:23  traffic
// Initial revision
//
// Revision 2.0  2002/02/14 14:43:17  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.4  2002/01/30 16:14:57  croessel
// Made the destructor virtual.
//
// Revision 1.3  2001/12/20 14:30:42  croessel
// using namespace std replaced by std::
//
// Revision 1.2  2001/12/19 16:32:32  croessel
// Changes due to new junction-hierarchy.
//
// Revision 1.1  2001/12/12 17:46:02  croessel
// Initial commit. Part of a new junction hierarchy.
//

#ifndef MSLogicJunction_H
#define MSLogicJunction_H

#include "MSJunction.h"
#include <vector>

/**
 */
class MSLogicJunction : public MSJunction
{
public:
    /// Destructor.
    virtual ~MSLogicJunction();
     
    /** Container for first-vehicle's request. Each element of this
        container represents one particular link from one lane to
        another. */
    typedef std::vector< bool > Request;

    /** Container for the request responds. The respond is
        lane-bound, not link-bound, so the size maybe smaller than
        the RequestCont's one. */
    typedef std::vector< bool > Respond;
     
    /** Return type for lane's drive request. There are drive
        requests (a lane will send a drive request if the first
        vehicle would leave the lane with it's vNext) and brake
        requests (will be send by vehicles using a prioritised link
        if their interaction distance reaches into the succeeding
        lane).
        What is returned? The begin and end iterator of a Request
        representing the lane's suceeding lanes. The order
        corresponds to myLogic and myInLanes. */
    class DriveBrakeRequest 
    {
    public:
        friend class MSLogicJunction;
        friend class MSRightOfWayJunction;
          
        DriveBrakeRequest( Request request,
                           bool driveRequest,
                           bool brakeRequest);
                             
    private:
        Request myRequest;
        bool myDriveRequest;
        bool myBrakeRequest;
              
        DriveBrakeRequest();
    };       
    
protected:

    MSLogicJunction( std::string id );
    
private:
    /// Default constructor.
    MSLogicJunction();
    
    /// Copy constructor.
    MSLogicJunction( const MSLogicJunction& );
    
    /// Assignment operator.
    MSLogicJunction& operator=( const MSLogicJunction& );     
};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "MSLogicJunction.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:










