/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2017-2019 German Aerospace Center (DLR) and others.
// TraaS module
// Copyright (C) 2013-2017 Dresden University of Technology
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    SimulationConvert3D.java
/// @author  Mario Krumnow
/// @date    2013
/// @version $Id$
///
//
/****************************************************************************/

package de.tudresden.ws;

import javax.xml.bind.annotation.XmlAccessType;
import javax.xml.bind.annotation.XmlAccessorType;
import javax.xml.bind.annotation.XmlType;


/**
 * <p>Java class for Simulation_convert3D complex type.
 *
 * <p>Das folgende Schemafragment gibt den erwarteten Content an, der in dieser Klasse enthalten ist.
 *
 * <pre>
 * &lt;complexType name="Simulation_convert3D">
 *   &lt;complexContent>
 *     &lt;restriction base="{http://www.w3.org/2001/XMLSchema}anyType">
 *       &lt;sequence>
 *         &lt;element name="edgeID" type="{http://www.w3.org/2001/XMLSchema}string" minOccurs="0"/>
 *         &lt;element name="pos" type="{http://www.w3.org/2001/XMLSchema}double"/>
 *         &lt;element name="laneIndex" type="{http://www.w3.org/2001/XMLSchema}byte"/>
 *         &lt;element name="toGeo" type="{http://www.w3.org/2001/XMLSchema}string" minOccurs="0"/>
 *       &lt;/sequence>
 *     &lt;/restriction>
 *   &lt;/complexContent>
 * &lt;/complexType>
 * </pre>
 *
 *
 */
@XmlAccessorType(XmlAccessType.FIELD)
@XmlType(name = "Simulation_convert3D", propOrder = {
    "edgeID",
    "pos",
    "laneIndex",
    "toGeo"
})
public class SimulationConvert3D {

    protected String edgeID;
    protected double pos;
    protected byte laneIndex;
    protected String toGeo;

    /**
     * Ruft den Wert der edgeID-Eigenschaft ab.
     *
     * @return
     *     possible object is
     *     {@link String }
     *
     */
    public String getEdgeID() {
        return edgeID;
    }

    /**
     * Legt den Wert der edgeID-Eigenschaft fest.
     *
     * @param value
     *     allowed object is
     *     {@link String }
     *
     */
    public void setEdgeID(String value) {
        this.edgeID = value;
    }

    /**
     * Ruft den Wert der pos-Eigenschaft ab.
     *
     */
    public double getPos() {
        return pos;
    }

    /**
     * Legt den Wert der pos-Eigenschaft fest.
     *
     */
    public void setPos(double value) {
        this.pos = value;
    }

    /**
     * Ruft den Wert der laneIndex-Eigenschaft ab.
     *
     */
    public byte getLaneIndex() {
        return laneIndex;
    }

    /**
     * Legt den Wert der laneIndex-Eigenschaft fest.
     *
     */
    public void setLaneIndex(byte value) {
        this.laneIndex = value;
    }

    /**
     * Ruft den Wert der toGeo-Eigenschaft ab.
     *
     * @return
     *     possible object is
     *     {@link String }
     *
     */
    public String getToGeo() {
        return toGeo;
    }

    /**
     * Legt den Wert der toGeo-Eigenschaft fest.
     *
     * @param value
     *     allowed object is
     *     {@link String }
     *
     */
    public void setToGeo(String value) {
        this.toGeo = value;
    }

}
