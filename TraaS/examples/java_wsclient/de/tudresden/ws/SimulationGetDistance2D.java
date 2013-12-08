
package de.tudresden.ws;

import javax.xml.bind.annotation.XmlAccessType;
import javax.xml.bind.annotation.XmlAccessorType;
import javax.xml.bind.annotation.XmlType;


/**
 * <p>Java class for Simulation_getDistance2D complex type.
 * 
 * <p>The following schema fragment specifies the expected content contained within this class.
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
 *         &lt;element name="isGeo" type="{http://www.w3.org/2001/XMLSchema}string" minOccurs="0"/>
 *         &lt;element name="isDriving" type="{http://www.w3.org/2001/XMLSchema}string" minOccurs="0"/>
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
    protected String isGeo;
    protected String isDriving;

    /**
     * Gets the value of the x1 property.
     * 
     */
    public double getX1() {
        return x1;
    }

    /**
     * Sets the value of the x1 property.
     * 
     */
    public void setX1(double value) {
        this.x1 = value;
    }

    /**
     * Gets the value of the y1 property.
     * 
     */
    public double getY1() {
        return y1;
    }

    /**
     * Sets the value of the y1 property.
     * 
     */
    public void setY1(double value) {
        this.y1 = value;
    }

    /**
     * Gets the value of the x2 property.
     * 
     */
    public double getX2() {
        return x2;
    }

    /**
     * Sets the value of the x2 property.
     * 
     */
    public void setX2(double value) {
        this.x2 = value;
    }

    /**
     * Gets the value of the y2 property.
     * 
     */
    public double getY2() {
        return y2;
    }

    /**
     * Sets the value of the y2 property.
     * 
     */
    public void setY2(double value) {
        this.y2 = value;
    }

    /**
     * Gets the value of the isGeo property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    public String getIsGeo() {
        return isGeo;
    }

    /**
     * Sets the value of the isGeo property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setIsGeo(String value) {
        this.isGeo = value;
    }

    /**
     * Gets the value of the isDriving property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    public String getIsDriving() {
        return isDriving;
    }

    /**
     * Sets the value of the isDriving property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setIsDriving(String value) {
        this.isDriving = value;
    }

}
