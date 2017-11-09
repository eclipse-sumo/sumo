
package de.tudresden.ws;

import javax.xml.bind.annotation.XmlAccessType;
import javax.xml.bind.annotation.XmlAccessorType;
import javax.xml.bind.annotation.XmlType;


/**
 * <p>Java-Klasse für sumoStopFlags complex type.
 * 
 * <p>Das folgende Schemafragment gibt den erwarteten Content an, der in dieser Klasse enthalten ist.
 * 
 * <pre>
 * &lt;complexType name="sumoStopFlags">
 *   &lt;complexContent>
 *     &lt;restriction base="{http://www.w3.org/2001/XMLSchema}anyType">
 *       &lt;sequence>
 *         &lt;element name="stopped" type="{http://www.w3.org/2001/XMLSchema}boolean"/>
 *         &lt;element name="parking" type="{http://www.w3.org/2001/XMLSchema}boolean"/>
 *         &lt;element name="triggered" type="{http://www.w3.org/2001/XMLSchema}boolean"/>
 *         &lt;element name="containerTriggered" type="{http://www.w3.org/2001/XMLSchema}boolean"/>
 *         &lt;element name="isBusStop" type="{http://www.w3.org/2001/XMLSchema}boolean"/>
 *         &lt;element name="isContainerStop" type="{http://www.w3.org/2001/XMLSchema}boolean"/>
 *       &lt;/sequence>
 *     &lt;/restriction>
 *   &lt;/complexContent>
 * &lt;/complexType>
 * </pre>
 * 
 * 
 */
@XmlAccessorType(XmlAccessType.FIELD)
@XmlType(name = "sumoStopFlags", propOrder = {
    "stopped",
    "parking",
    "triggered",
    "containerTriggered",
    "isBusStop",
    "isContainerStop"
})
public class SumoStopFlags {

    protected boolean stopped;
    protected boolean parking;
    protected boolean triggered;
    protected boolean containerTriggered;
    protected boolean isBusStop;
    protected boolean isContainerStop;

    /**
     * Ruft den Wert der stopped-Eigenschaft ab.
     * 
     */
    public boolean isStopped() {
        return stopped;
    }

    /**
     * Legt den Wert der stopped-Eigenschaft fest.
     * 
     */
    public void setStopped(boolean value) {
        this.stopped = value;
    }

    /**
     * Ruft den Wert der parking-Eigenschaft ab.
     * 
     */
    public boolean isParking() {
        return parking;
    }

    /**
     * Legt den Wert der parking-Eigenschaft fest.
     * 
     */
    public void setParking(boolean value) {
        this.parking = value;
    }

    /**
     * Ruft den Wert der triggered-Eigenschaft ab.
     * 
     */
    public boolean isTriggered() {
        return triggered;
    }

    /**
     * Legt den Wert der triggered-Eigenschaft fest.
     * 
     */
    public void setTriggered(boolean value) {
        this.triggered = value;
    }

    /**
     * Ruft den Wert der containerTriggered-Eigenschaft ab.
     * 
     */
    public boolean isContainerTriggered() {
        return containerTriggered;
    }

    /**
     * Legt den Wert der containerTriggered-Eigenschaft fest.
     * 
     */
    public void setContainerTriggered(boolean value) {
        this.containerTriggered = value;
    }

    /**
     * Ruft den Wert der isBusStop-Eigenschaft ab.
     * 
     */
    public boolean isIsBusStop() {
        return isBusStop;
    }

    /**
     * Legt den Wert der isBusStop-Eigenschaft fest.
     * 
     */
    public void setIsBusStop(boolean value) {
        this.isBusStop = value;
    }

    /**
     * Ruft den Wert der isContainerStop-Eigenschaft ab.
     * 
     */
    public boolean isIsContainerStop() {
        return isContainerStop;
    }

    /**
     * Legt den Wert der isContainerStop-Eigenschaft fest.
     * 
     */
    public void setIsContainerStop(boolean value) {
        this.isContainerStop = value;
    }

}
