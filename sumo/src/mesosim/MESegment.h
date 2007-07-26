/****************************************************************************/
/// @file    MESegment.h
/// @author  Daniel Krajzewicz
/// @date    Tue, May 2005
/// @version $Id: MESegment.h 98 2007-06-07 11:24:49Z behr_mi $
///
// A single mesoscopic segment (cell)
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
#ifndef MESegment_h
#define MESegment_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <algorithm>
#include <utils/common/StdDefs.h>
#include "MEGlobals.h"
#include "MEVehicle.h"
#include <microsim/output/meandata/MSLaneMeanDataValues.h>
#include <microsim/MSEdge.h>
#include <microsim/MSVehicle.h>
#include <microsim/trigger/MSTriggeredRerouter.h>
#include <microsim/MSVehicleControl.h>
#include <utils/common/FileHelpers.h>
#include <utils/iodevices/BinaryInputDevice.h>


        // ===========================================================================
        // enumeration definitions
        // ===========================================================================
        enum MEState {
            Full = 0,
            Jam = 1,
            Free = 2
        };


// ===========================================================================
// some definitions
// ===========================================================================
#define USING_ENTRY_MEASURES 1


// ===========================================================================
// class declarations
// ===========================================================================
class MEInductLoop;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MESegment
 * @brief A single mesoscopic segment (cell)
 */
class MESegment
{
public:
    /// SUMO constructor
    MESegment(MSNet &net, const std::string &id,
              const MSEdge * const parent, MESegment *next,
              SUMOReal l, SUMOReal v,
              SUMOReal tff, SUMOReal tfj, SUMOReal tjf, SUMOReal tjj,
              unsigned int idx, SUMOReal lanesScale, const std::vector<MSEdge*> &nextEdges)
            : myEdge(parent), myNextSegment(next),
            length(l), speed(v), tau_ff(tff), tau_fj(tfj), tau_jf(tjf), tau_jj(tjj),
            index(idx), myOccupancy(0), myLastMeanDataUpdate(-1),
            myID(id)//, myJammedTime(0)//, myLastInserted(0), myLastInserted2(0)
    {
        speed *= (SUMOReal) 1.2;
        state = Free;
        t_block = -1.0;
        nLanes = (SUMOReal) parent->getLanes()->size() * lanesScale;
        tau_ff /= (SUMOReal) nLanes; // Eissfeldt p. 89
        tau_fj /= (SUMOReal) nLanes; // Eissfeldt p. 89
        tau_jf /= (SUMOReal) nLanes; // Eissfeldt p. 89
        tau_jj /= (SUMOReal) nLanes; // Eissfeldt p. 89
        // reset mean data information
        myMeanData.insert(
            myMeanData.end(), net.getNDumpIntervalls(), MSLaneMeanDataValues());
        assert(myOccupancy<length*nLanes*.29||state!=Free);
        //
        if (nLanes>1) {
            for (std::vector<MSEdge*>::const_iterator i=nextEdges.begin(); i!=nextEdges.end(); ++i) {
                myLastCars[*i] = std::vector<MEVehicle*>();//0;
                myLastTBlock[*i] = 0;
            }
        }
    }

    inline void updateInductLoopsOnLeave(SUMOReal speed, SUMOReal length)
    {
        {
            for (std::map<void*, MSLaneMeanDataValues>::iterator i=myDetectorData.begin(); i!=myDetectorData.end(); ++i) {
                (*i).second.speedSum += speed;
                (*i).second.vehLengthSum += length;
                (*i).second.nSamples++;
                (*i).second.nVehEntireLane++;
            }
        }
    }

    inline void updateMeanDataOnLeave(SUMOReal speed, SUMOReal length)
    {
        {
            for (std::vector<MSLaneMeanDataValues>::iterator i=myMeanData.begin(); i!=myMeanData.end(); ++i) {
                (*i).speedSum += speed;
                (*i).vehLengthSum += length;
                (*i).nSamples++;
                (*i).nVehEntireLane++;
            }
        }
    }

    inline void updateInductLoopsOnEnter(SUMOReal speed, SUMOReal length)
    {
        {
            for (std::map<void*, MSLaneMeanDataValues>::iterator i=myDetectorData.begin(); i!=myDetectorData.end(); ++i) {
                (*i).second.speedSum += speed;
                (*i).second.vehLengthSum += length;
                (*i).second.nSamples++;
                (*i).second.nVehEntireLane++;
            }
        }
    }

    inline void updateMeanDataOnEnter(SUMOReal speed, SUMOReal length)
    {
        {
            for (std::vector<MSLaneMeanDataValues>::iterator i=myMeanData.begin(); i!=myMeanData.end(); ++i) {
                (*i).speedSum += speed;
                (*i).vehLengthSum += length;
                (*i).nSamples++;
                (*i).nVehEntireLane++;
            }
        }
    }

    inline bool initialise_(MEVehicle *veh, SUMOReal pos, SUMOTime time, bool &insertToNet)
    {
        if (cars.size()==0||myOccupancy+veh->getSUMOVehicle()->getLength()<=length*nLanes) {
            if (cars.size()==0) {
                insertToNet = true;
            }
            SUMOReal uspeed = (SUMOReal) MIN2(speed, veh->getSUMOVehicle()->getVehicleType().getMaxSpeed());
            SUMOReal trt = (length-pos)/uspeed+time;
            veh->update_tEvent(trt);
            veh->update_segment(this);
            cars.insert(cars.begin(), veh);
            if (myLastCars.size()!=0) {
                MSEdge *fe = (MSEdge*) veh->getSUMOVehicle()->succEdge(1);
                myLastCars[fe].insert(myLastCars[fe].begin(), veh);
                if (myLastCars[fe].size()==1) {
                    insertToNet = true;
                }
            }
            veh->setTLastEntry((SUMOReal) time);
            myOccupancy += (SUMOReal) veh->getSUMOVehicle()->getLength();
            if (myOccupancy<length*nLanes*.29) {
                state = Free;
            } else if (myOccupancy<length*nLanes) {
                state = Jam;
            } else {
                state = Full;
            }
#ifdef USING_ENTRY_MEASURES
            updateInductLoopsOnEnter(uspeed, veh->me_length());
            updateMeanDataOnEnter(uspeed, veh->me_length());
#endif
            assert(myOccupancy<length*nLanes*.29||state!=Free);
            assert(vehicleLengthSum()==myOccupancy);
            return true;
        }
        assert(myOccupancy<length*nLanes*.29||state!=Free);
        assert(vehicleLengthSum()==myOccupancy);
        return false;
    }

