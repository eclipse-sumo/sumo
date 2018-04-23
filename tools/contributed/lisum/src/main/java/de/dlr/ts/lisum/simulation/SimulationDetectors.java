/*
 * Copyright (C) 2016
 * Deutsches Zentrum fuer Luft- und Raumfahrt e.V.
 * Institut fuer Verkehrssystemtechnik
 * 
 * German Aerospace Center
 * Institute of Transportation Systems
 * 
 */
package de.dlr.ts.lisum.simulation;

import de.dlr.ts.commons.logger.DLRLogger;
import de.dlr.ts.lisum.interfaces.CityInterface;
import de.dlr.ts.lisum.interfaces.ControlUnitInterface;
import de.dlr.ts.lisum.interfaces.DetectorInterface;
import java.util.HashMap;
import java.util.Map;


/**
 *
 * @author @author <a href="mailto:maximiliano.bottazzi@dlr.de">Maximiliano Bottazzi</a>
 */
public class SimulationDetectors
{
    private final Map<String, DetectorInterface> detectors = new HashMap<>();

    
    /**
     * 
     */
    public SimulationDetectors()
    {
    }

    /**
     * 
     * @param configFile
     * @param city 
     */
    public void load(ConfigurationFile configFile, CityInterface city)
    {
        DLRLogger.info(this, "Loading detectors");
        
        for (ConfigurationFile.CFControlUnit controlUnit : configFile)
        {
            ControlUnitInterface controlUnitInt = city.getControlUnit(controlUnit.lisaName);
            
            for (int i = 0; i < controlUnit.detectors.size(); i++)
            {
                ConfigurationFile.CFControlUnit.Detector det = controlUnit.detectors.get(i);
                DetectorInterface detectorInt = controlUnitInt.getDetector(det.lisaName);
                
                if(detectorInt != null)
                    detectors.put(det.sumoName, detectorInt);
            }
        }
    }
    
    
    /**
     * 
     * @param id
     * @return 
     */
    public DetectorInterface getDetectorPerSumoId(String id)
    {
        return detectors.get(id);
    }
    
}
