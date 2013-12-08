
package de.tudresden.ws;

import javax.xml.bind.annotation.XmlAccessType;
import javax.xml.bind.annotation.XmlAccessorType;
import javax.xml.bind.annotation.XmlType;


/**
 * <p>Java class for Polygon_add complex type.
 * 
 * <p>The following schema fragment specifies the expected content contained within this class.
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
     * Gets the value of the polygonID property.
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
     * Sets the value of the polygonID property.
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
     * Gets the value of the shape property.
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
     * Sets the value of the shape property.
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
     * Gets the value of the color property.
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
     * Sets the value of the color property.
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
     * Gets the value of the fill property.
     * 
     */
    public boolean isFill() {
        return fill;
    }

    /**
     * Sets the value of the fill property.
     * 
     */
    public void setFill(boolean value) {
        this.fill = value;
    }

    /**
     * Gets the value of the polygonType property.
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
     * Sets the value of the polygonType property.
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
     * Gets the value of the layer property.
     * 
     */
    public int getLayer() {
        return layer;
    }

    /**
     * Sets the value of the layer property.
     * 
     */
    public void setLayer(int value) {
        this.layer = value;
    }

}
