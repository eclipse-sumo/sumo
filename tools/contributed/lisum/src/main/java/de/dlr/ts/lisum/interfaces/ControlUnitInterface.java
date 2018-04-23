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

import de.dlr.ts.lisum.enums.LightColor;

/**
 *
 * @author @author <a href="mailto:maximiliano.bottazzi@dlr.de">Maximiliano Bottazzi</a>
 */
public interface ControlUnitInterface
{
    String getFullName();

    int signalGroupsCount();

    LightColor getLightColor(int signalGroupIndex);
    LightColor getLightColor(String signalGroupName);
    
    SignalProgramInterface[] getSignalPrograms();    
    void setCurrentSignalProgram(int index);
    SignalProgramInterface getCurrentSignalProgram();
        
    DetectorInterface getDetector(String name);
    
    boolean isVA();
    void setVA(boolean va);
    
    boolean isIV();
    void setIV(boolean iv);
    
    boolean isOV();
    void setOV(boolean ov);
    
    int getEbene();
    void setEbene(int ebene);
    
    int getKnotenEinAus();
    void setKnotenEinAus(int einAus);
    
    void storePersistent();
    
    void setEnabled(boolean enabled);
    boolean isEnabled();
    
    void setCoordinated(int coordinated);
    int getCoordinated();
    
}
