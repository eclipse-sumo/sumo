/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2016-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    SumoDetector.java
/// @author  Maximiliano Bottazzi
/// @date    2016
/// @version $Id$
///
//
/****************************************************************************/
package de.dlr.ts.lisum.sumo;

import de.dlr.ts.lisum.interfaces.DetectorInterface;
import it.polito.appeal.traci.InductionLoop;
import it.polito.appeal.traci.LastStepVehicleData;
import java.io.IOException;

/**
 *
 * @author @author <a href="mailto:maximiliano.bottazzi@dlr.de">Maximiliano Bottazzi</a>
 */
public class SumoDetector
{
    private final String name;
    private InductionLoop sumoInductionLoop;
    private DetectorInterface cityDetector;
    
        
    /**
     * 
     * @param name
     */
    public SumoDetector(String name /*, VehicleTypes vehicleTypes*/)
    {
        this.name = name;        
    }

    /**
     * 
     * @return 
     */
    @Override
    public String toString()
    {
        return name;
    }
    
    /**
     * 
     * @param cityDetector 
     */
    public void setCityDetector(DetectorInterface cityDetector)
    {
        this.cityDetector = cityDetector;
    }

    /**
     * 
     * @return 
     */
    public String getName()
    {
        return name;
    }

    /**
     * 
     * @param sumoInductionLoop 
     */
    public void setSumoInductionLoop(InductionLoop sumoInductionLoop)
    {
        this.sumoInductionLoop = sumoInductionLoop;
    }
    
    
    private double lastEntryTime = 0L;
    private boolean onlyLeave = false;
    
    /**
     * 
     */
    public void executeSimulationStep()
    {
        if(cityDetector == null)
            return;
        
        try
        {
            for (LastStepVehicleData.InformationPacket ip : sumoInductionLoop.getLastStepVehicleData().getInformationPackets())
            {
                //if(!sumoInductionLoop.getID().toUpperCase().contains("IRD1"))
                //    continue;
                
                //String vehicleId = ip.getVehicleId();
                double entryTime = ip.getEntryTime();
                double leaveTime = ip.getLeaveTime();
                
                //System.out.println("...!..pp...... " + sumoInductionLoop.getID() + " - Id: " +
                //        vehicleId + " - " + entryTime + "    #    " + leaveTime);
                
                if(entryTime == lastEntryTime && leaveTime == -1)
                    continue;
                
                if(!onlyLeave)
                {
                    onlyLeave = true;
                    lastEntryTime = entryTime;
                    
                    entryTime -= ((int)entryTime);
                    entryTime *= 100;
                    
                    cityDetector.addFlanke((int) entryTime);
                    
                    //System.out.println("Entry flanke: " + (int) entryTime);
                }
                
                if(leaveTime != -1)
                {
                    leaveTime -= ((int)leaveTime);
                    leaveTime *= 100;
                    
                    int leaveTimeInt = (int) leaveTime;
                    leaveTimeInt *= -1;
                    
                    if(leaveTimeInt == 0)       //LISA+ expect negative values for outgoing time values
                        leaveTimeInt = -1;
                    
                    cityDetector.addFlanke(leaveTimeInt);
                    
                    lastEntryTime = 0L;
                    onlyLeave = false;
                                        
                    //System.out.println("Leave flanke: " + leaveTimeInt);
                }
            }
        } catch (IOException ex)
        {
            ex.printStackTrace(System.out);
        }
    }
}
