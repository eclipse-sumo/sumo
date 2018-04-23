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

/**
 *
 * @author @author <a href="mailto:maximiliano.bottazzi@dlr.de">Maximiliano Bottazzi</a>
 */
public interface SignalProgramInterface
{
    void setName(String name);
    String getName();
    
    void setObjNr(int objNr);
    int getObjNr();
    
    boolean isVA();
    boolean isFZ();
}
