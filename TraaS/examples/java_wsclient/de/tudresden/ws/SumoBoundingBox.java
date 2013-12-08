
package de.tudresden.ws;

import javax.xml.bind.annotation.XmlAccessType;
import javax.xml.bind.annotation.XmlAccessorType;
import javax.xml.bind.annotation.XmlElement;
import javax.xml.bind.annotation.XmlType;


/**
 * <p>Java class for sumoBoundingBox complex type.
 * 
 * <p>The following schema fragment specifies the expected content contained within this class.
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
     * Gets the value of the xMin property.
     * 
     */
    public double getXMin() {
        return xMin;
    }

    /**
     * Sets the value of the xMin property.
     * 
     */
    public void setXMin(double value) {
        this.xMin = value;
    }

    /**
     * Gets the value of the yMin property.
     * 
     */
    public double getYMin() {
        return yMin;
    }

    /**
     * Sets the value of the yMin property.
     * 
     */
    public void setYMin(double value) {
        this.yMin = value;
    }

    /**
     * Gets the value of the xMax property.
     * 
     */
    public double getXMax() {
        return xMax;
    }

    /**
     * Sets the value of the xMax property.
     * 
     */
    public void setXMax(double value) {
        this.xMax = value;
    }

    /**
     * Gets the value of the yMax property.
     * 
     */
    public double getYMax() {
        return yMax;
    }

    /**
     * Sets the value of the yMax property.
     * 
     */
    public void setYMax(double value) {
        this.yMax = value;
    }

}
