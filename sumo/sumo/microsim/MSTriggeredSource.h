#ifndef MSTriggeredSource_H
#define MSTriggeredSource_H

//---------------------------------------------------------------------------//
//                        MSTriggeredSource.h  -  Concrete Source that
//                        reads emit-times and -speeds from a file and
//                        chooses the routes according to a
//                        distribution. 
//                           -------------------
//  begin                : Wed, 12 Jun 2002
//  copyright            : (C) 2002 by Christian Roessel
//  organisation         : DLR/IVF http://ivf.dlr.de
//  email                : roessel@zpr.uni-koeln.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//          
//   This program is free software; you can redistribute it and/or modify  
//   it under the terms of the GNU General Public License as published by  
//   the Free Software Foundation; either version 2 of the License, or     
//   (at your option) any later version.                                   
//
//---------------------------------------------------------------------------//

#include "MSNet.h"
#include "MSSource.h"

/**
   @class MSTriggeredSource
   @brief Concrete Source that reads emit-times and -speeds from a 
   file and chooses the routes according to a distribution.   
   @see MSSource
   
*/
class MSTriggeredSource : public MSSource
{
public:    // public methods
    /** @brief Struct RouteDistributionElement provides elements to build @ref
	RouteDistribution.
	
	Struct RouteDistributionElement contains the a Route-pointer
	and a frequency of this route. The emitted vehicles will be
	distributed according to this frequency.
    */
    struct RouteDistributionElement 
    {
	/// Pointer to route. Shouldn't be 0.
	const MSNet::Route* myRoute;

	/** @brief Route-pointers frequency

	    Frequency with which the vehicles will get the
	    corresponding route-pointer. Is not bound to a maximum
	    value, but should be > 0.
	*/
	double              myFrequency;
    };

    
    /** @brief Container for @ref 
	RouteDistributionElement.
    */
    typedef std::vector< RouteDistributionElement > RouteDistribution;



    /** @brief Customized construtor
	
	@param aId String-id of the TriggeredSource object.
	@param aFilename Name of the file that contains the starting 
	times and speeds.
	@param aRouteDist RouteDistribution-container, should not be 
	empty. The sum of @ref myFrequency is not bound 
	to a fixed value.
	@param isValidated Flag, if input-file is validated. If not, 
	source won't work.
    */
    MSTriggeredSource(
	std::string        aId,
	const char*        aFilename,
	RouteDistribution* aRouteDist,
	bool               isValidated );
    
    
    /// Destructor.
    ~MSTriggeredSource( void );


    /** @brief Emit vehicle that is due.

	Try to emit the scheduled vehicle if it is time. If emission
	fails, try again next time, if emission succeeds get new
	vehicle from file.
	@param aNet Reference to the net to get the current time.
    */
    virtual void emit( const MSNet& aNet );
    
protected: // protected methods



private:   // private methods
    /// Default constructor.
    MSTriggeredSource( void );
    
    /// Copy constructor.
    MSTriggeredSource( const MSTriggeredSource& );
    
    /// Assignment operator.
    MSTriggeredSource& operator=( MSTriggeredSource& );     

protected: // protected members



private:   // private members



};  // end MSTriggeredSource 


//----------- DO NOT DECLARE OR DEFINE ANYTHING AFTER THIS POINT ------------//
#ifndef DISABLE_INLINE
#include "MSTriggeredSource.icc"
#endif // DISABLE_INLINE


// $Log$
// Revision 1.1  2002/06/12 19:15:12  croessel
// Initial commit.
//


// Local Variables:
// mode:C++
// End:

#endif // MSTriggeredSource_H








