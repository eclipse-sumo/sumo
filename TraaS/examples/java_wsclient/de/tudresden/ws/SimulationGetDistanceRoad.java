
package de.tudresden.ws;

import javax.xml.bind.annotation.XmlAccessType;
import javax.xml.bind.annotation.XmlAccessorType;
import javax.xml.bind.annotation.XmlType;


/**
 * <p>Java class for Simulation_getDistanceRoad complex type.
 * 
 * <p>The following schema fragment specifies the expected content contained within this class.
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
    protected String isDriving;

    /**
     * Gets the value of the edgeID1 property.
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
     * Sets the value of the edgeID1 property.
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
     * Gets the value of the pos1 property.
     * 
     */
    public double getPos1() {
        return pos1;
    }

    /**
     * Sets the value of the pos1 property.
     * 
     */
    public void setPos1(double value) {
        this.pos1 = value;
    }

    /**
     * Gets the value of the edgeID2 property.
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
     * Sets the value of the edgeID2 property.
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
     * Gets the value of the pos2 property.
     * 
     */
    public double getPos2() {
        return pos2;
    }

    /**
     * Sets the value of the pos2 property.
     * 
     */
    public void setPos2(double value) {
        this.pos2 = value;
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
