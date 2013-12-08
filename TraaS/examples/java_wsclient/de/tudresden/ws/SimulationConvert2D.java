
package de.tudresden.ws;

import javax.xml.bind.annotation.XmlAccessType;
import javax.xml.bind.annotation.XmlAccessorType;
import javax.xml.bind.annotation.XmlType;


/**
 * <p>Java class for Simulation_convert2D complex type.
 * 
 * <p>The following schema fragment specifies the expected content contained within this class.
 * 
 * <pre>
 * &lt;complexType name="Simulation_convert2D">
 *   &lt;complexContent>
 *     &lt;restriction base="{http://www.w3.org/2001/XMLSchema}anyType">
 *       &lt;sequence>
 *         &lt;element name="edgeID" type="{http://www.w3.org/2001/XMLSchema}string" minOccurs="0"/>
 *         &lt;element name="pos" type="{http://www.w3.org/2001/XMLSchema}double"/>
 *         &lt;element name="laneIndex" type="{http://www.w3.org/2001/XMLSchema}byte"/>
 *         &lt;element name="toGeo" type="{http://www.w3.org/2001/XMLSchema}string" minOccurs="0"/>
 *       &lt;/sequence>
 *     &lt;/restriction>
 *   &lt;/complexContent>
 * &lt;/complexType>
 * </pre>
 * 
 * 
 */
@XmlAccessorType(XmlAccessType.FIELD)
@XmlType(name = "Simulation_convert2D", propOrder = {
    "edgeID",
    "pos",
    "laneIndex",
    "toGeo"
})
public class SimulationConvert2D {

    protected String edgeID;
    protected double pos;
    protected byte laneIndex;
    protected String toGeo;

    /**
     * Gets the value of the edgeID property.
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
     * Sets the value of the edgeID property.
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
     * Gets the value of the pos property.
     * 
     */
    public double getPos() {
        return pos;
    }

    /**
     * Sets the value of the pos property.
     * 
     */
    public void setPos(double value) {
        this.pos = value;
    }

    /**
     * Gets the value of the laneIndex property.
     * 
     */
    public byte getLaneIndex() {
        return laneIndex;
    }

    /**
     * Sets the value of the laneIndex property.
     * 
     */
    public void setLaneIndex(byte value) {
        this.laneIndex = value;
    }

    /**
     * Gets the value of the toGeo property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    public String getToGeo() {
        return toGeo;
    }

    /**
     * Sets the value of the toGeo property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setToGeo(String value) {
        this.toGeo = value;
    }

}