    inline bool initialise2(MEVehicle *veh, SUMOReal pos, SUMOTime time, bool &insertToNet)
    {
        if (myOccupancy+veh->getSUMOVehicle()->getLength()<=length*nLanes) {
            SUMOReal uspeed = (SUMOReal) MIN2(speed, veh->getSUMOVehicle()->getVehicleType().getMaxSpeed());
            SUMOReal trt = (length/*-pos*/)/uspeed+time;
            veh->update_segment(this);

            if (myLastCars.size()!=0) {
                MSEdge *fe = (MSEdge*) veh->getSUMOVehicle()->succEdge(1);
                if (myLastCars[fe].size()!=0) {
                    trt = (SUMOReal) MAX2(myLastCars[fe][0]->get_tEvent()+tau_ff, trt);
                }
                veh->update_tEvent(trt);
                myLastCars[fe].insert(myLastCars[fe].begin(), veh);
                if (myLastCars[fe].size()==1) {
                    insertToNet = true;
                }
            } else {
                if (cars.size()!=0) {
                    trt = (SUMOReal) MAX2(cars[0]->get_tEvent()+tau_ff, trt);
                }
                veh->update_tEvent(trt);
                cars.insert(cars.begin(), veh);
                if (cars.size()==1) {
                    insertToNet = true;
                }
            }
            veh->setTLastEntry((SUMOReal) time);
            myOccupancy += (SUMOReal) veh->getSUMOVehicle()->getLength();
            if (myOccupancy<length*nLanes*.29) {
                state = Free;
            } else if (myOccupancy<length*nLanes) {
                state = Jam;
            } else {
                state = Full;
            }
#ifdef USING_ENTRY_MEASURES
            updateInductLoopsOnEnter(uspeed, veh->me_length());
            updateMeanDataOnEnter(uspeed, veh->me_length());
#endif
            assert(myOccupancy<length*nLanes*.29||state!=Free);
            assert(vehicleLengthSum()==myOccupancy);
            return true;
        }
        assert(myOccupancy<length*nLanes*.29||state!=Free);
        assert(vehicleLengthSum()==myOccupancy);
        return false;
    }

    inline MEVehicle *send(MEVehicle *tc, MESegment *next, SUMOReal time)
    {
        assert(myOccupancy<length*nLanes*.29||state!=Free);
        assert(vehicleLengthSum()==myOccupancy);
        if (myLastCars.size()>0) {
            if (time<myLastTBlock[(MSEdge*) tc->getSUMOVehicle()->succEdge(1)]) {
                tc->update_tEvent(myLastTBlock[(MSEdge*) tc->getSUMOVehicle()->succEdge(1)]);
                assert(myOccupancy<length*nLanes*.29||state!=Free);
                assert(vehicleLengthSum()==myOccupancy);
                return tc;
            }
        } else {
            if (time<t_block) {
                tc->update_tEvent(t_block);
                assert(myOccupancy<length*nLanes*.29||state!=Free);
                assert(vehicleLengthSum()==myOccupancy);
                return tc;
            }
        }
        MEVehicle *lc = cars.back();
        assert(cars.size()!=0);
        /* this was the prior version
        cars.pop_back();
        */
        // this is the new one
        if (tc==lc) {
            cars.pop_back();
        } else {
            cars.erase(std::find(cars.begin(), cars.end(), tc));
            lc = tc;
        }
        myOccupancy -= (SUMOReal) lc->getSUMOVehicle()->getLength();
#ifdef USING_LEAVE_MEASURES
        updateInductLoopsOnLeave(length/(SUMOReal)(time-lc->getTLastEntry()), lc->getSUMOVehicle()->getLength());
        updateMeanDataOnLeave(length/(SUMOReal)(time-lc->getTLastEntry()), lc->getSUMOVehicle()->getLength());
#endif
        //
        if (myLastCars.size()>0) {
            assert(myLastCars[(MSEdge*) lc->getSUMOVehicle()->succEdge(1)].size()!=0);
            MEVehicle *v = myLastCars[(MSEdge*) lc->getSUMOVehicle()->succEdge(1)].back();
            assert(myLastCars[(MSEdge*) lc->getSUMOVehicle()->succEdge(1)].back()==lc);
            myLastCars[(MSEdge*) lc->getSUMOVehicle()->succEdge(1)].pop_back();
        }
        if (cars.empty()) {
            state = Free;
            t_block = (SUMOReal) MAX2(time + tau_ff, t_block);
            if (myLastCars.size()>0) {
                myLastTBlock[(MSEdge*) lc->getSUMOVehicle()->succEdge(1)] = (SUMOReal) time + tau_ff;
            }
            assert(myOccupancy==0);
            assert(vehicleLengthSum()==myOccupancy);
            return 0;
        }
        lc = cars.back();
        SUMOReal tb = next->get_timeheadway(state);
        t_block = (SUMOReal) MAX2(t_block, time+tb);
        if (myLastCars.size()>0) {
            myLastTBlock[(MSEdge*) lc->getSUMOVehicle()->succEdge(1)] = (SUMOReal) time + tb;
            lc->update_tEvent((SUMOReal) MAX2(lc->get_tEvent(), time + tb));
        } else {
            lc->update_tEvent((SUMOReal) MAX2(lc->get_tEvent(), t_block));
        }
        if (myOccupancy<length*nLanes*.29) {
            state = Free;
        } else {
            state = Jam;
        }
        assert(myOccupancy<length*nLanes*.29||state!=Free);
        assert(vehicleLengthSum()==myOccupancy);
        return lc;
    }

