
package de.tudresden.ws;

import javax.xml.bind.annotation.XmlAccessType;
import javax.xml.bind.annotation.XmlAccessorType;
import javax.xml.bind.annotation.XmlType;


/**
 * <p>Java-Klasse für GUI_setBoundary complex type.
 * 
 * <p>Das folgende Schemafragment gibt den erwarteten Content an, der in dieser Klasse enthalten ist.
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
     * Ruft den Wert der viewID-Eigenschaft ab.
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
     * Legt den Wert der viewID-Eigenschaft fest.
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
     * Ruft den Wert der xmin-Eigenschaft ab.
     * 
     */
    public double getXmin() {
        return xmin;
    }

    /**
     * Legt den Wert der xmin-Eigenschaft fest.
     * 
     */
    public void setXmin(double value) {
        this.xmin = value;
    }

    /**
     * Ruft den Wert der ymin-Eigenschaft ab.
     * 
     */
    public double getYmin() {
        return ymin;
    }

    /**
     * Legt den Wert der ymin-Eigenschaft fest.
     * 
     */
    public void setYmin(double value) {
        this.ymin = value;
    }

    /**
     * Ruft den Wert der xmax-Eigenschaft ab.
     * 
     */
    public double getXmax() {
        return xmax;
    }

    /**
     * Legt den Wert der xmax-Eigenschaft fest.
     * 
     */
    public void setXmax(double value) {
        this.xmax = value;
    }

    /**
     * Ruft den Wert der ymax-Eigenschaft ab.
     * 
     */
    public double getYmax() {
        return ymax;
    }

    /**
     * Legt den Wert der ymax-Eigenschaft fest.
     * 
     */
    public void setYmax(double value) {
        this.ymax = value;
    }

}
