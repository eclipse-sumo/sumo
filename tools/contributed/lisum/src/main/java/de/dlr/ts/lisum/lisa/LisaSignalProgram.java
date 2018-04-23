/*
 * Copyright (C) 2016
 * Deutsches Zentrum fuer Luft- und Raumfahrt e.V.
 * Institut fuer Verkehrssystemtechnik
 * 
 * German Aerospace Center
 * Institute of Transportation Systems
 * 
 */
package de.dlr.ts.lisum.lisa;

import de.dlr.ts.lisum.interfaces.SignalProgramInterface;
import java.util.Objects;

/**
 *
 */
class LisaSignalProgram implements SignalProgramInterface
{
    private String name;
    private int objNr;
    private final boolean fz = false;
    private final boolean va = false;

    
    /**
     * 
     * @param name
     * @param objNr 
     */
    public LisaSignalProgram(String name, int objNr)
    {
        this.name = name;
        this.objNr = objNr;
    }

    @Override
    public String getName()
    {
        return name;
    }
    
    @Override
    public boolean equals(Object obj)
    {
        if (obj == null)
        {
            return false;
        }
        if (getClass() != obj.getClass())
        {
            return false;
        }
        final LisaSignalProgram other = (LisaSignalProgram) obj;
        return (this.name == null ? other.name == null : this.name.equals(other.name));
    }

    @Override
    public int hashCode()
    {
        int hash = 7;
        hash = 59 * hash + Objects.hashCode(this.name);
        return hash;
    }

    @Override
    public void setName(String name)
    {
        this.name = name;
    }

    @Override
    public void setObjNr(int objNr)
    {
        this.objNr = objNr;
    }

    @Override
    public int getObjNr()
    {
        return objNr;
    }

    @Override
    public boolean isVA()
    {
        return va;
    }

    @Override
    public boolean isFZ()
    {
        return fz;
    }
    
}