    inline MEVehicle *sendErasing_(MEVehicle *tc, SUMOReal time)
    {
        assert(myOccupancy<length*nLanes*.29||state!=Free);
        MEVehicle *lc = cars.back();
        assert(cars.size()!=0);
        /* this was the prior version
        cars.pop_back();
        */
        // this is the new one
        if (tc==lc) {
            cars.pop_back();
        } else {
            cars.erase(std::find(cars.begin(), cars.end(), tc));
            lc = tc;
        }
        myOccupancy -= (SUMOReal) lc->getSUMOVehicle()->getLength();
#ifdef USING_LEAVE_MEASURES
        updateInductLoopsOnLeave(length/(SUMOReal)(time-lc->getTLastEntry()), lc->getSUMOVehicle()->getLength());
        updateMeanDataOnLeave(length/(SUMOReal)(time-lc->getTLastEntry()), lc->getSUMOVehicle()->getLength());
#endif
        if (myLastCars.size()>0) {
            assert(myLastCars[(MSEdge*) lc->getSUMOVehicle()->succEdge(1)].back()==lc);
            myLastCars[(MSEdge*) lc->getSUMOVehicle()->succEdge(1)].pop_back();
        }
        if (cars.empty()) {
            state = Free;
            t_block = (SUMOReal) MAX2(time + tau_ff, t_block);
            if (myLastCars.size()>0) {
                myLastTBlock[(MSEdge*) lc->getSUMOVehicle()->succEdge(1)] = (SUMOReal) time + tau_ff;
            }
            assert(myOccupancy==0);
            assert(vehicleLengthSum()==myOccupancy);
            return 0;
        }
        lc = cars.back();
        /*
        SUMOReal tb = next->get_timeheadway(state);
        t_block = (SUMOReal) MAX2(t_block, time+tb);
        lc->update_tEvent((SUMOReal) MAX2(lc->get_tEvent(), t_block));
        !!! eigentlich sollte das sowieso geändert werden - next ist nicht das gleiche!!!
        */
        if (myOccupancy<length*nLanes*.29) {
            state = Free;
        } else {
            state = Jam;
        }
        assert(myOccupancy<length*nLanes*.29||state!=Free);
        assert(vehicleLengthSum()==myOccupancy);
        return lc;
    }


    inline SUMOReal getNormedDensity()
    {
        return (SUMOReal) cars.size() *(SUMOReal) 1000. / (SUMOReal) length;
    }


    inline MEVehicle *getFirstCar(SUMOReal time, MSEdge **begIt)
    {
        if (empty()) {
            return 0;
        }
        if (myLastCars.size()==0) {
            if (cars[0]->get_tEvent()<time) {
                return cars[0];
            }
        } else {
            std::map<MSEdge*, std::vector<MEVehicle*> >::iterator i=myLastCars.begin();
            if (*begIt!=0) {
                i = myLastCars.find(*begIt);
                i++;
            }
            for (; i!=myLastCars.end(); ++i) {
                if ((*i).second.size()!=0) {
                    MEVehicle *poss = (*i).second.back();
                    if (poss->get_tEvent()<time) {
                        *begIt = (*i).first;
                        return poss;
                    }
                }
            }
        }
        return 0;
    }


