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

import de.dlr.ts.commons.logger.DLRLogger;
import de.dlr.ts.lisum.GlobalConfig;
import de.dlr.ts.lisum.interfaces.DetectorInterface;
import de.dlr.ts.lisum.simulation.Simulation;
import de.dlr.ts.lisum.simulation.VehicleTypes;
import it.polito.appeal.traci.InductionLoop;
import it.polito.appeal.traci.SumoTraciConnection;
import it.polito.appeal.traci.TrafficLight;
import it.polito.appeal.traci.Vehicle;
import java.io.IOException;
import java.net.Inet4Address;
import java.net.UnknownHostException;
import java.util.Collection;



/**
 *  This class administrates the connection with SUMO and the simulation execution on the SUMO side.
 * 
 * 
 * @author @author <a href="mailto:maximiliano.bottazzi@dlr.de">Maximiliano Bottazzi</a>
 */
public class Sumo
{
    private Simulation simulation;
    private SumoControlUnits sumoControlUnits;
    private SumoDetectors sumoDetectors;
    private VehicleTypes vehicleTypes;
    private long vehiclesCount;
    private long stepsPerSecond = 0L;    
    
    
    /**
     *
     * @param project
     */
    public Sumo(Simulation project)
    {
        this.simulation = project;        
    }

    /**
     * 
     */
    public void initBeforePlay()
    {
        sumoControlUnits = new SumoControlUnits();
        sumoControlUnits.load(simulation.getSimulationFiles().getSumoNetFile());
        
        vehicleTypes = new VehicleTypes();
        vehicleTypes.load(simulation.getSimulationFiles().getSumoRouFile());
        
        sumoDetectors = new SumoDetectors(/*vehicleTypes*/);
        //sumoDetectors.load(simulation.getSimulationFiles().getSumoAddFile());
    }
    
    /**
     * Starts SumoGUI, which takes controls of the whole application.
     */
    public void play()
    {
        Thread thread = new Thread(runnable);
        thread.start();
    }

    /**
     * 
     */
    private final Runnable runnable = new Runnable()
    {
        @Override
        public void run()
        {
            SumoTraciConnection conn;

            try
            {
                /**
                 *
                 */
                DLRLogger.info("Sumo", "Starting SumoGUI instance, port " + 
                        GlobalConfig.getInstance().getSumoPort());
                startSumoGUI();

                conn = new SumoTraciConnection(Inet4Address.getLocalHost(), 
                        GlobalConfig.getInstance().getSumoPort());
                
                long simulationTimeSeconds;
                
                /**
                 * Control units
                 */
                Collection<TrafficLight> tls_ = conn.getTrafficLightRepository().getAll().values();
                for (TrafficLight tl : tls_)
                {
                    SumoControlUnit sumoControlUnit = sumoControlUnits.getControlUnit(tl.getID());
                    if(sumoControlUnit != null)
                    {
                        sumoControlUnit.setTrafficLight(tl);                             
                        sumoControlUnit.setControlUnits(simulation.getControlUnits());
                    }
                }


                /**
                 * Induction loops
                 */                
                DLRLogger.finest(this, "Reading detectors from sumo");
                Collection<InductionLoop> inducts = conn.getInductionLoopRepository().getAll().values();
                DLRLogger.finest(this, inducts.size() + " detectors from sumo");
                
                for (InductionLoop induct : inducts)
                {
                    DetectorInterface cityDetector = simulation.getDetectors().getDetectorPerSumoId(induct.getID());                    
                    sumoDetectors.addDetector(induct.getID(), induct, cityDetector);
                }
                
                long time = System.currentTimeMillis();
                long _steps = 0L;
                
                /**
                 * 
                 */
                boolean run = true;
                while (run)
                {
                    conn.nextSimStep();
                    
                    simulationTimeSeconds = conn.getCurrentSimTime() / 1000;
                    
                    if(System.currentTimeMillis() - time > 1000L)
                    {
                        stepsPerSecond = _steps;
                        _steps = 0L;
                        time = System.currentTimeMillis();
                    }
                    else
                        _steps++;
                    
                    Collection<Vehicle> vehicles = conn.getVehicleRepository().getAll().values();
                    vehiclesCount = vehicles.size();
                    
                    /**
                     * 
                     */
                    sumoDetectors.executeSimulationStep();
                    sumoControlUnits.executeSimulationStep();
                    simulation.executeSimulationStep(simulationTimeSeconds);
                }
                
                conn.close();
            }
            catch (UnknownHostException ex)
            {
                //DLRLogger.severe(this, ex);
                DLRLogger.info("Sumo", "Closing SumoGUI instance");                
            }
            catch (IOException | InterruptedException ex)
            {
                //DLRLogger.severe(this, ex);
                DLRLogger.info("Sumo", "Closing SumoGUI instance");
            }
            
            stepsPerSecond = 0;
            vehiclesCount = 0;
        }
    };

    /**
     * 
     * @return 
     */
    public Runnable getRunnable()
    {
        return runnable;
    }
    
    /**
     *
     */
    private void startSumoGUI()
    {
        try
        {
            String sumoConfig = simulation.getSimulationFiles().getSumoConfigFile().getAbsolutePath();

            ProcessBuilder pb = new ProcessBuilder(GlobalConfig.getInstance().getSumoExec(),
                    "--remote-port",
                    String.valueOf(GlobalConfig.getInstance().getSumoPort()),
                    "-c",
                    sumoConfig);

            pb.start();
        }
        catch (IOException ex)
        {
            DLRLogger.severe(this, ex);
        }
    }

    /**
     * 
     * @return 
     */
    public long getVehiclesCount()
    {
        return vehiclesCount;
    }

    /**
     * 
     * @return 
     */
    public long getStepsPerSecond()
    {
        return stepsPerSecond;
    }
    
}
