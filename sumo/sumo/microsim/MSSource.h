#ifndef MSSource_H
#define MSSource_H

//---------------------------------------------------------------------------//
//                        MSSource.h  -  Abstract Base Class for all
//                        types of source-like emitters. 
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

#include <string>
class MSNet;
class MSLane;


/**
   @class MSSource
   @brief Abstract Base Class for all types of source-like emitters.
   
   
*/
class MSSource
{

public:    // public methods
    /// Destructor.
    virtual ~MSSource( void );

    /** @brief Get the id of the source.
	@return The id of the Source-object
    */
    const std::string getId( void ) const;


    /** @brief Emit vehicle that is due.

	Virtual method. Some concrete ones will try to emit vehicles
	according to the current time. Others will use the
	MSEventControl-mechanism. 
	@param aNet Reference to the net to get the current time.
    */
    virtual void emit( const MSNet& aNet ) {};

protected: // protected methods
    /// Default constructor.
    MSSource( void );

    /** @brief Constructor called by some inherited classes.
	@param aId String-Id of the Source-object
	@param aLane Emission Lane
    */
    MSSource( std::string aId, MSLane* aLane );

    void setId( std::string aID );
    void setLane( MSLane* aLane );
    void setPos( double aPos );
    
private:   // private methods
    /// Copy constructor.
    MSSource( const MSSource& );
    
    /// Assignment operator.
    MSSource& operator=( const MSSource& );     

protected: // protected members
    /// Id of the source
    std::string myID;

    /// Emission lane.
    MSLane* myLane;

    /// Position of source on lane.
    double myPos;
     
private:   // private members



};  // end MSSource 


//----------- DO NOT DECLARE OR DEFINE ANYTHING AFTER THIS POINT ------------//
#ifndef DISABLE_INLINE
#include "MSSource.icc"
#endif // DISABLE_INLINE


// $Log$
// Revision 1.4  2002/07/31 17:32:48  roessel
// Initial sourceforge commit.
//
// Revision 1.8  2002/07/23 15:26:06  croessel
// Added member pos and method setPos.
//
// Revision 1.7  2002/07/11 09:02:34  croessel
// Readding file to repository.
//
// Revision 1.5  2002/07/09 18:27:37  croessel
// Constructor parameter changes and new set-methods.
//
// Revision 1.4  2002/07/05 14:49:29  croessel
// Changed member MSLane* to MSLane&.
//
// Revision 1.3  2002/06/18 10:14:44  croessel
// Not needed for release 0.7
//
// Revision 1.2  2002/06/12 19:41:05  croessel
// Added a emission lane.
//
// Revision 1.1  2002/06/12 19:15:11  croessel
// Initial commit.
//


// Local Variables:
// mode:C++
// End:

#endif // MSSource_H