    inline MEVehicle *receive(MEVehicle *tc, SUMOReal time, MEVehicle **toRemove)
    {
        if (static_cast<MSVehicle*>(tc)->proceedVirtualReturnWhetherEnded(myEdge)) {
            MSNet::getInstance()->getVehicleControl().scheduleVehicleRemoval(static_cast<MSVehicle*>(tc));
            assert(vehicleLengthSum()==myOccupancy);
            return 0;
        }
        checkRerouter(*(tc->getSUMOVehicle()), (SUMOTime) time); // !!! konvertierung checken
        assert(myOccupancy<length*nLanes*.29||state!=Free);
        if (cars.empty()) {
            state = Free;
            SUMOReal uspeed = (SUMOReal) MIN2(speed, tc->getSUMOVehicle()->getVehicleType().getMaxSpeed());
            SUMOReal tleave;
            if (myLastCars.size()>0) {
                tleave = time+length/uspeed;
                myLastTBlock[(MSEdge*) tc->getSUMOVehicle()->succEdge(1)] = tleave;
            } else {
                tleave = (SUMOReal) MAX2(time+length/uspeed, t_block);
                t_block = tleave;
            }
            tc->update_tEvent(tleave);
            tc->update_segment(this);
            cars.push_back(tc);
            tc->setTLastEntry(time);
            myOccupancy += (SUMOReal) tc->getSUMOVehicle()->me_length();
            if (myOccupancy>length*nLanes*.29) {
                state = Jam;
            }
#ifdef USING_ENTRY_MEASURES
            updateInductLoopsOnEnter(length/(tleave-time), tc->me_length());
            updateMeanDataOnEnter(length/(tleave-time), tc->me_length());
#endif
            assert(myOccupancy<length*nLanes*.29||state!=Free);
            if (myLastCars.size()!=0) {
                assert(myLastCars[(MSEdge*) tc->getSUMOVehicle()->succEdge(1)].size()==0);
                myLastCars[(MSEdge*) tc->getSUMOVehicle()->succEdge(1)].push_back(tc);
            }
            assert(vehicleLengthSum()==myOccupancy);
            return tc;
        } else {
            SUMOReal uspeed = (SUMOReal) MIN2(speed, tc->getSUMOVehicle()->getVehicleType().getMaxSpeed());
            SUMOReal tleave = time+length/uspeed;
            MEVehicle *ret = 0;
            if (myLastCars.size()==0) {
                SUMOReal leaderOut = cars[0]->get_tEvent();
                if (leaderOut>tleave) {
                    if (cars.size()==1) {
                        *toRemove = cars[0];
                        ret = tc;
                    }
                    cars.insert(cars.begin()+1, tc);
                } else {
                    tleave = (SUMOReal) MAX2(leaderOut+tau_ff, tleave);
                    cars.insert(cars.begin(), tc);
                }
            } else {
                if (myLastCars[(MSEdge*) tc->getSUMOVehicle()->succEdge(1)].size()==0) {
                    SUMOReal leaderOut = cars[0]->get_tEvent();
                    if (leaderOut>tleave) {
                        if (cars.size()==1) {
                            *toRemove = cars[0];
                            ret = tc;
                        }
                        cars.insert(cars.begin()+1, tc);
                    } else {
                        tleave = (SUMOReal) MAX2(leaderOut+tau_ff, tleave);
                        cars.insert(cars.begin(), tc);
                    }
                    MSEdge *next = (MSEdge*) tc->getSUMOVehicle()->succEdge(1);
                    myLastCars[next].push_back(tc);
                    ret = tc;
                } else {
                    SUMOReal leaderOut = myLastCars[(MSEdge*) tc->getSUMOVehicle()->succEdge(1)][0]->get_tEvent();
                    tleave = (SUMOReal) MAX2(leaderOut+tau_ff, tleave);
                    cars.insert(cars.begin(), tc);
                    MSEdge *next = (MSEdge*) tc->getSUMOVehicle()->succEdge(1);
                    myLastCars[next].insert(myLastCars[next].begin(), tc);
                }
            }
            tc->update_tEvent(tleave);
            tc->update_segment(this);
            tc->setTLastEntry(time);
            myOccupancy += (SUMOReal) tc->getSUMOVehicle()->me_length();
            if (myOccupancy<length*nLanes*.29) {
                state = Free;
            } else if (myOccupancy<length*nLanes) {
                state = Jam;
            } else {
                state = Full;
            }
#ifdef USING_ENTRY_MEASURES
            updateInductLoopsOnEnter(length/(tleave-time), tc->me_length());
            updateMeanDataOnEnter(length/(tleave-time), tc->me_length());
#endif
            assert(vehicleLengthSum()==myOccupancy);
            assert(myOccupancy<length*nLanes*.29||state!=Free);
            return ret;
        }
    }

    inline SUMOReal get_timeheadway(MEState predState)
    {
        assert(vehicleLengthSum()==myOccupancy);
        if (predState!=Free) {
            if (state==Free) {
                return tau_jf;
            } else {
                SUMOReal m = tau_jj;
                SUMOReal b = (SUMOReal) length / (SUMOReal) 7.5/*capacity*/ * (SUMOReal)(tau_jf-m); //!!!
                SUMOReal zw = (SUMOReal) m * (SUMOReal) cars.size() + (SUMOReal) b;
                return zw;
            }
        } else {
            if (state==Free) {
                return tau_ff;
            } else {
                return tau_fj;
            }
        }
    }

    inline MEVehicle *first_car()
    {
        if (cars.size()==0) {
            return 0;
        }
        return cars[cars.size()-1];
    }

    inline size_t noCars()
    {
        return cars.size();
    }

    MEState get_state()
    {
        assert(vehicleLengthSum()==myOccupancy);
        assert(myOccupancy<length*nLanes*.29||state!=Free);
        return state;
    }
    unsigned int get_index()
    {
        return index;
    }

    MESegment *getNextSegment() const
    {
        return myNextSegment;
    }

    inline MSLaneMeanDataValues &getMeanData(int idx)
    {
        assert((int) myMeanData.size()>idx);
        return myMeanData[idx];
    }

    inline MSLaneMeanDataValues &getDetectorData(void *loop, SUMOTime time)
    {
        return myDetectorData[loop];
    }

    inline void flushMeanData(int idx, SUMOTime time)
    {
        assert((int) myMeanData.size()>idx);
        myMeanData[idx].speedSum = 0;
        myMeanData[idx].nSamples = 0;
        myMeanData[idx].vehLengthSum = 0;
        myMeanData[idx].nVehEntireLane = 0;
    }

    inline void flushDetectorData(void *loop, SUMOTime time)
    {
        myDetectorData[loop].speedSum = 0;
        myDetectorData[loop].nSamples = 0;
        myDetectorData[loop].vehLengthSum = 0;
        myDetectorData[loop].nVehEntireLane = 0;
    }

    SUMOReal getLength()
    {
        return length;
    }

    SUMOReal getMaxSpeed()
    {
        return speed;
    }

    void addDetectorData(void *loop, SUMOTime freq, SUMOTime time)
    {
        assert(myDetectorData.find(loop)==myDetectorData.end());
        myDetectorData[loop] = MSLaneMeanDataValues();
    }

    inline SUMOReal getFirstTEvent2(MEVehicle *veh) const
    {
        if (myLastCars.size()!=0) {
            if (myLastCars.find((MSEdge*) veh->getSUMOVehicle()->succEdge(1))->second.size()!=0) {
                return myLastCars.find((MSEdge*) veh->getSUMOVehicle()->succEdge(1))->second.back()->get_tEvent();
            }
            std::map<MSEdge*, std::vector<MEVehicle*> >::const_iterator i;
            for (i=myLastCars.begin(); i!=myLastCars.end(); ++i) {
                if ((SUMOReal)(*i).second.size()!=0) {
                    return (*i).second.back()->get_tEvent();
                }
            }
        }
        return cars.back()->get_tEvent();
    }

