/*
 * Copyright (C) 2016
 * Deutsches Zentrum fuer Luft- und Raumfahrt e.V.
 * Institut fuer Verkehrssystemtechnik
 * 
 * German Aerospace Center
 * Institute of Transportation Systems
 * 
 */
package de.dlr.ts.lisum.interfaces;

import de.dlr.ts.lisum.simulation.Simulation;
import java.io.File;


/**
 *
 * @author @author <a href="mailto:maximiliano.bottazzi@dlr.de">Maximiliano Bottazzi</a>
 */
public interface CityInterface
{
    void load(File configFileOrFolder);
    
    Simulation.InitBeforePlayResponse initBeforePlay();
    
    void executeSimulationStep(long simulationStep);

    ControlUnitInterface[] getControlUnits();
    
    ControlUnitInterface getControlUnit(String name);
    
}
