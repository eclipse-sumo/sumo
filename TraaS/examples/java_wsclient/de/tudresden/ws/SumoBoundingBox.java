
package de.tudresden.ws;

import javax.xml.bind.annotation.XmlAccessType;
import javax.xml.bind.annotation.XmlAccessorType;
import javax.xml.bind.annotation.XmlElement;
import javax.xml.bind.annotation.XmlType;


/**
 * <p>Java-Klasse für sumoBoundingBox complex type.
 * 
 * <p>Das folgende Schemafragment gibt den erwarteten Content an, der in dieser Klasse enthalten ist.
 * 
 * <pre>
 * &lt;complexType name="sumoBoundingBox">
 *   &lt;complexContent>
 *     &lt;restriction base="{http://www.w3.org/2001/XMLSchema}anyType">
 *       &lt;sequence>
 *         &lt;element name="x_min" type="{http://www.w3.org/2001/XMLSchema}double"/>
 *         &lt;element name="y_min" type="{http://www.w3.org/2001/XMLSchema}double"/>
 *         &lt;element name="x_max" type="{http://www.w3.org/2001/XMLSchema}double"/>
 *         &lt;element name="y_max" type="{http://www.w3.org/2001/XMLSchema}double"/>
 *       &lt;/sequence>
 *     &lt;/restriction>
 *   &lt;/complexContent>
 * &lt;/complexType>
 * </pre>
 * 
 * 
 */
@XmlAccessorType(XmlAccessType.FIELD)
@XmlType(name = "sumoBoundingBox", propOrder = {
    "xMin",
    "yMin",
    "xMax",
    "yMax"
})
public class SumoBoundingBox {

    @XmlElement(name = "x_min")
    protected double xMin;
    @XmlElement(name = "y_min")
    protected double yMin;
    @XmlElement(name = "x_max")
    protected double xMax;
    @XmlElement(name = "y_max")
    protected double yMax;

    /**
     * Ruft den Wert der xMin-Eigenschaft ab.
     * 
     */
    public double getXMin() {
        return xMin;
    }

    /**
     * Legt den Wert der xMin-Eigenschaft fest.
     * 
     */
    public void setXMin(double value) {
        this.xMin = value;
    }

    /**
     * Ruft den Wert der yMin-Eigenschaft ab.
     * 
     */
    public double getYMin() {
        return yMin;
    }

    /**
     * Legt den Wert der yMin-Eigenschaft fest.
     * 
     */
    public void setYMin(double value) {
        this.yMin = value;
    }

    /**
     * Ruft den Wert der xMax-Eigenschaft ab.
     * 
     */
    public double getXMax() {
        return xMax;
    }

    /**
     * Legt den Wert der xMax-Eigenschaft fest.
     * 
     */
    public void setXMax(double value) {
        this.xMax = value;
    }

    /**
     * Ruft den Wert der yMax-Eigenschaft ab.
     * 
     */
    public double getYMax() {
        return yMax;
    }

    /**
     * Legt den Wert der yMax-Eigenschaft fest.
     * 
     */
    public void setYMax(double value) {
        this.yMax = value;
    }

}