    inline SUMOReal getLastTEvent() const
    {
        return cars[0]->get_tEvent();
    }

    inline SUMOReal getTauJJ() const
    {
        return tau_jj;
    }

    inline SUMOReal getTauFF() const
    {
        return tau_ff;
    }

    inline bool empty() const
    {
        return cars.empty();
    }

    inline SUMOReal getLength() const
    {
        return length;
    }

    inline SUMOReal occupancy() const
    {
        return myOccupancy;
    }

    const std::string &getID() const
    {
        return myID;
    }

    inline SUMOReal getDensity() const
    {
        return myOccupancy / length;
    }

    inline SUMOReal getMeanSpeed() const
    {
        assert(myOccupancy<length*nLanes*.29||state!=Free);
        if (cars.size()==0) {
            return speed;
        }
        SUMOReal v = 0;
        for (std::vector<MEVehicle*>::const_iterator i=cars.begin(); i!=cars.end(); ++i) {
            v += (length/(*i)->getNeededTime());
        }
        return v / (SUMOReal) cars.size();
    }

    inline const std::vector<MEVehicle*> &getVehicles() const
    {
        return cars;
    }

    bool checkOccupancy()
    {
        SUMOReal lenSum = 0;
        for (std::vector<MEVehicle*>::const_iterator i=cars.begin(); i!=cars.end(); ++i) {
            lenSum += (*i)->getSUMOVehicle()->getLength();
        }
        return lenSum==myOccupancy;
    }

    inline void eraseCar_(MEVehicle *v)
    {
        assert(myOccupancy<length*nLanes*.29||state!=Free);
        cars.erase(std::find(cars.begin(), cars.end(), v));
        myOccupancy -= v->me_length();
        if (myOccupancy<length*nLanes*.29) {
            state = Free;
        } else if (myOccupancy<length*nLanes) {
            state = Jam;
        } else {
            state = Full;
        }
        if (myLastCars.size()>0) {
            std::vector<MEVehicle*> &lcv = myLastCars[(MSEdge*) v->getSUMOVehicle()->succEdge(1)];
            lcv.erase(std::find(lcv.begin(), lcv.end(), v));
        }
        {
            for (std::map<void*, MSLaneMeanDataValues>::iterator i=myDetectorData.begin(); i!=myDetectorData.end(); ++i) {
                (*i).second.speedSum = MAX2((SUMOReal) 0., (*i).second.speedSum-(length / v->getNeededTime()));
                (*i).second.vehLengthSum = MAX2((SUMOReal) 0., (*i).second.vehLengthSum-v->me_length());
                (*i).second.nSamples = MAX2((SUMOReal) 0, (*i).second.nSamples-1);
                (*i).second.nVehEntireLane = MAX2((unsigned int) 0, (*i).second.nVehEntireLane-1);
            }
        }
        {
            for (std::vector<MSLaneMeanDataValues>::iterator i=myMeanData.begin(); i!=myMeanData.end(); ++i) {
                (*i).speedSum = MAX2((SUMOReal) 0, (*i).speedSum-(length / v->getNeededTime()));
                (*i).vehLengthSum = MAX2((SUMOReal) 0, (*i).vehLengthSum-v->me_length());
                (*i).nSamples = MAX2((SUMOReal) 0, (*i).nSamples-1);
                (*i).nVehEntireLane = MAX2((unsigned int) 0, (*i).nVehEntireLane-1);
            }
        }
        assert(myOccupancy<length*nLanes*.29||state!=Free);
        assert(vehicleLengthSum()==myOccupancy);
    }

    inline MEVehicle *removeCar2(MEVehicle *v, bool check=true)
    {
        assert(myOccupancy<length*nLanes*.29||state!=Free);
        myOccupancy -= v->me_length();
        if (myOccupancy<length*nLanes*.29) {
            state = Free;
        } else if (myOccupancy<length*nLanes) {
            state = Jam;
        } else {
            state = Full;
        }
        if (myLastCars.size()>0) {
            std::vector<MEVehicle*> &lcv = myLastCars[(MSEdge*) v->getSUMOVehicle()->succEdge(1)];
            if (check) {
                assert(lcv.back()==v);
            }
            assert(std::find(lcv.begin(), lcv.end(), v)!=lcv.end());
            lcv.erase(std::find(lcv.begin(), lcv.end(), v));
            assert(myOccupancy<length*nLanes*.29||state!=Free);
            assert(vehicleLengthSum()==myOccupancy);
            if (lcv.size()==0) {
                assert(vehicleLengthSum()==myOccupancy);
                assert(myOccupancy<length*nLanes*.29||state!=Free);
                return 0;
            }
            return lcv.back();
        } else {
            if (check) {
                assert(cars.back()==v);
            }
            assert(std::find(cars.begin(), cars.end(), v)!=cars.end());
            cars.erase(std::find(cars.begin(), cars.end(), v));
            assert(myOccupancy<length*nLanes*.29||state!=Free);
            assert(vehicleLengthSum()==myOccupancy);
            if (cars.size()==0) {
                assert(vehicleLengthSum()==myOccupancy);
                assert(myOccupancy<length*nLanes*.29||state!=Free);
                return 0;
            }
            return cars.back();
        }
    }

    inline bool overtake()
    {
        return ((double) rand() / (double) RAND_MAX) > (myOccupancy / (length*nLanes));
    }


