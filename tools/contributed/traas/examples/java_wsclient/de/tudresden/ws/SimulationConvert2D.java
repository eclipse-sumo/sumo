
package de.tudresden.ws;

import javax.xml.bind.annotation.XmlAccessType;
import javax.xml.bind.annotation.XmlAccessorType;
import javax.xml.bind.annotation.XmlType;


/**
 * <p>Java-Klasse für Simulation_convert2D complex type.
 * 
 * <p>Das folgende Schemafragment gibt den erwarteten Content an, der in dieser Klasse enthalten ist.
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
     * Ruft den Wert der toGeo-Eigenschaft ab.
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
     * Legt den Wert der toGeo-Eigenschaft fest.
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
