
package de.tudresden.ws;

import javax.xml.bind.annotation.XmlAccessType;
import javax.xml.bind.annotation.XmlAccessorType;
import javax.xml.bind.annotation.XmlType;


/**
 * <p>Java-Klasse für Vehicle_add complex type.
 * 
 * <p>Das folgende Schemafragment gibt den erwarteten Content an, der in dieser Klasse enthalten ist.
 * 
 * <pre>
 * &lt;complexType name="Vehicle_add">
 *   &lt;complexContent>
 *     &lt;restriction base="{http://www.w3.org/2001/XMLSchema}anyType">
 *       &lt;sequence>
 *         &lt;element name="vehID" type="{http://www.w3.org/2001/XMLSchema}string" minOccurs="0"/>
 *         &lt;element name="typeID" type="{http://www.w3.org/2001/XMLSchema}string" minOccurs="0"/>
 *         &lt;element name="routeID" type="{http://www.w3.org/2001/XMLSchema}string" minOccurs="0"/>
 *         &lt;element name="depart" type="{http://www.w3.org/2001/XMLSchema}int"/>
 *         &lt;element name="pos" type="{http://www.w3.org/2001/XMLSchema}double"/>
 *         &lt;element name="speed" type="{http://www.w3.org/2001/XMLSchema}double"/>
 *         &lt;element name="lane" type="{http://www.w3.org/2001/XMLSchema}byte"/>
 *       &lt;/sequence>
 *     &lt;/restriction>
 *   &lt;/complexContent>
 * &lt;/complexType>
 * </pre>
 * 
 * 
 */
@XmlAccessorType(XmlAccessType.FIELD)
@XmlType(name = "Vehicle_add", propOrder = {
    "vehID",
    "typeID",
    "routeID",
    "depart",
    "pos",
    "speed",
    "lane"
})
public class VehicleAdd {

    protected String vehID;
    protected String typeID;
    protected String routeID;
    protected int depart;
    protected double pos;
    protected double speed;
    protected byte lane;

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

    /**
     * Ruft den Wert der routeID-Eigenschaft ab.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    public String getRouteID() {
        return routeID;
    }

    /**
     * Legt den Wert der routeID-Eigenschaft fest.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setRouteID(String value) {
        this.routeID = value;
    }

    /**
     * Ruft den Wert der depart-Eigenschaft ab.
     * 
     */
    public int getDepart() {
        return depart;
    }

    /**
     * Legt den Wert der depart-Eigenschaft fest.
     * 
     */
    public void setDepart(int value) {
        this.depart = value;
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
     * Ruft den Wert der speed-Eigenschaft ab.
     * 
     */
    public double getSpeed() {
        return speed;
    }

    /**
     * Legt den Wert der speed-Eigenschaft fest.
     * 
     */
    public void setSpeed(double value) {
        this.speed = value;
    }

    /**
     * Ruft den Wert der lane-Eigenschaft ab.
     * 
     */
    public byte getLane() {
        return lane;
    }

    /**
     * Legt den Wert der lane-Eigenschaft fest.
     * 
     */
    public void setLane(byte value) {
        this.lane = value;
    }

}
