
package de.tudresden.ws;

import javax.xml.bind.annotation.XmlAccessType;
import javax.xml.bind.annotation.XmlAccessorType;
import javax.xml.bind.annotation.XmlType;


/**
 * <p>Java-Klasse für Vehicle_setAdaptedTraveltime complex type.
 * 
 * <p>Das folgende Schemafragment gibt den erwarteten Content an, der in dieser Klasse enthalten ist.
 * 
 * <pre>
 * &lt;complexType name="Vehicle_setAdaptedTraveltime">
 *   &lt;complexContent>
 *     &lt;restriction base="{http://www.w3.org/2001/XMLSchema}anyType">
 *       &lt;sequence>
 *         &lt;element name="vehID" type="{http://www.w3.org/2001/XMLSchema}string" minOccurs="0"/>
 *         &lt;element name="begTime" type="{http://www.w3.org/2001/XMLSchema}int"/>
 *         &lt;element name="endTime" type="{http://www.w3.org/2001/XMLSchema}int"/>
 *         &lt;element name="edgeID" type="{http://www.w3.org/2001/XMLSchema}string" minOccurs="0"/>
 *         &lt;element name="time" type="{http://www.w3.org/2001/XMLSchema}double"/>
 *       &lt;/sequence>
 *     &lt;/restriction>
 *   &lt;/complexContent>
 * &lt;/complexType>
 * </pre>
 * 
 * 
 */
@XmlAccessorType(XmlAccessType.FIELD)
@XmlType(name = "Vehicle_setAdaptedTraveltime", propOrder = {
    "vehID",
    "begTime",
    "endTime",
    "edgeID",
    "time"
})
public class VehicleSetAdaptedTraveltime {

    protected String vehID;
    protected int begTime;
    protected int endTime;
    protected String edgeID;
    protected double time;

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
     * Ruft den Wert der begTime-Eigenschaft ab.
     * 
     */
    public int getBegTime() {
        return begTime;
    }

    /**
     * Legt den Wert der begTime-Eigenschaft fest.
     * 
     */
    public void setBegTime(int value) {
        this.begTime = value;
    }

    /**
     * Ruft den Wert der endTime-Eigenschaft ab.
     * 
     */
    public int getEndTime() {
        return endTime;
    }

    /**
     * Legt den Wert der endTime-Eigenschaft fest.
     * 
     */
    public void setEndTime(int value) {
        this.endTime = value;
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
     * Ruft den Wert der time-Eigenschaft ab.
     * 
     */
    public double getTime() {
        return time;
    }

    /**
     * Legt den Wert der time-Eigenschaft fest.
     * 
     */
    public void setTime(double value) {
        this.time = value;
    }

}