    inline MEVehicle *receive2(MEVehicle *tc, SUMOReal time, MEVehicle **toRemove)
    {
        if (static_cast<MSVehicle*>(tc)->proceedVirtualReturnWhetherEnded(myEdge)) {
            MSNet::getInstance()->getVehicleControl().scheduleVehicleRemoval(static_cast<MSVehicle*>(tc));
            assert(vehicleLengthSum()==myOccupancy);
            assert(myOccupancy<length*nLanes*.29||state!=Free);
            return 0;
        }
        checkRerouter(*(tc->getSUMOVehicle()), (SUMOTime) time); // !!! konvertierung checken
        assert(myOccupancy<length*nLanes*.29||state!=Free);
        if (cars.empty()&&myLastCars.size()==0) {
            state = Free;
            SUMOReal uspeed = (SUMOReal) MIN2(speed, tc->getSUMOVehicle()->getVehicleType().getMaxSpeed());
            SUMOReal tleave;
            tleave = (SUMOReal) MAX2(time+length/uspeed, t_block);
            t_block = tleave;
            cars.push_back(tc);
            tc->update_tEvent(tleave);
            tc->update_segment(this);
            tc->setTLastEntry(time);
            myOccupancy += (SUMOReal) tc->getSUMOVehicle()->me_length();
            if (myOccupancy>length*nLanes*.29) {
                state = Jam;
            }
#ifdef USING_ENTRY_MEASURES
            updateInductLoopsOnEnter(length/(tleave-time), tc->me_length());
            updateMeanDataOnEnter(length/(tleave-time), tc->me_length());
#endif
            assert(myOccupancy<length*nLanes*.29||state!=Free);
            assert(vehicleLengthSum()==myOccupancy);
            return tc;
        } else {
            SUMOReal uspeed = (SUMOReal) MIN2(speed, tc->getSUMOVehicle()->getVehicleType().getMaxSpeed());
            SUMOReal tleave = time+length/uspeed;
            MEVehicle *ret = 0;
            if (myLastCars.size()==0) {
                SUMOReal leaderOut = cars[0]->get_tEvent();
                if (leaderOut>tleave&&overtake()) {
                    if (cars.size()==1) {
                        *toRemove = cars[0];
                        ret = tc;
                    }
                    cars.insert(cars.begin()+1, tc);
                } else {
                    tleave = (SUMOReal) MAX2(leaderOut+tau_ff, tleave);
                    cars.insert(cars.begin(), tc);
                }
            } else {
                if (myLastCars[(MSEdge*) tc->getSUMOVehicle()->succEdge(1)].size()==0) {
                    tleave = time+length/uspeed;
                    myLastTBlock[(MSEdge*) tc->getSUMOVehicle()->succEdge(1)] = tleave;
                    assert(myLastCars[(MSEdge*) tc->getSUMOVehicle()->succEdge(1)].size()==0);
                    myLastCars[(MSEdge*) tc->getSUMOVehicle()->succEdge(1)].push_back(tc);
                    ret = tc;
                } else {
                    SUMOReal leaderOut = myLastCars[(MSEdge*) tc->getSUMOVehicle()->succEdge(1)][0]->get_tEvent();
                    if (leaderOut>tleave&&overtake()) {
                        if (myLastCars[(MSEdge*) tc->getSUMOVehicle()->succEdge(1)].size()==1) {
                            *toRemove = myLastCars[(MSEdge*) tc->getSUMOVehicle()->succEdge(1)][0];
                            ret = tc;
                        }
                        MSEdge *next = (MSEdge*) tc->getSUMOVehicle()->succEdge(1);
                        myLastCars[next].insert(myLastCars[next].begin()+1, tc);
                    } else {
                        tleave = (SUMOReal) MAX2(leaderOut+tau_ff, tleave);
                        MSEdge *next = (MSEdge*) tc->getSUMOVehicle()->succEdge(1);
                        myLastCars[next].insert(myLastCars[next].begin(), tc);
                    }
                }
            }
            tc->update_tEvent(tleave);
            tc->update_segment(this);
            tc->setTLastEntry(time);
            myOccupancy += (SUMOReal) tc->getSUMOVehicle()->me_length();
            if (myOccupancy<length*nLanes*.29) {
                state = Free;
            } else if (myOccupancy<length*nLanes) {
                state = Jam;
            } else {
                state = Full;
            }
#ifdef USING_ENTRY_MEASURES
            updateInductLoopsOnEnter(length/(tleave-time), tc->me_length());
            updateMeanDataOnEnter(length/(tleave-time), tc->me_length());
#endif
            assert(vehicleLengthSum()==myOccupancy);
            assert(myOccupancy<length*nLanes*.29||state!=Free);
            return ret;
        }
    }


