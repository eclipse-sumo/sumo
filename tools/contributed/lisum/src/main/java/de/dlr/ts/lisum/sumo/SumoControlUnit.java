/*
 * Copyright (C) 2016
 * Deutsches Zentrum fuer Luft- und Raumfahrt e.V.
 * Institut fuer Verkehrssystemtechnik
 * 
 * German Aerospace Center
 * Institute of Transportation Systems
 * 
 */
package de.dlr.ts.lisum.sumo;

import de.dlr.ts.lisum.enums.LightColor;
import de.dlr.ts.lisum.simulation.SimulationControlUnits;
import it.polito.appeal.traci.TLState;
import java.util.ArrayList;
import java.util.List;
import it.polito.appeal.traci.TrafficLight;
import java.io.IOException;


/**
 *
 * @author @author <a href="mailto:maximiliano.bottazzi@dlr.de">Maximiliano Bottazzi</a>
 */
class SumoControlUnit
{
    private final List<SignalGroup> signalGroups = new ArrayList<>();
    private final String name;
    private TrafficLight trafficLight;
    //private ControlUnitInterface cityControlUnit;
    private SimulationControlUnits controlUnits;
    
    /**
     * 
     */
    public SumoControlUnit(String name, String[] sumoStrings)
    {
        this.name = name;
        int phasesCount = sumoStrings[0].length();
        
        for (int i = 0; i < phasesCount; i++)
        {
            String nodeLine = extractStates(i, sumoStrings);
            
            char greenType = 'G';
            
            for (int j = 0; j < nodeLine.length(); j++)
                if(nodeLine.charAt(j) == 'g')
                    greenType = 'g';

            SignalGroup sg = new SignalGroup(greenType);
            signalGroups.add(sg);
        }
    }

    /**
     * 
     * @param index
     * @param lines
     * @return 
     */
    private String extractStates(int index, String[] lines)
    {
        String tmp = "";
        
        for (String line : lines)
            tmp += line.charAt(index);
        
        return tmp;
    }    
    
    /**
     * 
     * @param controlUnits 
     */
    void setControlUnits(SimulationControlUnits controlUnits)
    {
        this.controlUnits = controlUnits;
    }
    
    /**
     * 
     */
    public void executeSimulationStep()
    {
        for (int j = 0; j < signalGroups.size(); j++)
            signalGroups.get(j).setCurrentLightColor(controlUnits.getLightColor(this.name, j));
        
        set();
    }
    
    
    /**
     * 
     */
    private void set()
    {
        String signalGroupState = "";
        
        for (SignalGroup signalGroup : signalGroups)
        {
            char sumoCurrentLightColor = signalGroup.getSumoCurrentLightColor();
            signalGroupState += sumoCurrentLightColor;
        }
        
        try
        {
            trafficLight.changeLightsState(new TLState(signalGroupState));
        } catch (IOException ex)
        {
            ex.printStackTrace(System.out);
        }
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
     * @param trafficLight 
     */
    public void setTrafficLight(TrafficLight trafficLight)
    {
        this.trafficLight = trafficLight;        
    }
    
    /**
     * 
     * @return 
     */
    public int getSignalGroupsCount()
    {
        return signalGroups.size();
    }
    
    /**
     * 
     * @param signalGroupIndex
     * @param color 
     */
    public void setLightColor(int signalGroupIndex, LightColor color)
    {
        signalGroups.get(signalGroupIndex).setCurrentLightColor(color);
    }
    
    /**
     * 
     */
    public static class SignalGroup
    {
        private char green = 'G';
        private LightColor currentLightColor = LightColor.OFF;

        
        /**
         * 
         */
        public SignalGroup(char green)
        {
            this.green = green;
        }
        
        
        /**
         * 
         * @param currentLightColor 
         */
        public void setCurrentLightColor(LightColor currentLightColor)
        {
            this.currentLightColor = currentLightColor;
        }

        /**
         * 
         * @return 
         */
        public char getSumoCurrentLightColor()
        {
            if(currentLightColor.isGreen())
                return green;
            
            return currentLightColor.getSumoCode();
        }
    }
    
}
