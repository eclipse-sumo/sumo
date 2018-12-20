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
/// @author  Jakob Erdmann
/// @date    2016
/// @version $Id$
///
//
/****************************************************************************/
package de.dlr.ts.lisum.simulation;

import de.dlr.ts.commons.logger.DLRLogger;
import de.dlr.ts.lisum.enums.LightColor;
import de.dlr.ts.lisum.interfaces.CityInterface;
import java.util.HashMap;
import java.util.ArrayList;
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
    private final static Map<String /*complexCode*/, LightColor> COMPLEX_CODES = new HashMap<>();
    
    
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
                SimuControlUnit cuu = new SimuControlUnit(controlUnitInter, cfcu.sumoName);
                cuu.load(cfcu);
                
                perSumo.put(cfcu.sumoName, cuu);
                perCity.put(cfcu.lisaName, cfcu.sumoName);
                controlUnitInter.setEnabled(true);
            }
        }
        // initialization states
        COMPLEX_CODES.put("O", LightColor.OFF);
        COMPLEX_CODES.put("OO", LightColor.OFF);
        COMPLEX_CODES.put("OOO", LightColor.OFF);
        COMPLEX_CODES.put("OoO", LightColor.OFF);
        
        COMPLEX_CODES.put("r", LightColor.RED);
        COMPLEX_CODES.put("o", LightColor.YELLOW_BLINKING);
        COMPLEX_CODES.put("y", LightColor.YELLOW);
        COMPLEX_CODES.put("u", LightColor.RED_YELLOW);
        COMPLEX_CODES.put("G", LightColor.GREEN);
        COMPLEX_CODES.put("g", LightColor.DARK_GREEN);

        COMPLEX_CODES.put("rO", LightColor.RED);
        COMPLEX_CODES.put("Or", LightColor.RED);
        COMPLEX_CODES.put("ro", LightColor.RED);
        COMPLEX_CODES.put("ry", LightColor.YELLOW);
        COMPLEX_CODES.put("yr", LightColor.YELLOW);
        COMPLEX_CODES.put("yO", LightColor.YELLOW);
        COMPLEX_CODES.put("yo", LightColor.YELLOW);
        COMPLEX_CODES.put("Oy", LightColor.YELLOW);
        COMPLEX_CODES.put("uo", LightColor.RED_YELLOW);
        COMPLEX_CODES.put("uO", LightColor.RED_YELLOW);
        COMPLEX_CODES.put("Ou", LightColor.RED_YELLOW);
        COMPLEX_CODES.put("Go", LightColor.DARK_GREEN);
        COMPLEX_CODES.put("go", LightColor.DARK_GREEN);
        COMPLEX_CODES.put("gO", LightColor.DARK_GREEN);
        COMPLEX_CODES.put("OG", LightColor.GREEN);
        COMPLEX_CODES.put("Gr", LightColor.GREEN);
        COMPLEX_CODES.put("GO", LightColor.GREEN);
        COMPLEX_CODES.put("Gu", LightColor.GREEN);

        COMPLEX_CODES.put("rro", LightColor.RED);
        COMPLEX_CODES.put("rrO", LightColor.RED);
        COMPLEX_CODES.put("rGo", LightColor.RED);
        COMPLEX_CODES.put("rGO", LightColor.RED);
        COMPLEX_CODES.put("rOO", LightColor.RED);
        COMPLEX_CODES.put("roO", LightColor.RED);
        COMPLEX_CODES.put("yOO", LightColor.YELLOW);
        COMPLEX_CODES.put("yyo", LightColor.YELLOW);
        COMPLEX_CODES.put("yyO", LightColor.YELLOW);
        COMPLEX_CODES.put("yoO", LightColor.OFF);
        COMPLEX_CODES.put("uOO", LightColor.RED_YELLOW);
        COMPLEX_CODES.put("uoO", LightColor.RED_YELLOW);
        COMPLEX_CODES.put("ruO", LightColor.RED_YELLOW);
        COMPLEX_CODES.put("ruo", LightColor.RED_YELLOW);
        COMPLEX_CODES.put("OGo", LightColor.DARK_GREEN);
        COMPLEX_CODES.put("GoO", LightColor.DARK_GREEN);
        COMPLEX_CODES.put("goO", LightColor.DARK_GREEN);
        COMPLEX_CODES.put("GoG", LightColor.GREEN);
        COMPLEX_CODES.put("GOG", LightColor.GREEN);


        // this is a hack for halle_vital.net intersection A
        COMPLEX_CODES.put("yG", LightColor.GREEN); 
        //complexCodes.put("GOO", LightColor.DARK_GREEN);

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
        private final Map<Integer, ArrayList<SignalGroup>> signalGroups = new HashMap<>();
        private final ControlUnitInterface cui;
        private final String sumoLogicName;

        
        /**
         * 
         * @param cui 
         */
        public SimuControlUnit(ControlUnitInterface cui, String sumoLogicName)
        {
            this.cui = cui;
            this.sumoLogicName = sumoLogicName;
        }

        /**
         * 
         * @param signalGroupIndex
         * @return 
         */
        private LightColor getColor(int signalGroupIndex)
        {
            int debugIndex = -1;
            ArrayList<SignalGroup> sgs = signalGroups.get(signalGroupIndex);            

            if(sgs == null || sgs.isEmpty())
                return LightColor.OFF;
            
            //SignalGroup defaultSG = sgs.get(0);
            
            String complexCode = "";
            for (SignalGroup sg : sgs) {
                if (signalGroupIndex == debugIndex) {
                    DLRLogger.info("  buildComplexState sg.lisa=" + sg.lisa + " ocit=" + 
                            cui.getLightColor(sg.lisa).getOcitCode());
                }                                
                
                complexCode += cui.getLightColor(sg.lisa).getSumoCode();
            }

            LightColor lightColor = COMPLEX_CODES.get(complexCode);
            if (signalGroupIndex == debugIndex) {
                DLRLogger.info("sumoLogic=" + sumoLogicName + " sumoIndex=" + 
                        signalGroupIndex + " complex=" + complexCode + " lc=" + lightColor);
            }
            if (lightColor == null) {
                DLRLogger.info("sumoLogic=" + sumoLogicName + " sumoIndex=" + signalGroupIndex + 
                        " complex=" + complexCode + " could not be interpreted");
                
                lightColor = LightColor.OFF;
            }
            
            if(lightColor.isOff() && !sgs.get(0).ifOff.isEmpty())
                lightColor = cui.getLightColor(sgs.get(0).ifOff);            
            
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
                
                for (String s : split) {
                    if(s.trim().isEmpty())
                        continue;
                    
                    int sumoIndex = Integer.valueOf(s);
                    if (!signalGroups.containsKey(sumoIndex)) {
                        signalGroups.put(sumoIndex, new ArrayList<SignalGroup>());
                    }
                    signalGroups.get(sumoIndex).add(new SignalGroup(sg.lisa, sg.ifOff));
                }
            }
        }
        
        /**
         * 
         */
        private class SignalGroup
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
