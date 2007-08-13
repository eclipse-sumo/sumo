/****************************************************************************/
/// @file    RandomDistributor.h
/// @author  Daniel Krajzewicz
/// @date    2005-09-15
/// @version $Id$
///
// missing_desc
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
#ifndef RandomDistributor_h
#define RandomDistributor_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <cassert>
#include <utils/common/RandHelper.h>
#include <utils/common/UtilExceptions.h>

template<class T>
class RandomDistributor
{
public:
    RandomDistributor() : myProb(0)
    { }
    ~RandomDistributor()
    { }

    void add(SUMOReal prob, T val)
    {
        assert(prob>=0);
        myVals.push_back(val);
        myProbs.push_back(prob);
        myProb += prob;
    }

    T get() const
        {
			if(myProb==0) {
				throw OutOfBoundsException();
			}
            SUMOReal prob = randSUMO(myProb);
            for (int i=0; i<(int) myVals.size(); i++) {
                if (prob<myProbs[i]) {
                    return myVals[i];
                }
                prob -= myProbs[i];
            }
            return myVals[myVals.size()-1];
        }

    SUMOReal getOverallProb() const
    {
        return myProb;
    }

    void clear()
    {
        myProb = 0;
        myVals.clear();
        myProbs.clear();
    }

    const std::vector<T> &getVals() const
    {
        return myVals;
    }

    const std::vector<SUMOReal> &getProbs() const
    {
        return myProbs;
    }

private:
    SUMOReal myProb;
    std::vector<T> myVals;
    std::vector<SUMOReal> myProbs;

};


#endif

/****************************************************************************/

