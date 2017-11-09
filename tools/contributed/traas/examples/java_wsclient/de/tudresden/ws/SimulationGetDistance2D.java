
package de.tudresden.ws;

import javax.xml.bind.annotation.XmlAccessType;
import javax.xml.bind.annotation.XmlAccessorType;
import javax.xml.bind.annotation.XmlType;


/**
 * <p>Java-Klasse für Simulation_getDistance2D complex type.
 * 
 * <p>Das folgende Schemafragment gibt den erwarteten Content an, der in dieser Klasse enthalten ist.
 * 
 * <pre>
 * &lt;complexType name="Simulation_getDistance2D">
 *   &lt;complexContent>
 *     &lt;restriction base="{http://www.w3.org/2001/XMLSchema}anyType">
 *       &lt;sequence>
 *         &lt;element name="x1" type="{http://www.w3.org/2001/XMLSchema}double"/>
 *         &lt;element name="y1" type="{http://www.w3.org/2001/XMLSchema}double"/>
 *         &lt;element name="x2" type="{http://www.w3.org/2001/XMLSchema}double"/>
 *         &lt;element name="y2" type="{http://www.w3.org/2001/XMLSchema}double"/>
 *         &lt;element name="isGeo" type="{http://www.w3.org/2001/XMLSchema}boolean"/>
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
@XmlType(name = "Simulation_getDistance2D", propOrder = {
    "x1",
    "y1",
    "x2",
    "y2",
    "isGeo",
    "isDriving"
})
public class SimulationGetDistance2D {

    protected double x1;
    protected double y1;
    protected double x2;
    protected double y2;
    protected boolean isGeo;
    protected boolean isDriving;

    /**
     * Ruft den Wert der x1-Eigenschaft ab.
     * 
     */
    public double getX1() {
        return x1;
    }

    /**
     * Legt den Wert der x1-Eigenschaft fest.
     * 
     */
    public void setX1(double value) {
        this.x1 = value;
    }

    /**
     * Ruft den Wert der y1-Eigenschaft ab.
     * 
     */
    public double getY1() {
        return y1;
    }

    /**
     * Legt den Wert der y1-Eigenschaft fest.
     * 
     */
    public void setY1(double value) {
        this.y1 = value;
    }

    /**
     * Ruft den Wert der x2-Eigenschaft ab.
     * 
     */
    public double getX2() {
        return x2;
    }

    /**
     * Legt den Wert der x2-Eigenschaft fest.
     * 
     */
    public void setX2(double value) {
        this.x2 = value;
    }

    /**
     * Ruft den Wert der y2-Eigenschaft ab.
     * 
     */
    public double getY2() {
        return y2;
    }

    /**
     * Legt den Wert der y2-Eigenschaft fest.
     * 
     */
    public void setY2(double value) {
        this.y2 = value;
    }

    /**
     * Ruft den Wert der isGeo-Eigenschaft ab.
     * 
     */
    public boolean isIsGeo() {
        return isGeo;
    }

    /**
     * Legt den Wert der isGeo-Eigenschaft fest.
     * 
     */
    public void setIsGeo(boolean value) {
        this.isGeo = value;
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