    inline MEVehicle *eraseRandomCar2()
    {
        for (std::vector<MEVehicle*>::reverse_iterator i=cars.rbegin(); i!=cars.rend(); ++i) {
            MEVehicle *v = *i;
            // !!! patch for Soccer
            if (v->getSUMOVehicle()->getID().substr(0, 2)=="e_") {
                continue;
            }
            {
                for (std::map<void*, MSLaneMeanDataValues>::iterator i=myDetectorData.begin(); i!=myDetectorData.end(); ++i) {
                    (*i).second.speedSum = MAX2((SUMOReal) 0, (*i).second.speedSum-(length / v->getNeededTime()));
                    (*i).second.vehLengthSum = MAX2((SUMOReal) 0, (*i).second.vehLengthSum-v->me_length());
                    (*i).second.nSamples = MAX2((SUMOReal) 0, (*i).second.nSamples-1);
                    (*i).second.nVehEntireLane = MAX2((unsigned int) 0, (*i).second.nVehEntireLane-1);
                }
            }
            {
                for (std::vector<MSLaneMeanDataValues>::iterator i=myMeanData.begin(); i!=myMeanData.end(); ++i) {
                    (*i).speedSum = MAX2((SUMOReal) 0, (*i).speedSum-(length / v->getNeededTime()));
                    (*i).vehLengthSum = MAX2((SUMOReal) 0, (*i).vehLengthSum-v->me_length());
                    (*i).nSamples = MAX2((SUMOReal) 0, (*i).nSamples-1);
                    (*i).nVehEntireLane = MAX2((unsigned int) 0, (*i).nVehEntireLane-1);
                }
            }
            assert(myOccupancy<length*nLanes*.29||state!=Free);
            assert(vehicleLengthSum()==myOccupancy);
            return v;
        }

        for (std::map<MSEdge*, std::vector<MEVehicle*> >::iterator j=myLastCars.begin(); j!=myLastCars.end(); ++j) {
            for (std::vector<MEVehicle*>::reverse_iterator i=(*j).second.rbegin(); i!=(*j).second.rend(); ++i) {
                MEVehicle *v = *i;
                // !!! patch for Soccer
                if (v->getSUMOVehicle()->getID().substr(0, 2)=="e_") {
                    continue;
                }
                {
                    for (std::map<void*, MSLaneMeanDataValues>::iterator i=myDetectorData.begin(); i!=myDetectorData.end(); ++i) {
                        (*i).second.speedSum = MAX2((SUMOReal) 0, (*i).second.speedSum-(length / v->getNeededTime()));
                        (*i).second.vehLengthSum = MAX2((SUMOReal) 0, (*i).second.vehLengthSum-v->me_length());
                        (*i).second.nSamples = MAX2((SUMOReal) 0, (*i).second.nSamples-1);
                        (*i).second.nVehEntireLane = MAX2((unsigned int) 0, (*i).second.nVehEntireLane-1);
                    }
                }
                {
                    for (std::vector<MSLaneMeanDataValues>::iterator i=myMeanData.begin(); i!=myMeanData.end(); ++i) {
                        (*i).speedSum = MAX2((SUMOReal) 0, (*i).speedSum-(length / v->getNeededTime()));
                        (*i).vehLengthSum = MAX2((SUMOReal) 0, (*i).vehLengthSum-v->me_length());
                        (*i).nSamples = MAX2((SUMOReal) 0, (*i).nSamples-1);
                        (*i).nVehEntireLane = MAX2((unsigned int) 0, (*i).nVehEntireLane-1);
                    }
                }
                assert(myOccupancy<length*nLanes*.29||state!=Free);
                assert(vehicleLengthSum()==myOccupancy);
                return v;
            }
        }
        assert(vehicleLengthSum()==myOccupancy);
        assert(myOccupancy<length*nLanes*.29||state!=Free);
        return 0;
    }


    inline MEVehicle *eraseRandomCar2_()
    {
        if (cars.size()==0) {
            return 0;
        }
        for (std::vector<MEVehicle*>::reverse_iterator i=cars.rbegin(); i!=cars.rend(); ++i) {
            MEVehicle *v = *i;
            // !!! patch for Soccer
            if (v->getSUMOVehicle()->getID().substr(0, 2)=="e_") {
                continue;
            }
            assert(myOccupancy<length*nLanes*.29||state!=Free);
            cars.erase(std::find(cars.begin(), cars.end(), v));
            myOccupancy -= v->me_length();
            if (myOccupancy<length*nLanes*.29) {
                state = Free;
            } else if (myOccupancy<length*nLanes) {
                state = Jam;
            } else {
                state = Full;
            }
            if (myLastCars.size()>0) {
                std::vector<MEVehicle*> &lcv = myLastCars[(MSEdge*) v->getSUMOVehicle()->succEdge(1)];
                lcv.erase(std::find(lcv.begin(), lcv.end(), v));
            }
            {
                for (std::map<void*, MSLaneMeanDataValues>::iterator i=myDetectorData.begin(); i!=myDetectorData.end(); ++i) {
                    (*i).second.speedSum = MAX2((SUMOReal) 0, (*i).second.speedSum-(length / v->getNeededTime()));
                    (*i).second.vehLengthSum = MAX2((SUMOReal) 0, (*i).second.vehLengthSum-v->me_length());
                    (*i).second.nSamples = MAX2((SUMOReal) 0, (*i).second.nSamples-1);
                    (*i).second.nVehEntireLane = MAX2((unsigned int) 0, (*i).second.nVehEntireLane-1);
                }
            }
            {
                for (std::vector<MSLaneMeanDataValues>::iterator i=myMeanData.begin(); i!=myMeanData.end(); ++i) {
                    (*i).speedSum = MAX2((SUMOReal) 0, (*i).speedSum-(length / v->getNeededTime()));
                    (*i).vehLengthSum = MAX2((SUMOReal) 0, (*i).vehLengthSum-v->me_length());
                    (*i).nSamples = MAX2((SUMOReal) 0, (*i).nSamples-1);
                    (*i).nVehEntireLane = MAX2((unsigned int) 0, (*i).nVehEntireLane-1);
                }
            }
            assert(myOccupancy<length*nLanes*.29||state!=Free);
            assert(vehicleLengthSum()==myOccupancy);
            return v;
        }
        return 0;
    }


    const MSEdge *getEdge()
    {
        return myEdge;
    }

    void saveState(std::ostream &os, long what)
    {
        assert(myOccupancy<length*nLanes*.29||state!=Free);
        FileHelpers::writeUInt(os, (unsigned int) state);
        FileHelpers::writeFloat(os, t_block);
        FileHelpers::writeFloat(os, myOccupancy);
        FileHelpers::writeFloat(os, myLastMeanDataUpdate);
        FileHelpers::writeUInt(os, cars.size());
        {
            for (std::vector<MEVehicle*>::iterator i=cars.begin(); i!=cars.end(); ++i) {
                FileHelpers::writeString(os, (*i)->getSUMOVehicle()->getID());
            }
        }
        FileHelpers::writeUInt(os, myLastCars.size());
        if (myLastCars.size()>0) {
            {
                for (std::map<MSEdge*, std::vector<MEVehicle*> >::iterator i=myLastCars.begin(); i!=myLastCars.end(); ++i) {
                    FileHelpers::writeString(os, (*i).first->getID());
                    const std::vector<MEVehicle*> &vehs = (*i).second;
                    FileHelpers::writeUInt(os, vehs.size());
                    for (std::vector<MEVehicle*>::const_iterator j=vehs.begin(); j!=vehs.end(); ++j) {
                        FileHelpers::writeString(os, (*j)->getSUMOVehicle()->getID());
                    }
                }
            }
            {
                for (std::map<MSEdge*, SUMOReal >::iterator i=myLastTBlock.begin(); i!=myLastTBlock.end(); ++i) {
                    FileHelpers::writeString(os, (*i).first->getID());
                    FileHelpers::writeFloat(os, (*i).second);
                }
            }
        }
        assert(myOccupancy<length*nLanes*.29||state!=Free);
    }

