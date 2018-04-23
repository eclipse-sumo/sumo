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
import de.dlr.ts.lisum.exceptions.LisumException;
import java.io.File;

/**
 *
 * @author @author <a href="mailto:maximiliano.bottazzi@dlr.de">Maximiliano Bottazzi</a>
 */
public class SimulationFiles
{
    private File sumoNetFile;
    private File sumoAddFile;
    private File sumoRouFile;
    
    private File simulationDirectory;
    private File lisumConfigFile;
    private File sumoConfigFile;
    
    private File lisaDataDirectory;
    
    
    /**
     * 
     */
    public SimulationFiles()
    {
    }

    /**
     * 
     * @param lisaDataDirectory
     */
    public void setLisaDataDirectory(File lisaDataDirectory)
    {
        if(!lisaDataDirectory.isAbsolute())
            lisaDataDirectory = new File(simulationDirectory.getAbsolutePath() 
                    + File.separator + lisaDataDirectory.getName());
        
        this.lisaDataDirectory = lisaDataDirectory;
    }
    
    /**
     * 
     * @param lisumConfigurationFile
     * @throws de.dlr.ts.lisum.exceptions.LisumException  
     */
    public void read(File lisumConfigurationFile) throws LisumException
    {
        DLRLogger.config(this, "Reading and setting simulation properties from " + lisumConfigurationFile);
        
        /**
         * Setting Lisum configuration file
         */
        lisumConfigFile = lisumConfigurationFile;
        if(!lisumConfigFile.isAbsolute())
            lisumConfigFile = new File(lisumConfigFile.getAbsolutePath());
        
        if(!lisumConfigFile.exists())
            throw new LisumException("Fatal error: Configuration file '" + 
                    lisumConfigFile.getAbsolutePath() + "' does not exist");

        /**
         * Setting simulation directory
         */
        simulationDirectory = new File(lisumConfigFile.getParent());
        DLRLogger.config(this, "Setting simulation directory: " + simulationDirectory);
        
        
        
        /**
         * Setting Sumo configuration file
         */
        String tmp = null;
        for (String s : simulationDirectory.list())
            if(s.endsWith(".sumocfg"))
                tmp = s;
        
        if(tmp == null)
            throw new LisumException("Fatal error: no Sumo configuration file could be found");
        
        sumoConfigFile = new File(simulationDirectory.getAbsolutePath() + File.separator + tmp);
        
        DLRLogger.config(this, "Setting Sumo configuration file: " + sumoConfigFile);

        
        
        /**
         * Setting sumo net file
         */
        tmp = null;
        for (String s : simulationDirectory.list())
            if(s.endsWith("net.xml"))
                tmp = s;
        
        if(tmp == null)
            throw new LisumException("Fatal error: no Sumo net.xml file could be found");
        
        sumoNetFile = new File(simulationDirectory.getAbsolutePath() + File.separator + tmp);
        DLRLogger.config(this, "Setting Sumo net.xml file: " + sumoNetFile);
        
        
        /**
         * Setting sumo add file
         */
        tmp = null;
        for (String s : simulationDirectory.list())
            if(s.endsWith("add.xml"))
                tmp = s;
        
        if(tmp != null)
        {
            sumoAddFile = new File(simulationDirectory.getAbsolutePath() + File.separator + tmp);
            DLRLogger.config(this, "Setting Sumo add.xml file: " + sumoAddFile);
        }
        
        /**
         * Setting rou file
         */
        tmp = null;
        for (String s : simulationDirectory.list())
            if(s.endsWith("rou.xml"))
                tmp = s;
        
        if(tmp != null)
        {
            sumoRouFile = new File(simulationDirectory.getAbsolutePath() + File.separator + tmp);
            DLRLogger.config(this, "Setting Sumo rou.xml file: " + sumoRouFile);
        }
    }
    
    /**
     * 
     * @return 
     */
    public File getLisumConfigFile()
    {
        return lisumConfigFile;
    }
    
    /**
     * 
     * @return 
     */
    public File getSumoConfigFile()
    {
        return sumoConfigFile;
    }

    /**
     * 
     * @return 
     */
    public File getSimulationDirectory()
    {
        return simulationDirectory;
    }

    /**
     * 
     * @return 
     */
    public File getLisaDataDirectory()
    {
        return lisaDataDirectory;
    }
    
    /**
     * 
     * @return 
     */
    public File getSumoNetFile()
    {
        return sumoNetFile;
    }

    /**
     * 
     * @return 
     */
    public File getSumoAddFile()
    {
        return sumoAddFile;
    }

    /**
     * 
     * @return 
     */
    public File getSumoRouFile()
    {
        return sumoRouFile;
    }
    
    /**
     * 
     * @param simulationDirectory 
     */
    void setSimulationDirectory(File simulationDirectory)
    {
        this.simulationDirectory = simulationDirectory;
    }

}
