
package de.tudresden.ws;

import javax.xml.bind.annotation.XmlAccessType;
import javax.xml.bind.annotation.XmlAccessorType;
import javax.xml.bind.annotation.XmlType;


/**
 * <p>Java class for GUI_setBoundary complex type.
 * 
 * <p>The following schema fragment specifies the expected content contained within this class.
 * 
 * <pre>
 * &lt;complexType name="GUI_setBoundary">
 *   &lt;complexContent>
 *     &lt;restriction base="{http://www.w3.org/2001/XMLSchema}anyType">
 *       &lt;sequence>
 *         &lt;element name="viewID" type="{http://www.w3.org/2001/XMLSchema}string" minOccurs="0"/>
 *         &lt;element name="xmin" type="{http://www.w3.org/2001/XMLSchema}double"/>
 *         &lt;element name="ymin" type="{http://www.w3.org/2001/XMLSchema}double"/>
 *         &lt;element name="xmax" type="{http://www.w3.org/2001/XMLSchema}double"/>
 *         &lt;element name="ymax" type="{http://www.w3.org/2001/XMLSchema}double"/>
 *       &lt;/sequence>
 *     &lt;/restriction>
 *   &lt;/complexContent>
 * &lt;/complexType>
 * </pre>
 * 
 * 
 */
@XmlAccessorType(XmlAccessType.FIELD)
@XmlType(name = "GUI_setBoundary", propOrder = {
    "viewID",
    "xmin",
    "ymin",
    "xmax",
    "ymax"
})
public class GUISetBoundary {

    protected String viewID;
    protected double xmin;
    protected double ymin;
    protected double xmax;
    protected double ymax;

    /**
     * Gets the value of the viewID property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    public String getViewID() {
        return viewID;
    }

    /**
     * Sets the value of the viewID property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setViewID(String value) {
        this.viewID = value;
    }

    /**
     * Gets the value of the xmin property.
     * 
     */
    public double getXmin() {
        return xmin;
    }

    /**
     * Sets the value of the xmin property.
     * 
     */
    public void setXmin(double value) {
        this.xmin = value;
    }

    /**
     * Gets the value of the ymin property.
     * 
     */
    public double getYmin() {
        return ymin;
    }

    /**
     * Sets the value of the ymin property.
     * 
     */
    public void setYmin(double value) {
        this.ymin = value;
    }

    /**
     * Gets the value of the xmax property.
     * 
     */
    public double getXmax() {
        return xmax;
    }

    /**
     * Sets the value of the xmax property.
     * 
     */
    public void setXmax(double value) {
        this.xmax = value;
    }

    /**
     * Gets the value of the ymax property.
     * 
     */
    public double getYmax() {
        return ymax;
    }

    /**
     * Sets the value of the ymax property.
     * 
     */
    public void setYmax(double value) {
        this.ymax = value;
    }

}
