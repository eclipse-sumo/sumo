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
    std::string getId( void );


    /** @brief Emit vehicle that is due.

	Pure virtual method. Concrete ones will try to emit vehicles
	according to the current time.
	@param aNet Reference to the net to get the current time.
    */
    virtual void emit( const MSNet& aNet ) = 0;
     
protected: // protected methods
    /** @brief Constructor called by inherited classes.
	@param aId String-Id of the Source-object
    */
    MSSource( std::string aId );

private:   // private methods
    /// Default constructor.
    MSSource( void );
    
    /// Copy constructor.
    MSSource( const MSSource& );
    
    /// Assignment operator.
    MSSource& operator=( const MSSource& );     

protected: // protected members
    /// Id of the source
    std::string myID;


private:   // private members



};  // end MSSource 


//----------- DO NOT DECLARE OR DEFINE ANYTHING AFTER THIS POINT ------------//
#ifndef DISABLE_INLINE
#include "MSSource.icc"
#endif // DISABLE_INLINE


// $Log$
// Revision 1.1  2002/06/12 19:15:11  croessel
// Initial commit.
//


// Local Variables:
// mode:C++
// End:

#endif // MSSource_H








