#ifndef MSLaneSpeedTrigger_h
#define MSLaneSpeedTrigger_h
//---------------------------------------------------------------------------//
//                        MSLaneSpeedTrigger.h -
//  Class that realises the setting of a lane's maximum speed triggered by
//      values read from a file
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.3  2003/09/23 14:18:15  dkrajzew
// hierarchy refactored; user-friendly implementation
//
// Revision 1.2  2003/02/07 10:41:51  dkrajzew
// updated
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <string>
#include <helpers/Command.h>
#include "MSTriggeredXMLReader.h"
#include "MSTrigger.h"

/* =========================================================================
 * class declarations
 * ======================================================================= */
class MSNet;
class MSLane;

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * MSLaneSpeedTrigger
 * This trigger reads the next maximum velocity of the lane he is
 * responsible for from a file and sets it.
 * Lanes with variable speeds are so possible.
 */
class MSLaneSpeedTrigger : public MSTriggeredXMLReader,
                           public MSTrigger {
public:
    /** constructor */
    MSLaneSpeedTrigger(const std::string &id,
        MSNet &net, MSLane &destLane,
        const std::string &aXMLFilename);

    /** destructor */
    ~MSLaneSpeedTrigger();

    /** the implementation of the MSTriggeredReader-init method */
    void init(MSNet &net);

    /** the implementation of the MSTriggeredReader-processNext method */
    void processNext();

protected:
    /** the implementation of the SAX-handler interface for reading
        element begins */
    void myStartElement(int element, const std::string &name,
        const Attributes &attrs);

    /** the implementation of the SAX-handler interface for reading
        characters */
    void myCharacters(int element, const std::string &name,
        const std::string &chars);

    /** the implementation of the SAX-handler interface for reading
        element ends */
    void myEndElement(int element, const std::string &name);

    bool nextRead();

private:
    /** the lane the trigger is responsible for */
    MSLane &_destLane;

    /** the speed that will be set on the next call */
    double _currentSpeed;

    bool myHaveNext;
};

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "MSLaneSpeedTrigger.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

