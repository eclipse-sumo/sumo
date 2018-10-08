/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2016-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    Constants.java
/// @author  Maximiliano Bottazzi
/// @date    2016
/// @version $Id$
///
//
/****************************************************************************/
package de.dlr.ts.lisum.simulation;

import de.dlr.ts.lisum.enums.LightColor;
import de.dlr.ts.lisum.interfaces.CityInterface;
import java.util.HashMap;
import java.util.Map;
import de.dlr.ts.lisum.interfaces.ControlUnitInterface;


/**
 *
 * @author @author <a href="mailto:maximiliano.bottazzi@dlr.de">Maximiliano Bottazzi</a>
 */
public class SimulationControlUnits
{
    private final Map<String /*Sumo*/, SimuControlUnit> perSumo = new HashMap<>();
    private final Map<String /*City*/, String /*Sumo*/> perCity = new HashMap<>();
    
    
    /**
     * 
     */
    public SimulationControlUnits()
    {
    }
    
    /**
     * 
     * @param cityName
     * @return      
     * */
    public String getSumoName(String cityName)
    {
        return perCity.get(cityName);
    }
    
    /**
     * 
     * @param configFile
     * @param city     
     */
    public void load(ConfigurationFile configFile, CityInterface city)
    {
        for (ConfigurationFile.CFControlUnit cfcu : configFile)
        {
            ControlUnitInterface controlUnitInter = city.getControlUnit(cfcu.lisaName);
            
            if(controlUnitInter != null)
            {
                SimuControlUnit cuu = new SimuControlUnit(controlUnitInter);
                cuu.load(cfcu);
                
                perSumo.put(cfcu.sumoName, cuu);
                perCity.put(cfcu.lisaName, cfcu.sumoName);
                controlUnitInter.setEnabled(true);
            }
        }
    }    
    
    /**
     * 
     * @param controlUnitSumoName
     * @param signalGroupIndex
     * @return 
     */
    public LightColor getLightColor(String controlUnitSumoName, int signalGroupIndex)
    {
        SimuControlUnit hh = perSumo.get(controlUnitSumoName);
        
        if(hh != null)
            return hh.getColor(signalGroupIndex);
        else
            return LightColor.OFF;
    }
    
    /**
     * 
     */
    private static class SimuControlUnit
    {
        private final Map<Integer, SignalGroup> signalGroups = new HashMap<>();
        private final ControlUnitInterface cui;

        
        /**
         * 
         * @param cui 
         */
        public SimuControlUnit(ControlUnitInterface cui)
        {
            this.cui = cui;
        }

        /**
         * 
         * @param signalGroupIndex
         * @return 
         */
        LightColor getColor(int signalGroupIndex)
        {
            SignalGroup sg = signalGroups.get(signalGroupIndex);

            if(sg == null)
                return LightColor.OFF;
            
            LightColor lightColor = cui.getLightColor(sg.lisa);
            
            if(lightColor == LightColor.OFF)
                if(sg.ifOff != null)
                    return cui.getLightColor(sg.ifOff);
            
            return lightColor;
        }
        
        /**
         * 
         * @param controlUnit 
         */
        void load(ConfigurationFile.CFControlUnit controlUnit)
        {
            for (int i = 0; i < controlUnit.signalGroups.size(); i++)
            {
                ConfigurationFile.CFControlUnit.SignalGroup sg = controlUnit.signalGroups.get(i);
                String string = sg.sumo;
                String[] split = string.split(",");
                
                for (String s : split)
                    signalGroups.put(Integer.valueOf(s), new SignalGroup(sg.lisa, sg.ifOff));
            }
        }
        
        /**
         * 
         */
        class SignalGroup
        {
            String lisa;
            String ifOff;

            public SignalGroup(String lisa, String main)
            {
                this.lisa = lisa;
                this.ifOff = main;
            }
            
        }
    }
}
