
package de.tudresden.ws;

import javax.xml.bind.annotation.XmlAccessType;
import javax.xml.bind.annotation.XmlAccessorType;
import javax.xml.bind.annotation.XmlType;


/**
 * <p>Java-Klasse für Simulation_getDistanceRoad complex type.
 * 
 * <p>Das folgende Schemafragment gibt den erwarteten Content an, der in dieser Klasse enthalten ist.
 * 
 * <pre>
 * &lt;complexType name="Simulation_getDistanceRoad">
 *   &lt;complexContent>
 *     &lt;restriction base="{http://www.w3.org/2001/XMLSchema}anyType">
 *       &lt;sequence>
 *         &lt;element name="edgeID1" type="{http://www.w3.org/2001/XMLSchema}string" minOccurs="0"/>
 *         &lt;element name="pos1" type="{http://www.w3.org/2001/XMLSchema}double"/>
 *         &lt;element name="edgeID2" type="{http://www.w3.org/2001/XMLSchema}string" minOccurs="0"/>
 *         &lt;element name="pos2" type="{http://www.w3.org/2001/XMLSchema}double"/>
 *         &lt;element name="isDriving" type="{http://www.w3.org/2001/XMLSchema}boolean"/>
 *       &lt;/sequence>
 *     &lt;/restriction>
 *   &lt;/complexContent>
 * &lt;/complexType>
 * </pre>
 * 
 * 
 */
@XmlAccessorType(XmlAccessType.FIELD)
@XmlType(name = "Simulation_getDistanceRoad", propOrder = {
    "edgeID1",
    "pos1",
    "edgeID2",
    "pos2",
    "isDriving"
})
public class SimulationGetDistanceRoad {

    protected String edgeID1;
    protected double pos1;
    protected String edgeID2;
    protected double pos2;
    protected boolean isDriving;

    /**
     * Ruft den Wert der edgeID1-Eigenschaft ab.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    public String getEdgeID1() {
        return edgeID1;
    }

    /**
     * Legt den Wert der edgeID1-Eigenschaft fest.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setEdgeID1(String value) {
        this.edgeID1 = value;
    }

    /**
     * Ruft den Wert der pos1-Eigenschaft ab.
     * 
     */
    public double getPos1() {
        return pos1;
    }

    /**
     * Legt den Wert der pos1-Eigenschaft fest.
     * 
     */
    public void setPos1(double value) {
        this.pos1 = value;
    }

    /**
     * Ruft den Wert der edgeID2-Eigenschaft ab.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    public String getEdgeID2() {
        return edgeID2;
    }

    /**
     * Legt den Wert der edgeID2-Eigenschaft fest.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setEdgeID2(String value) {
        this.edgeID2 = value;
    }

    /**
     * Ruft den Wert der pos2-Eigenschaft ab.
     * 
     */
    public double getPos2() {
        return pos2;
    }

    /**
     * Legt den Wert der pos2-Eigenschaft fest.
     * 
     */
    public void setPos2(double value) {
        this.pos2 = value;
    }

    /**
     * Ruft den Wert der isDriving-Eigenschaft ab.
     * 
     */
    public boolean isIsDriving() {
        return isDriving;
    }

    /**
     * Legt den Wert der isDriving-Eigenschaft fest.
     * 
     */
    public void setIsDriving(boolean value) {
        this.isDriving = value;
    }

}
