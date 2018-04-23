/*
 * Copyright (C) 2016
 * Deutsches Zentrum fuer Luft- und Raumfahrt e.V.
 * Institut fuer Verkehrssystemtechnik
 * 
 * German Aerospace Center
 * Institute of Transportation Systems
 * 
 */
package de.dlr.ts.lisum.gui;

import de.dlr.ts.commons.tools.FileTools;
import de.dlr.ts.lisum.GlobalConfig;
import de.dlr.ts.lisum.simulation.Simulation;
import de.dlr.ts.utils.xmladmin2.XMLAdmin2;
import de.dlr.ts.utils.xmladmin2.exceptions.MalformedKeyOrNameException;
import de.dlr.ts.utils.xmladmin2.exceptions.XMLNodeNotFoundException;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.util.logging.Level;
import java.util.logging.Logger;
import javafx.beans.property.BooleanProperty;
import javafx.beans.property.SimpleBooleanProperty;
import javafx.scene.image.Image;
import org.apache.commons.io.IOUtils;
import org.xml.sax.SAXException;


/**
 *
 * @author @author <a href="mailto:maximiliano.bottazzi@dlr.de">Maximiliano Bottazzi</a>
 */
public class SystemProperties
{
    private static final SystemProperties INSTANCE = new SystemProperties();

    private Simulation currentSimulation = null;
    private final BooleanProperty simulationOpened = new SimpleBooleanProperty(false);
    private final BooleanProperty simulationStarted = new SimpleBooleanProperty(false);
    
    private final int width = 1200;
    private final int height = 900;
    
    private final String systemName = "LiSuM";
    private String systemVersion = "1.0";
    private final Image mainIcon = new Image(getClass().getResourceAsStream("/images/icons/LisaSumoIcon.png"));
    
    
    
    
    /**
     * 
     * @return 
     */
    public static SystemProperties getInstance()
    {        
        return INSTANCE;
    }

    /**
     * 
     */
    private SystemProperties()
    {
        loadSystemVersion();
    }
    
    /**
     * 
     * @return 
     */
    public BooleanProperty simulationOpenedProperty()
    {
        return simulationOpened;
    }

    public String getSystemVersion() {
        return systemVersion;
    }
    
    /**
     * 
     * @return 
     */
    public BooleanProperty simulationStartedProperty()
    {
        return simulationStarted;
    }
    
    /**
     * 
     * @return 
     */
    public Image getMainIcon()
    {
        return mainIcon;
    }
    
    /**
     * 
     * @param project
     */
    public void setCurrentSimulation(Simulation project)
    {
        this.currentSimulation = project;
        
        if(project != null)
        {
            this.simulationOpened.set(true);
        }
        else
        {
            this.simulationOpened.set(false);
            this.simulationStarted.set(false);
        }
    }

    /**
     * 
     * @return 
     */
    public String getSystemName()
    {
        return systemName + " " + systemVersion;
    }
    
    /**
     * 
     * @return 
     */
    public Simulation getCurrentSimulation()
    {
        return currentSimulation;
    }

    /**
     * 
     * @return 
     */
    public int getWidth()
    {
        return width;
    }

    /**
     * 
     * @return 
     */
    public int getHeight()
    {
        return height;
    }
    
    private void loadSystemVersion()
    {
        File pom = new File("pom.xml");
        if(pom.exists())
        {                        
            try {
                XMLAdmin2 x = new XMLAdmin2().load(pom);                
                String version = x.getNode("version").getValue();                                
                FileTools.writeSmallTextFile("src/main/resources/version/version", version);                
            }
            catch (SAXException | IOException | MalformedKeyOrNameException | XMLNodeNotFoundException ex) {
                Logger.getLogger(GlobalConfig.class.getName()).log(Level.SEVERE, null, ex);
            }
        }
                                        
        ClassLoader classLoader = getClass().getClassLoader();
        InputStream res = classLoader.getResourceAsStream("version/version");        

        if(res != null)
        {
            try {
                byte[] array = IOUtils.toByteArray(res);
                this.systemVersion = new String(array).trim();
            } catch (IOException ex) {
                Logger.getLogger(GlobalConfig.class.getName()).log(Level.SEVERE, null, ex);
            }
        }        
    }    
}
