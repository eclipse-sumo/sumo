
package de.tudresden.ws;

import javax.xml.bind.annotation.XmlAccessType;
import javax.xml.bind.annotation.XmlAccessorType;
import javax.xml.bind.annotation.XmlType;


/**
 * <p>Java-Klasse für Vehicle_setStop complex type.
 * 
 * <p>Das folgende Schemafragment gibt den erwarteten Content an, der in dieser Klasse enthalten ist.
 * 
 * <pre>
 * &lt;complexType name="Vehicle_setStop">
 *   &lt;complexContent>
 *     &lt;restriction base="{http://www.w3.org/2001/XMLSchema}anyType">
 *       &lt;sequence>
 *         &lt;element name="vehID" type="{http://www.w3.org/2001/XMLSchema}string" minOccurs="0"/>
 *         &lt;element name="edgeID" type="{http://www.w3.org/2001/XMLSchema}string" minOccurs="0"/>
 *         &lt;element name="pos" type="{http://www.w3.org/2001/XMLSchema}double"/>
 *         &lt;element name="laneIndex" type="{http://www.w3.org/2001/XMLSchema}byte"/>
 *         &lt;element name="duration" type="{http://www.w3.org/2001/XMLSchema}int"/>
 *         &lt;element name="stopType" type="{http://ws.tudresden.de/}sumoStopFlags" minOccurs="0"/>
 *       &lt;/sequence>
 *     &lt;/restriction>
 *   &lt;/complexContent>
 * &lt;/complexType>
 * </pre>
 * 
 * 
 */
@XmlAccessorType(XmlAccessType.FIELD)
@XmlType(name = "Vehicle_setStop", propOrder = {
    "vehID",
    "edgeID",
    "pos",
    "laneIndex",
    "duration",
    "stopType"
})
public class VehicleSetStop {

    protected String vehID;
    protected String edgeID;
    protected double pos;
    protected byte laneIndex;
    protected int duration;
    protected SumoStopFlags stopType;

    /**
     * Ruft den Wert der vehID-Eigenschaft ab.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    public String getVehID() {
        return vehID;
    }

    /**
     * Legt den Wert der vehID-Eigenschaft fest.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setVehID(String value) {
        this.vehID = value;
    }

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
     * Ruft den Wert der duration-Eigenschaft ab.
     * 
     */
    public int getDuration() {
        return duration;
    }

    /**
     * Legt den Wert der duration-Eigenschaft fest.
     * 
     */
    public void setDuration(int value) {
        this.duration = value;
    }

    /**
     * Ruft den Wert der stopType-Eigenschaft ab.
     * 
     * @return
     *     possible object is
     *     {@link SumoStopFlags }
     *     
     */
    public SumoStopFlags getStopType() {
        return stopType;
    }

    /**
     * Legt den Wert der stopType-Eigenschaft fest.
     * 
     * @param value
     *     allowed object is
     *     {@link SumoStopFlags }
     *     
     */
    public void setStopType(SumoStopFlags value) {
        this.stopType = value;
    }

}