    std::vector<MEVehicle*> loadState(BinaryInputDevice &bis, long what, MSVehicleControl &vc)
    {
        assert(myOccupancy<length*nLanes*.29||state!=Free);
        std::vector<MEVehicle*> ret;
        unsigned int tstate;
        bis >> tstate;
        state = (MEState) tstate;
        bis >> t_block;
        bis >> myOccupancy;
        bis >> myLastMeanDataUpdate;
        unsigned int noCars;
        bis >> noCars;
        while (noCars>0) {
            std::string id;
            bis >> id;
            MSVehicle *v = vc.getVehicle(id);
            assert(v!=0);
            cars.push_back(v);
            noCars--;
        }
        if (cars.size()!=0) {
            ret.push_back(cars.back());
        }
        unsigned int noFollowEdges;
        bis >> noFollowEdges;
        {
            for (size_t i=0; i<noFollowEdges; i++) {
                std::string edgeID;
                bis >> edgeID;
                MSEdge *e = MSEdge::dictionary(edgeID);
                myLastCars[e] = std::vector<MEVehicle*>();
                assert(e!=0);
                bis >> noCars;
                while (noCars>0) {
                    std::string carID;
                    bis >> carID;
                    MEVehicle *v = vc.getVehicle(carID);
                    assert(v!=0);
                    myLastCars[e].push_back(v);
                    noCars--;
                }
                if (myLastCars[e].size()!=0) {
                    ret.push_back(myLastCars[e].back());
                }
            }
        }
        {
            for (size_t i=0; i<noFollowEdges; i++) {
                std::string edgeID;
                bis >> edgeID;
                MSEdge *e = MSEdge::dictionary(edgeID);
                SUMOReal v;
                bis >> v;
                myLastTBlock[e] = v;
            }
        }
        assert(myOccupancy<length*nLanes*.29||state!=Free);
        return ret;
    }

    void addRerouter(MSTriggeredRerouter *rr)
    {
        myRerouter.push_back(rr);
    }

    inline void checkRerouter(MSVehicle &tc, SUMOTime time)
    {
        if (myRerouter.size()==0) {
            return;
        }
        MSTriggeredRerouter *r = myRerouter[0];
        r->reroute(tc, myEdge);
    }

    void addMeanData(size_t no)
    {
        myMeanData.insert(myMeanData.end(), no, MSLaneMeanDataValues());
    }

    SUMOReal getNoLanes() const
    {
        return nLanes;
    }

    SUMOReal getRouteSpread() const
    {
        if (myLastCars.size()==0) {
            return 1.;
        }
        SUMOReal ret = 0;
        std::map<MSEdge*, std::vector<MEVehicle*> >::const_iterator i;
        for (i=myLastCars.begin(); i!=myLastCars.end(); ++i) {
            if ((SUMOReal)(*i).second.size()>ret) {
                ret = (SUMOReal)(*i).second.size();
            }
        }
        return ret / (SUMOReal) cars.size();
    }

    SUMOReal vehicleLengthSum() const
    {
        SUMOReal ret = 0;
        if (myLastCars.size()!=0) {
            std::map<MSEdge*, std::vector<MEVehicle*> >::const_iterator i;
            for (i=myLastCars.begin(); i!=myLastCars.end(); ++i) {
                for (std::vector<MEVehicle*>::const_iterator j=(*i).second.begin(); j!=(*i).second.end(); ++j) {
                    ret += (*j)->getSUMOVehicle()->getLength();
                }
            }
        } else {
            for (std::vector<MEVehicle*>::const_iterator i=cars.begin(); i!=cars.end(); ++i) {
                ret += (*i)->getSUMOVehicle()->getLength();
            }
        }
        return ret;
    }

    friend class MELoop;

private:
    const MSEdge *myEdge;
    MESegment *myNextSegment;

    std::vector<MEVehicle*> cars;
    MEState state;
    SUMOReal length, speed, tau_ff, tau_fj, tau_jf, tau_jj;
    unsigned int index;
    SUMOReal t_block;
    SUMOReal myOccupancy; // in m
    SUMOReal nLanes;

    std::string myID;

    /** Container of MeanDataValues, one element for each intervall. */
    std::vector< MSLaneMeanDataValues > myMeanData;

    std::map<void*, MSLaneMeanDataValues> myDetectorData;

    std::vector<MSTriggeredRerouter*> myRerouter; // !!! (move reminder)

    SUMOReal myLastMeanDataUpdate;
    MSLaneMeanDataValues myLastMeanData;

    std::map<MSEdge*, std::vector<MEVehicle*> > myLastCars;
    std::map<MSEdge*, SUMOReal > myLastTBlock;

    class by_leave_sorter
    {
    public:
        /// constructor
        explicit by_leave_sorter()
        { }

        int operator()(MEVehicle *p1, MEVehicle *p2)
        {
            if (p1->get_tEvent()>p2->get_tEvent()) {
                if (p1->inserted) {
                    return 1;
                }
                return -1;
            }
            return p1->get_tEvent()>p2->get_tEvent();
        }
    };

};


#endif

/****************************************************************************/
