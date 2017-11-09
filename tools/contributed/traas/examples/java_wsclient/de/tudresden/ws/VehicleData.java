
package de.tudresden.ws;

import javax.xml.bind.annotation.XmlAccessType;
import javax.xml.bind.annotation.XmlAccessorType;
import javax.xml.bind.annotation.XmlElement;
import javax.xml.bind.annotation.XmlType;


/**
 * <p>Java-Klasse für vehicleData complex type.
 * 
 * <p>Das folgende Schemafragment gibt den erwarteten Content an, der in dieser Klasse enthalten ist.
 * 
 * <pre>
 * &lt;complexType name="vehicleData">
 *   &lt;complexContent>
 *     &lt;restriction base="{http://www.w3.org/2001/XMLSchema}anyType">
 *       &lt;sequence>
 *         &lt;element name="vehID" type="{http://www.w3.org/2001/XMLSchema}string" minOccurs="0"/>
 *         &lt;element name="length" type="{http://www.w3.org/2001/XMLSchema}double"/>
 *         &lt;element name="entry_time" type="{http://www.w3.org/2001/XMLSchema}double"/>
 *         &lt;element name="leave_time" type="{http://www.w3.org/2001/XMLSchema}double"/>
 *         &lt;element name="typeID" type="{http://www.w3.org/2001/XMLSchema}string" minOccurs="0"/>
 *       &lt;/sequence>
 *     &lt;/restriction>
 *   &lt;/complexContent>
 * &lt;/complexType>
 * </pre>
 * 
 * 
 */
@XmlAccessorType(XmlAccessType.FIELD)
@XmlType(name = "vehicleData", propOrder = {
    "vehID",
    "length",
    "entryTime",
    "leaveTime",
    "typeID"
})
public class VehicleData {

    protected String vehID;
    protected double length;
    @XmlElement(name = "entry_time")
    protected double entryTime;
    @XmlElement(name = "leave_time")
    protected double leaveTime;
    protected String typeID;

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
     * Ruft den Wert der length-Eigenschaft ab.
     * 
     */
    public double getLength() {
        return length;
    }

    /**
     * Legt den Wert der length-Eigenschaft fest.
     * 
     */
    public void setLength(double value) {
        this.length = value;
    }

    /**
     * Ruft den Wert der entryTime-Eigenschaft ab.
     * 
     */
    public double getEntryTime() {
        return entryTime;
    }

    /**
     * Legt den Wert der entryTime-Eigenschaft fest.
     * 
     */
    public void setEntryTime(double value) {
        this.entryTime = value;
    }

    /**
     * Ruft den Wert der leaveTime-Eigenschaft ab.
     * 
     */
    public double getLeaveTime() {
        return leaveTime;
    }

    /**
     * Legt den Wert der leaveTime-Eigenschaft fest.
     * 
     */
    public void setLeaveTime(double value) {
        this.leaveTime = value;
    }

    /**
     * Ruft den Wert der typeID-Eigenschaft ab.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    public String getTypeID() {
        return typeID;
    }

    /**
     * Legt den Wert der typeID-Eigenschaft fest.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setTypeID(String value) {
        this.typeID = value;
    }

}
