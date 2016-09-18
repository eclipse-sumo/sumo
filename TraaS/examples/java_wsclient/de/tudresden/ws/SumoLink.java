
package de.tudresden.ws;

import javax.xml.bind.annotation.XmlAccessType;
import javax.xml.bind.annotation.XmlAccessorType;
import javax.xml.bind.annotation.XmlType;


/**
 * <p>Java-Klasse für sumoLink complex type.
 * 
 * <p>Das folgende Schemafragment gibt den erwarteten Content an, der in dieser Klasse enthalten ist.
 * 
 * <pre>
 * &lt;complexType name="sumoLink">
 *   &lt;complexContent>
 *     &lt;restriction base="{http://www.w3.org/2001/XMLSchema}anyType">
 *       &lt;sequence>
 *         &lt;element name="from" type="{http://www.w3.org/2001/XMLSchema}string" minOccurs="0"/>
 *         &lt;element name="to" type="{http://www.w3.org/2001/XMLSchema}string" minOccurs="0"/>
 *         &lt;element name="over" type="{http://www.w3.org/2001/XMLSchema}string" minOccurs="0"/>
 *         &lt;element name="notInternalLane" type="{http://www.w3.org/2001/XMLSchema}string" minOccurs="0"/>
 *         &lt;element name="internalLane" type="{http://www.w3.org/2001/XMLSchema}string" minOccurs="0"/>
 *         &lt;element name="state" type="{http://www.w3.org/2001/XMLSchema}string" minOccurs="0"/>
 *         &lt;element name="direction" type="{http://www.w3.org/2001/XMLSchema}string" minOccurs="0"/>
 *         &lt;element name="hasPriority" type="{http://www.w3.org/2001/XMLSchema}byte"/>
 *         &lt;element name="isOpen" type="{http://www.w3.org/2001/XMLSchema}byte"/>
 *         &lt;element name="hasApproachingFoe" type="{http://www.w3.org/2001/XMLSchema}byte"/>
 *         &lt;element name="length" type="{http://www.w3.org/2001/XMLSchema}double"/>
 *       &lt;/sequence>
 *     &lt;/restriction>
 *   &lt;/complexContent>
 * &lt;/complexType>
 * </pre>
 * 
 * 
 */
@XmlAccessorType(XmlAccessType.FIELD)
@XmlType(name = "sumoLink", propOrder = {
    "from",
    "to",
    "over",
    "notInternalLane",
    "internalLane",
    "state",
    "direction",
    "hasPriority",
    "isOpen",
    "hasApproachingFoe",
    "length"
})
public class SumoLink {

    protected String from;
    protected String to;
    protected String over;
    protected String notInternalLane;
    protected String internalLane;
    protected String state;
    protected String direction;
    protected byte hasPriority;
    protected byte isOpen;
    protected byte hasApproachingFoe;
    protected double length;

    /**
     * Ruft den Wert der from-Eigenschaft ab.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    public String getFrom() {
        return from;
    }

    /**
     * Legt den Wert der from-Eigenschaft fest.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setFrom(String value) {
        this.from = value;
    }

    /**
     * Ruft den Wert der to-Eigenschaft ab.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    public String getTo() {
        return to;
    }

    /**
     * Legt den Wert der to-Eigenschaft fest.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setTo(String value) {
        this.to = value;
    }

    /**
     * Ruft den Wert der over-Eigenschaft ab.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    public String getOver() {
        return over;
    }

    /**
     * Legt den Wert der over-Eigenschaft fest.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setOver(String value) {
        this.over = value;
    }

    /**
     * Ruft den Wert der notInternalLane-Eigenschaft ab.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    public String getNotInternalLane() {
        return notInternalLane;
    }

    /**
     * Legt den Wert der notInternalLane-Eigenschaft fest.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setNotInternalLane(String value) {
        this.notInternalLane = value;
    }

    /**
     * Ruft den Wert der internalLane-Eigenschaft ab.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    public String getInternalLane() {
        return internalLane;
    }

    /**
     * Legt den Wert der internalLane-Eigenschaft fest.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setInternalLane(String value) {
        this.internalLane = value;
    }

    /**
     * Ruft den Wert der state-Eigenschaft ab.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    public String getState() {
        return state;
    }

    /**
     * Legt den Wert der state-Eigenschaft fest.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setState(String value) {
        this.state = value;
    }

    /**
     * Ruft den Wert der direction-Eigenschaft ab.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    public String getDirection() {
        return direction;
    }

    /**
     * Legt den Wert der direction-Eigenschaft fest.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setDirection(String value) {
        this.direction = value;
    }

    /**
     * Ruft den Wert der hasPriority-Eigenschaft ab.
     * 
     */
    public byte getHasPriority() {
        return hasPriority;
    }

    /**
     * Legt den Wert der hasPriority-Eigenschaft fest.
     * 
     */
    public void setHasPriority(byte value) {
        this.hasPriority = value;
    }

    /**
     * Ruft den Wert der isOpen-Eigenschaft ab.
     * 
     */
    public byte getIsOpen() {
        return isOpen;
    }

    /**
     * Legt den Wert der isOpen-Eigenschaft fest.
     * 
     */
    public void setIsOpen(byte value) {
        this.isOpen = value;
    }

    /**
     * Ruft den Wert der hasApproachingFoe-Eigenschaft ab.
     * 
     */
    public byte getHasApproachingFoe() {
        return hasApproachingFoe;
    }

    /**
     * Legt den Wert der hasApproachingFoe-Eigenschaft fest.
     * 
     */
    public void setHasApproachingFoe(byte value) {
        this.hasApproachingFoe = value;
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

}
