
package de.tudresden.ws;

import javax.xml.bind.annotation.XmlAccessType;
import javax.xml.bind.annotation.XmlAccessorType;
import javax.xml.bind.annotation.XmlType;


/**
 * <p>Java-Klasse für Polygon_add complex type.
 * 
 * <p>Das folgende Schemafragment gibt den erwarteten Content an, der in dieser Klasse enthalten ist.
 * 
 * <pre>
 * &lt;complexType name="Polygon_add">
 *   &lt;complexContent>
 *     &lt;restriction base="{http://www.w3.org/2001/XMLSchema}anyType">
 *       &lt;sequence>
 *         &lt;element name="polygonID" type="{http://www.w3.org/2001/XMLSchema}string" minOccurs="0"/>
 *         &lt;element name="shape" type="{http://ws.tudresden.de/}sumoGeometry" minOccurs="0"/>
 *         &lt;element name="color" type="{http://ws.tudresden.de/}sumoColor" minOccurs="0"/>
 *         &lt;element name="fill" type="{http://www.w3.org/2001/XMLSchema}boolean"/>
 *         &lt;element name="polygonType" type="{http://www.w3.org/2001/XMLSchema}string" minOccurs="0"/>
 *         &lt;element name="layer" type="{http://www.w3.org/2001/XMLSchema}int"/>
 *       &lt;/sequence>
 *     &lt;/restriction>
 *   &lt;/complexContent>
 * &lt;/complexType>
 * </pre>
 * 
 * 
 */
@XmlAccessorType(XmlAccessType.FIELD)
@XmlType(name = "Polygon_add", propOrder = {
    "polygonID",
    "shape",
    "color",
    "fill",
    "polygonType",
    "layer"
})
public class PolygonAdd {

    protected String polygonID;
    protected SumoGeometry shape;
    protected SumoColor color;
    protected boolean fill;
    protected String polygonType;
    protected int layer;

    /**
     * Ruft den Wert der polygonID-Eigenschaft ab.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    public String getPolygonID() {
        return polygonID;
    }

    /**
     * Legt den Wert der polygonID-Eigenschaft fest.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setPolygonID(String value) {
        this.polygonID = value;
    }

    /**
     * Ruft den Wert der shape-Eigenschaft ab.
     * 
     * @return
     *     possible object is
     *     {@link SumoGeometry }
     *     
     */
    public SumoGeometry getShape() {
        return shape;
    }

    /**
     * Legt den Wert der shape-Eigenschaft fest.
     * 
     * @param value
     *     allowed object is
     *     {@link SumoGeometry }
     *     
     */
    public void setShape(SumoGeometry value) {
        this.shape = value;
    }

    /**
     * Ruft den Wert der color-Eigenschaft ab.
     * 
     * @return
     *     possible object is
     *     {@link SumoColor }
     *     
     */
    public SumoColor getColor() {
        return color;
    }

    /**
     * Legt den Wert der color-Eigenschaft fest.
     * 
     * @param value
     *     allowed object is
     *     {@link SumoColor }
     *     
     */
    public void setColor(SumoColor value) {
        this.color = value;
    }

    /**
     * Ruft den Wert der fill-Eigenschaft ab.
     * 
     */
    public boolean isFill() {
        return fill;
    }

    /**
     * Legt den Wert der fill-Eigenschaft fest.
     * 
     */
    public void setFill(boolean value) {
        this.fill = value;
    }

    /**
     * Ruft den Wert der polygonType-Eigenschaft ab.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    public String getPolygonType() {
        return polygonType;
    }

    /**
     * Legt den Wert der polygonType-Eigenschaft fest.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setPolygonType(String value) {
        this.polygonType = value;
    }

    /**
     * Ruft den Wert der layer-Eigenschaft ab.
     * 
     */
    public int getLayer() {
        return layer;
    }

    /**
     * Legt den Wert der layer-Eigenschaft fest.
     * 
     */
    public void setLayer(int value) {
        this.layer = value;
    }

}
